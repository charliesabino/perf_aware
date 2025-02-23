#pragma once

#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <ostream>
#include <string_view>
#include <thread>

inline auto rdtsc() -> uint64_t {
  uint64_t start_cycles;
  asm volatile("mrs %0, cntvct_el0" : "=r"(start_cycles));
  return start_cycles;
}

inline auto get_cpu_frequency() -> uint64_t {
  using namespace std::literals::chrono_literals;

  auto start_time = std::chrono::high_resolution_clock::now();
  auto start_cycles = rdtsc();

  std::this_thread::sleep_for(100ms);
  auto end_time = std::chrono::high_resolution_clock::now();
  auto end_cycles = rdtsc();

  auto duration = std::chrono::duration<double>{end_time - start_time};
  auto cycles = end_cycles - start_cycles;
  return static_cast<uint64_t>((cycles * 10) / duration.count());
}

struct ProfileAnchor {
  uint64_t tsc_elapsed{};
  uint64_t hit_count{};
  uint64_t bytes_processed{};
  std::string_view label{};
};

struct Profiler {
  std::vector<ProfileAnchor> anchors{};
  uint64_t start_tsc{};
  uint64_t end_tsc{};
  std::atomic<uint32_t> next_index{0};

  auto get_next_index() -> uint32_t {
    auto idx = next_index++;
    if (idx >= anchors.size()) {
      anchors.resize(idx + 1);
    }
    return idx;
  }
};

inline Profiler global_profiler{};

struct ProfileBlock {
  std::string_view label{};
  uint32_t anchor_idx{};
  uint64_t start_tsc{};
  uint64_t bytes_processed{};

  ProfileBlock(std::string_view label, uint32_t anchor_idx,
               uint64_t bytes_processed = 0)
      : label{label}, anchor_idx{anchor_idx}, start_tsc{rdtsc()},
        bytes_processed{bytes_processed} {}

  ~ProfileBlock() {
    auto end_tsc = rdtsc();
    auto &anchor = global_profiler.anchors[anchor_idx];
    anchor.tsc_elapsed += (end_tsc - start_tsc);
    anchor.hit_count++;
    anchor.bytes_processed += bytes_processed;
    anchor.label = label;
  }
};

inline auto print_time_elapsed(uint64_t total_tsc_elapsed,
                               ProfileAnchor &anchor) -> void {
  auto percent =
      100.0 * (static_cast<double>(anchor.tsc_elapsed) / total_tsc_elapsed);
  std::cout << anchor.label << " " << '[' << anchor.hit_count
            << "]: " << anchor.tsc_elapsed << " (" << std::setprecision(2)
            << percent << "%)";
  if (anchor.bytes_processed > 0) {
    auto seconds =
        static_cast<double>(anchor.tsc_elapsed) / get_cpu_frequency();
    auto bandwidth =
        static_cast<double>(anchor.bytes_processed) / (seconds * 1024 * 1024);
    auto mb_processed =
        static_cast<double>(anchor.bytes_processed) / (1024 * 1024);
    std::cout << " - " << std::setprecision(6) << bandwidth << " MB/s ("
              << std::setprecision(2) << mb_processed << " MB total)";
  }
  std::cout << std::endl;
}

inline auto begin_profile() -> void { global_profiler.start_tsc = rdtsc(); }

inline auto end_and_print_profile() -> void {
  global_profiler.end_tsc = rdtsc();
  auto freq = get_cpu_frequency();
  auto total_tsc_elapsed = global_profiler.end_tsc - global_profiler.start_tsc;
  std::cout << "Total time: "
            << static_cast<double>(1000 * total_tsc_elapsed) / freq
            << " (CPU freq: " << freq << ")\n";
  for (auto &anchor : global_profiler.anchors) {
    if (anchor.tsc_elapsed != 0) {
      print_time_elapsed(total_tsc_elapsed, anchor);
    }
  }
}

#define NameConcat2(A, B) A##B
#define NameConcat(A, B) NameConcat2(A, B)
#define TimeBlock(Name, Bytes)                                                 \
  static const uint32_t NameConcat(BlockIndex, __LINE__) =                     \
      global_profiler.get_next_index();                                        \
  ProfileBlock NameConcat(Block, __LINE__)(                                    \
      Name, NameConcat(BlockIndex, __LINE__), Bytes)
#define TimeBandwidth(Bytes) TimeBlock(__func__, Bytes)
#define TimeFunction TimeBlock(__func__, 0)
