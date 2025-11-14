# C++20 Prime Number Calculator

A high-performance prime number calculator implemented in modern C++20, featuring:

- Multi-threaded calculation using a thread pool
- Efficient prime checking algorithm
- Detailed timing information for each calculation
- Progress tracking and statistics
- Clean, modern C++20 implementation

## Building

### Prerequisites

- CMake 3.20 or higher
- C++20 compatible compiler (GCC 10+, Clang 10+, MSVC 2019 16.10+)

### Build Instructions

```bash
mkdir -p build && cd build
cmake ..
cmake --build .
```

## Usage

```bash
./prime_calculator <max_number> [thread_count]
```

- `max_number`: Calculate primes up to this number (required, minimum 2)
- `thread_count`: Number of worker threads (optional, defaults to hardware concurrency)

### Examples

Calculate primes up to 1000 using all available CPU cores:
```bash
./prime_calculator 1000
```

Calculate primes up to 1,000,000 using 8 threads:
```bash
./prime_calculator 1000000 8
```

## Features

- **Thread Pool**: Efficiently manages worker threads for parallel computation
- **Optimized Prime Checking**: Uses trial division up to square root with memoization
- **Detailed Statistics**: Tracks calculation time for each number
- **Progress Tracking**: Shows progress during long-running calculations
- **Clean Architecture**: Separates concerns with clear interfaces

## Implementation Details

- Uses C++20 features like concepts, ranges, and modern memory management
- Thread-safe data structures for concurrent access
- RAII for resource management
- Exception-safe design

## Performance

The implementation is optimized for performance:
- Parallel processing of number checks
- Memoization of previously found primes
- Efficient data structures for fast lookups
- Minimal locking for thread safety

## License

This project is open source and available under the MIT License.
