#pragma once

/**
 * @file benchmark.h
 * @brief Lightweight benchmark framework for performance testing
 *
 * This provides a simple benchmarking framework without external dependencies.
 */

#include <chrono>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

namespace calc::benchmark {

/**
 * @brief Benchmark result containing timing statistics
 */
struct BenchmarkResult {
    std::string name;     ///< Name of the benchmark
    double mean_ns;       ///< Mean execution time in nanoseconds
    double median_ns;     ///< Median execution time in nanoseconds
    double min_ns;        ///< Minimum execution time in nanoseconds
    double max_ns;        ///< Maximum execution time in nanoseconds
    double stddev_ns;     ///< Standard deviation in nanoseconds
    uint64_t iterations;  ///< Number of iterations performed
    double ops_per_sec;   ///< Operations per second

    /**
     * @brief Format the result as a human-readable string
     */
    std::string format() const {
        std::ostringstream oss;
        oss << name << "\n";
        oss << "  Iterations:    " << iterations << "\n";
        oss << "  Mean:          " << format_time(mean_ns) << "\n";
        oss << "  Median:        " << format_time(median_ns) << "\n";
        oss << "  Min:           " << format_time(min_ns) << "\n";
        oss << "  Max:           " << format_time(max_ns) << "\n";
        oss << "  StdDev:        " << format_time(stddev_ns) << "\n";
        oss << "  Ops/sec:       " << static_cast<uint64_t>(ops_per_sec) << "\n";
        return oss.str();
    }

   private:
    static std::string format_time(double nanos) {
        if (nanos < 1000.0) {
            return std::to_string(static_cast<int>(nanos)) + " ns";
        } else if (nanos < 1'000'000.0) {
            return std::to_string(nanos / 1000.0) + " µs";
        } else if (nanos < 1'000'000'000.0) {
            return std::to_string(nanos / 1'000'000.0) + " ms";
        } else {
            return std::to_string(nanos / 1'000'000'000.0) + " s";
        }
    }
};

/**
 * @brief Benchmark configuration
 */
struct BenchmarkConfig {
    uint64_t warmup_iterations = 10;      ///< Number of warmup iterations (not timed)
    uint64_t min_iterations = 100;        ///< Minimum number of timed iterations
    uint64_t max_iterations = 1'000'000;  ///< Maximum number of timed iterations
    double min_duration_ms = 100.0;       ///< Minimum duration in milliseconds
    double max_duration_ms = 5000.0;      ///< Maximum duration in milliseconds
};

/**
 * @class Benchmark
 * @brief Single benchmark test
 *
 * Usage:
 * @code
 *   Benchmark b("Parser Complex Expression");
 *   b.run([] {
 *       parser.parse("(1+2)*(3-4)/(5^6)");
 *   });
 *   b.print();
 * @endcode
 */
class Benchmark {
   public:
    explicit Benchmark(std::string name, BenchmarkConfig config = {})
        : name_(std::move(name)), config_(std::move(config)) {}

    /**
     * @brief Run the benchmark with a function that takes no arguments
     */
    template <typename Func>
    BenchmarkResult run(Func&& func) {
        std::vector<double> timings;
        timings.reserve(config_.max_iterations);

        // Warmup phase
        for (uint64_t i = 0; i < config_.warmup_iterations; ++i) {
            func();
        }

        auto total_start = std::chrono::high_resolution_clock::now();
        auto total_elapsed = std::chrono::duration<double, std::milli>(0);

        // Timed phase
        uint64_t iteration = 0;
        while (iteration < config_.min_iterations ||
               total_elapsed.count() < config_.min_duration_ms) {

            if (iteration >= config_.max_iterations ||
                total_elapsed.count() >= config_.max_duration_ms) {
                break;
            }

            auto start = std::chrono::high_resolution_clock::now();
            func();
            auto end = std::chrono::high_resolution_clock::now();

            auto duration = std::chrono::duration<double, std::nano>(end - start).count();
            timings.push_back(duration);

            total_elapsed = std::chrono::duration<double, std::milli>(
                std::chrono::high_resolution_clock::now() - total_start);
            ++iteration;
        }

        return compute_result(timings, iteration);
    }

