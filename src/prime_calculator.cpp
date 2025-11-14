#include "prime_calculator.hpp"
#include "thread_pool.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>

using namespace std::chrono;

std::string PrimeResult::to_string() const {
    std::ostringstream ss;
    ss << number << " is " << (is_prime ? "prime" : "not prime") 
       << " (calculated in " << calculation_time.count() << " µs)";
    return ss.str();
}

PrimeCalculator::PrimeCalculator(size_t thread_count)
    : thread_pool(std::make_unique<::ThreadPool>(thread_count)) {
    // Add the first prime number
    std::lock_guard<std::mutex> lock(primes_mutex);
    primes.push_back(2);
    primes_found = 1;
}

void PrimeCalculator::calculate_up_to(uint64_t max_number) {
    if (max_number < 2) return;
    
    // Number of numbers each worker should process in one batch
    constexpr size_t batch_size = 1000;
    
    // Start from 3, check only odd numbers
    uint64_t current = 3;
    std::vector<std::future<void>> futures;
    
    // Create initial batch of work
    while (current <= max_number) {
        uint64_t batch_start = current;
        uint64_t batch_end = std::min(current + batch_size * 2, max_number | 1);
        
        futures.push_back(thread_pool->enqueue([this, batch_start, batch_end]() {
            for (uint64_t n = batch_start; n <= batch_end; n += 2) {
                process_number(n);
            }
        }));
        
        current = batch_end + 2;
        
        // Limit the number of in-flight batches to 2x the number of threads
        if (futures.size() >= thread_pool->get_thread_count() * 2) {
            // Wait for the first batch to complete
            futures.front().wait();
            futures.erase(futures.begin());
        }
    }
    
    // Wait for all remaining batches to complete
    for (auto& future : futures) {
        future.wait();
    }
    
    // Sort the results by number
    std::lock_guard<std::mutex> lock(results_mutex);
    std::sort(results.begin(), results.end(), 
        [](const PrimeResult& a, const PrimeResult& b) {
            return a.number < b.number;
        });
}

bool PrimeCalculator::is_prime(uint64_t n) const {
    // Handle edge cases
    if (n <= 1) return false;
    if (n == 2) return true;
    if (std::has_single_bit(n) && n > 2) return false;  // Check if n is a power of 2 > 2
    
    uint64_t max_divisor = static_cast<uint64_t>(std::sqrt(n)) + 1;
    
    // Check against all primes up to sqrt(n)
    std::lock_guard<std::mutex> lock(primes_mutex);
    for (uint64_t p : primes) {
        if (p > max_divisor) break;
        if (n % p == 0) return false;
    }
    
    return true;
}

void PrimeCalculator::process_number(uint64_t n) {
    auto start = high_resolution_clock::now();
    
    bool prime = is_prime(n);
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);
    
    PrimeResult result{n, prime, duration};
    
    {
        std::lock_guard<std::mutex> lock(results_mutex);
        results.push_back(result);
    }
    
    if (prime) {
        std::lock_guard<std::mutex> lock(primes_mutex);
        primes.push_back(n);
        primes_found++;
    }
    
    total_calculated++;
}

std::vector<uint64_t> PrimeCalculator::get_primes() const {
    std::lock_guard<std::mutex> lock(primes_mutex);
    return primes;
}

std::vector<PrimeResult> PrimeCalculator::get_detailed_results() const {
    std::lock_guard<std::mutex> lock(results_mutex);
    return results;
}
