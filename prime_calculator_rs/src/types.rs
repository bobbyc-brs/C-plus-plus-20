// This file defines types and structures used throughout the project, such as `PrimeResult` for storing results of prime number calculations.

use std::fmt;

#[derive(Debug, Clone, PartialEq, Eq, Hash)]
pub struct PrimeResult {
    pub number: u64,
    pub is_prime: bool,
    /// microseconds
    pub calculation_time_micros: u64,
}

impl PrimeResult {
    pub fn new(number: u64, is_prime: bool, calculation_time_micros: u64) -> Self {
        Self {
            number,
            is_prime,
            calculation_time_micros,
        }
    }

    pub fn to_string(&self) -> String {
        format!(
            "{} is {} (calculated in {} µs)",
            self.number,
            if self.is_prime { "prime" } else { "not prime" },
            self.calculation_time_micros
        )
    }
}

impl fmt::Display for PrimeResult {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}", self.to_string())
    }
}