#pragma once

#include <vector>
#include <set>
#include <mutex>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <string>
#include <memory>
#include <thread>
#include <functional>
#include "thread_pool.hpp"

struct PrimeResult {
    uint64_t number;
    bool is_prime;
    std::chrono::microseconds calculation_time;
    
    bool operator<(const PrimeResult& other) const {
        return number < other.number;
    }
    
    std::string to_string() const;
};

class PrimeCalculator {
public:
    explicit PrimeCalculator(size_t thread_count = 0);
    ~PrimeCalculator() = default;
    
    // Non-copyable, non-movable
    PrimeCalculator(const PrimeCalculator&) = delete;
    PrimeCalculator& operator=(const PrimeCalculator&) = delete;
    PrimeCalculator(PrimeCalculator&&) = delete;
    PrimeCalculator& operator=(PrimeCalculator&&) = delete;
    
    // Start calculating primes up to the specified number
    void calculate_up_to(uint64_t max_number);
    
    // Get all found prime numbers
    std::vector<uint64_t> get_primes() const;
    
    // Get calculation statistics
    size_t get_total_calculated() const { return total_calculated.load(); }
    size_t get_primes_found() const { return primes_found.load(); }
    
    // Get detailed results for all numbers checked
    std::vector<PrimeResult> get_detailed_results() const;
    
private:
    bool is_prime(uint64_t n) const;
    void process_number(uint64_t n);
    
    std::vector<uint64_t> primes;
    mutable std::mutex primes_mutex;
    std::set<PrimeResult> results;
    mutable std::mutex results_mutex;
    
    std::atomic<size_t> total_calculated{0};
    std::atomic<size_t> primes_found{0};
    
    std::unique_ptr<::ThreadPool> thread_pool;
};
