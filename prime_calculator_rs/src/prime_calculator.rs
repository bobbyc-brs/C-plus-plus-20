// filepath: /prime_calculator_rs/prime_calculator_rs/src/prime_calculator.rs
use std::sync::{Arc, Mutex};
use std::sync::atomic::{AtomicU64, Ordering};
use std::thread;
// use std::time::{Duration, Instant};
use std::time::{Instant};
use std::collections::HashSet;

use crate::types::PrimeResult;
use crate::thread_pool; // access init/global

pub struct PrimeCalculator {
    primes: Arc<Mutex<Vec<u64>>>,
    results: Arc<Mutex<HashSet<PrimeResult>>>,
    total_calculated: Arc<AtomicU64>,
    thread_pool: Arc<crate::thread_pool::ThreadPool>, // hold Arc to singleton
}

impl PrimeCalculator {
    pub fn new(thread_count: usize) -> Self {
        let primes = Arc::new(Mutex::new(vec![2]));
        let results = Arc::new(Mutex::new(HashSet::new()));
        let total_calculated = Arc::new(AtomicU64::new(0));

        // initialize/get the singleton pool
        let pool = crate::thread_pool::init_global_pool(thread_count);

        PrimeCalculator {
            primes,
            results,
            total_calculated,
            thread_pool: pool,
        }
    }

    pub fn calculate_up_to(&self, max_number: u64) {
        if max_number < 2 {
            return;
        }

        let mut handles = vec![];

        for n in (3..=max_number).step_by(2) {
            let primes = Arc::clone(&self.primes);
            let results = Arc::clone(&self.results);
            let total_calculated = Arc::clone(&self.total_calculated);

            // submit work to a thread pool (assumes `pool` is created earlier in this function)
            self.thread_pool.execute(move || {
                let start = Instant::now();
                let prime = Self::is_prime(n, &primes);
                let duration = start.elapsed();

                let result = PrimeResult::new(n, prime, duration.as_micros() as u64);
                results.lock().unwrap().insert(result);

                if prime {
                    primes.lock().unwrap().push(n);
                }

                total_calculated.fetch_add(1, Ordering::Relaxed);
            });

            handles.push(handle);
        }

        for handle in handles {
            handle.join().unwrap();
        }
    }

    fn is_prime(n: u64, primes: &Arc<Mutex<Vec<u64>>>) -> bool {
        if n <= 1 {
            return false;
        }
        if n == 2 {
            return true;
        }
        if n % 2 == 0 {
            return false;
        }

        let max_divisor = (n as f64).sqrt() as u64;

        let primes = primes.lock().unwrap();
        for &p in primes.iter() {
            if p > max_divisor {
                break;
            }
            if n % p == 0 {
                return false;
            }
        }

        true
    }

    /// Returns a cloned vector of all primes found so far.
    /// This method does not provide a reference to the internal vector,
    /// but instead returns a copy to avoid exposing internal state.
    pub fn get_primes(&self) -> Vec<u64> {
        let primes = self.primes.lock().unwrap();
        primes.clone()
    }
    pub fn get_detailed_results(&self) -> Vec<PrimeResult> {
        let results = self.results.lock().unwrap();
        results.iter().cloned().collect()
    }
}