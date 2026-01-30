// src/main.rs
use std::time::Instant;
// use prime_calculator_rs::prime_calculator_rs::PrimeCalculator;
use prime_calculator_rs::PrimeCalculator;

fn main() {
    let max_number = 100 /* _000;*/ ; // Example limit for prime calculation
    let thread_count = 4; // Number of threads to use

    let calculator = PrimeCalculator::new(thread_count);
    let start_time = Instant::now();
    
    calculator.calculate_up_to(max_number);
    
    let duration = start_time.elapsed();
    println!("Calculated primes up to {} in {:?}", max_number, duration);
    
    let primes = calculator.get_primes();
    println!("Found {} primes.", primes.len());
    
    for prime in primes {
        println!("Prime: {} - calculation took {:.6} seconds", prime.number, prime.duration.as_secs_f64());
    }
}