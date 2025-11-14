#include "prime_calculator.hpp"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>

void print_usage() {
    std::cout << "Usage: prime_calculator <max_number> [thread_count]\n"
              << "  max_number:  Calculate primes up to this number (required, minimum 2)\n"
              << "  thread_count: Number of worker threads (optional, defaults to hardware concurrency)\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage();
        return 1;
    }

    try {
        uint64_t max_number = std::stoull(argv[1]);
        if (max_number < 2) {
            std::cerr << "Error: max_number must be at least 2\n";
            return 1;
        }

        size_t thread_count = std::thread::hardware_concurrency();
        if (argc > 2) {
            thread_count = std::stoul(argv[2]);
            if (thread_count == 0) {
                std::cerr << "Error: thread_count must be at least 1\n";
                return 1;
            }
        }

        std::cout << "Calculating primes up to " << max_number 
                  << " using " << thread_count << " threads...\n";

        auto start_time = std::chrono::high_resolution_clock::now();
        
        PrimeCalculator calculator(thread_count);
        calculator.calculate_up_to(max_number);
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            end_time - start_time);

        // Get results
        auto primes = calculator.get_primes();
        auto results = calculator.get_detailed_results();
        
        // Print summary
        std::cout << "\n=== Prime Number Calculation Complete ===\n"
                  << "Range: 1 - " << max_number << "\n"
                  << "Numbers checked: " << results.size() + 1 << "\n"
                  << "Primes found: " << primes.size() << "\n"
                  << "Total calculation time: " << duration.count() << " ms\n";

        // Print the first and last 5 primes
        const size_t max_primes_to_show = 5;
        std::cout << "\nFirst " << std::min(primes.size(), max_primes_to_show) 
                  << " primes: ";
        for (size_t i = 0; i < std::min(primes.size(), max_primes_to_show); ++i) {
            std::cout << primes[i];
            if (i < std::min(primes.size(), max_primes_to_show) - 1) {
                std::cout << ", ";
            }
        }
        
        if (primes.size() > max_primes_to_show) {
            std::cout << "\nLast " << max_primes_to_show << " primes: ";
            for (size_t i = primes.size() - std::min(primes.size(), max_primes_to_show); 
                 i < primes.size(); ++i) {
                std::cout << primes[i];
                if (i < primes.size() - 1) {
                    std::cout << ", ";
                }
            }
        }
        
        // Print detailed timing for the first 10 numbers
        std::cout << "\n\n=== Detailed Timing (first 100 numbers) ===\n";
        std::cout << "Number  | Prime? | Time (µs)\n";
        std::cout << "--------|--------|-----------\n";
        
        size_t count = std::min<size_t>(100 results.size());
        for (size_t i = 0; i < count; ++i) {
            const auto& result = results[i];
            std::cout << std::setw(7) << result.number << " | "
                      << std::setw(6) << (result.is_prime ? "yes" : "no") << " | "
                      << result.calculation_time.count() << "\n";
        }
        
        std::cout << "\nCalculation complete!\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
