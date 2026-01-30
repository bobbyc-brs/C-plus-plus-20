# Prime Number Calculator

This project is a Rust implementation of a prime number calculator. It utilizes a multi-threaded approach to efficiently calculate prime numbers up to a specified limit. The project includes a thread pool for managing concurrent tasks and provides detailed results of the calculations.

## Features

- Multi-threaded prime number calculation
- Efficient handling of large numbers
- Detailed results including calculation time for each number
- Easy integration with other Rust projects

## Usage

To use the prime number calculator, follow these steps:

1. Clone the repository:
   ```
   git clone <repository-url>
   cd prime_calculator_rs
   ```

2. Build the project:
   ```
   cargo build
   ```

3. Run the application:
   ```
   cargo run -- <max_number>
   ```
   Replace `<max_number>` with the upper limit for prime number calculation.

4. To run benchmarks:
   ```
   cargo bench
   ```

5. To run tests:
   ```
   cargo test
   ```

## Project Structure

- `Cargo.toml`: Configuration file for the Rust project.
- `.gitignore`: Specifies files and directories to be ignored by Git.
- `src/main.rs`: Entry point of the application.
- `src/lib.rs`: Library interface for the project.
- `src/prime_calculator.rs`: Implementation of the `PrimeCalculator` struct.
- `src/thread_pool.rs`: Implementation of a thread pool for concurrent execution.
- `src/types.rs`: Definitions of types and structures used in the project.
- `benches/bench_primes.rs`: Benchmarks for measuring performance.
- `tests/integration_test.rs`: Integration tests for the project.

## Contributing

Contributions are welcome! Please open an issue or submit a pull request for any improvements or bug fixes.

## License

This project is licensed under the MIT License. See the LICENSE file for more details.