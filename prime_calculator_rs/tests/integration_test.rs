// integration_test.rs
use prime_calculator_rs::PrimeCalculator;

#[test]
fn test_prime_calculator() {
    let max_number = 100;
    let calculator = PrimeCalculator::new(4); // Assuming 4 threads for the thread pool
    calculator.calculate_up_to(max_number);
    
    let primes = calculator.get_primes();
    let expected_primes: Vec<u64> = vec![2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97];
    
    assert_eq!(primes, expected_primes);
}

#[test]
fn test_no_primes_below_two() {
    let calculator = PrimeCalculator::new(4);
    calculator.calculate_up_to(1);
    
    let primes = calculator.get_primes();
    assert!(primes.is_empty());
}

#[test]
fn test_large_prime_calculation() {
    let max_number = 1_000_000;
    let calculator = PrimeCalculator::new(8); // Using 8 threads for larger calculations
    calculator.calculate_up_to(max_number);
    
    let primes = calculator.get_primes();
    assert!(primes.len() > 0);
    assert!(primes.iter().all(|&p| p.is_prime())); // Assuming is_prime() method exists
}