    /**
     * @brief Run the benchmark with a function that takes an index
     * Useful for parametrized benchmarks
     */
    template <typename Func>
    BenchmarkResult run_with_index(Func&& func) {
        std::vector<double> timings;
        timings.reserve(config_.max_iterations);

        // Warmup phase
        for (uint64_t i = 0; i < config_.warmup_iterations; ++i) {
            func(i);
        }

        auto total_start = std::chrono::high_resolution_clock::now();
        auto total_elapsed = std::chrono::duration<double, std::milli>(0);

        // Timed phase
        uint64_t iteration = 0;
        while (iteration < config_.min_iterations ||
               total_elapsed.count() < config_.min_duration_ms) {

            if (iteration >= config_.max_iterations ||
                total_elapsed.count() >= config_.max_duration_ms) {
                break;
            }

            auto start = std::chrono::high_resolution_clock::now();
            func(iteration);
            auto end = std::chrono::high_resolution_clock::now();

            auto duration = std::chrono::duration<double, std::nano>(end - start).count();
            timings.push_back(duration);

            total_elapsed = std::chrono::duration<double, std::milli>(
                std::chrono::high_resolution_clock::now() - total_start);
            ++iteration;
        }

        return compute_result(timings, iteration);
    }

    /**
     * @brief Run a comparison between two functions
     */
    template <typename Func1, typename Func2>
    void compare(const std::string& name1, Func1&& func1, const std::string& name2, Func2&& func2) {
        Benchmark b1(name_ + " - " + name1, config_);
        Benchmark b2(name_ + " - " + name2, config_);

        auto r1 = b1.run(func1);
        auto r2 = b2.run(func2);

        std::cout << r1.format();
        std::cout << r2.format();

        double speedup = r2.mean_ns / r1.mean_ns;
        std::cout << "  Speedup:        ";
        if (speedup > 1.0) {
            std::cout << name1 << " is " << speedup << "x faster than " << name2 << "\n";
        } else {
            std::cout << name2 << " is " << (1.0 / speedup) << "x faster than " << name1 << "\n";
        }
        std::cout << "\n";
    }

    /**
     * @brief Print the result to stdout
     */
    void print_result(const BenchmarkResult& result) const { std::cout << result.format() << "\n"; }

    const std::string& name() const { return name_; }

   private:
    BenchmarkResult compute_result(const std::vector<double>& timings, uint64_t iterations) {
        BenchmarkResult result;
        result.name = name_;
        result.iterations = iterations;

        if (timings.empty()) {
            return result;
        }

        // Sort for median calculation
        std::vector<double> sorted = timings;
        std::sort(sorted.begin(), sorted.end());

        // Min/Max
        result.min_ns = sorted.front();
        result.max_ns = sorted.back();

        // Median
        size_t n = sorted.size();
        if (n % 2 == 0) {
            result.median_ns = (sorted[n / 2 - 1] + sorted[n / 2]) / 2.0;
        } else {
            result.median_ns = sorted[n / 2];
        }

        // Mean and StdDev
        double sum = std::accumulate(sorted.begin(), sorted.end(), 0.0);
        result.mean_ns = sum / n;

        double variance = 0.0;
        for (double t : sorted) {
            double diff = t - result.mean_ns;
            variance += diff * diff;
        }
        result.stddev_ns = std::sqrt(variance / n);

        // Operations per second
        if (result.mean_ns > 0) {
            result.ops_per_sec = 1'000'000'000.0 / result.mean_ns;
        }

        return result;
    }

    std::string name_;
    BenchmarkConfig config_;
};

/**
 * @class BenchmarkSuite
 * @brief Collection of benchmarks
 */
class BenchmarkSuite {
   public:
    explicit BenchmarkSuite(std::string name) : name_(std::move(name)) {}

    /**
     * @brief Add a benchmark to the suite
     */
    template <typename Func>
    void add(const std::string& benchmark_name, Func&& func) {
        benchmarks_.emplace_back(benchmark_name);
        benchmarks_.back().run(func);
    }

    /**
     * @brief Run all benchmarks and print results
     */
    void run_all() {
        std::cout << "========================================\n";
        std::cout << "Benchmark Suite: " << name_ << "\n";
        std::cout << "========================================\n\n";

        for (auto& bench : benchmarks_) {
            auto result = bench.run([&] { /* result already cached */ });
            bench.print_result(result);
        }

        std::cout << "========================================\n";
    }

   private:
    std::string name_;
    std::vector<Benchmark> benchmarks_;
};

/**
 * @brief Helper macro to define a benchmark
 */
#define BENCHMARK(name) calc::benchmark::Benchmark _bench_##name(#name)

/**
 * @brief Helper macro to define a benchmark suite
 */
#define BENCHMARK_SUITE(name) calc::benchmark::BenchmarkSuite _suite_##name(#name)

}  // namespace calc::benchmark
