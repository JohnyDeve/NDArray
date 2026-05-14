# Getting Started

## Requirements

| Requirement  | Version            |
| ------------ | ------------------ |
| C++ Standard | C++20              |
| CMake        | 3.16+              |
| Compiler     | GCC / Clang / MSVC |

## Build

```bash
cmake -B build
cmake --build build
```

## Running Tests

```bash
ctest --test-dir build --output-on-failure
```

## Basic Usage

```cpp
NDArray<int, 2> matrix {
    {1, 2, 3},
    {4, 5, 6}
};
```

## Iteration

```cpp
for (const auto& row : matrix) {
    for (const auto& value : row) {
        std::cout << value << ' ';
    }
}
```

## Accessing Raw Storage

```cpp
int* ptr = matrix.data();
```

> [!TIP]
> NDArray guarantees contiguous storage semantics.
