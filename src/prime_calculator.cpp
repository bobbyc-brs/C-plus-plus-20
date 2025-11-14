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

class Semaphore {
    std::mutex mutex_;
    std::condition_variable cv_;
    size_t count_;

public:
    explicit Semaphore(size_t count = 0) : count_(count) {}

    void notify() {
        std::lock_guard<std::mutex> lock(mutex_);
        ++count_;
        cv_.notify_one();
    }

    void wait() {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return count_ > 0; });
        --count_;
    }
};

void PrimeCalculator::calculate_up_to(uint64_t max_number) {
    if (max_number < 2) return;
    
    constexpr size_t batch_size = 1000;
    const size_t max_in_flight = thread_pool->get_thread_count() * 2;
    
    // Semaphore to limit in-flight batches
    Semaphore sem(max_in_flight);
    std::vector<std::future<void>> futures;
    
    // Start from 3, check only odd numbers
    for (uint64_t n = 3; n <= max_number; n += 2) {
        // Wait if we've reached max in-flight batches
        if (futures.size() >= max_in_flight) {
            // Wait for any batch to complete
            auto it = std::find_if(futures.begin(), futures.end(),
                [](const auto& f) { 
                    return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready; 
                });
            
            if (it != futures.end()) {
                it->wait();
                futures.erase(it);
            } else {
                futures.front().wait();
                futures.erase(futures.begin());
            }
        }
        
        // Process a batch of numbers
        uint64_t batch_end = std::min(n + batch_size * 2 - 2, max_number | 1);
        
        futures.push_back(thread_pool->enqueue([this, n, batch_end]() {
            for (uint64_t num = n; num <= batch_end; num += 2) {
                process_number(num);
            }
        }));
        
        n = batch_end; // Skip to the end of this batch
    }
    
    // Wait for all remaining batches to complete
    for (auto& future : futures) {
        future.wait();
    }
    
    // Results are already sorted in the set (no need to sort again)
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
        results.insert(result);
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
    return {results.begin(), results.end()};
}
