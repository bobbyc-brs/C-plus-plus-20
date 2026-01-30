// This file contains benchmarks for measuring the performance of the prime number calculation functions.

use criterion::{black_box, criterion_group, criterion_main, Criterion};
use prime_calculator_rs::PrimeCalculator;

fn benchmark_prime_calculator(c: &mut Criterion) {
    c.bench_function("calculate primes up to 1,000,000", |b| {
        b.iter(|| {
            let mut calculator = PrimeCalculator::new(4); // Using 4 threads
            calculator.calculate_up_to(black_box(1_000_000));
        })
    });
}

criterion_group!(benches, benchmark_prime_calculator);
criterion_main!(benches);