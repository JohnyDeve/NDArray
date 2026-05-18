<p align="center">
  <b>English</b> | <a href="./README.ru.md">Русский</a>
</p>

<p align="center">
  <img src="./readme_assets/ndarray-banner.png" alt="NDArray Banner" width="1000">
</p>

<p align="center">

[![C++23](https://img.shields.io/badge/C%2B%2B-00599C?style=flat-square&logo=c%2B%2B&logoColor=white)](https://en.cppreference.com/w/cpp/23)
[![License: MIT](https://img.shields.io/badge/MIT-green?style=flat-square)](https://opensource.org/license/mit)
[![GitHub Action](https://img.shields.io/badge/GitHub_Actions-2088FF?style=flat-square&logo=github-actions&logoColor=white)](https://github.com/JohnyDeve/ndarray-container-cpp/actions)
[![GoogleTest](https://img.shields.io/badge/googletests-25A162?style=flat-square&logo=google&logoColor=white)](https://github.com/google/googletest)
[![clang-format](https://img.shields.io/badge/Code%20Style-clang--format-8A2BE2?style=flat-square&logo=llvm)](https://clang.llvm.org/docs/ClangFormat.html)

![Repo Size](https://img.shields.io/github/repo-size/JohnyDeve/ndarray-container-cpp?style=flat-square)
![Last Commit](https://img.shields.io/github/last-commit/JohnyDeve/ndarray-container-cpp?style=flat-square)
[![CI: format & gtests chekers](https://github.com/JohnyDeve/ndarray-container-cpp/actions/workflows/ci.yml/badge.svg)](https://github.com/JohnyDeve/ndarray-container-cpp/actions/workflows/ci.yml)


</p>

# NDArray

Modern C++ header-only multidimensional contiguous container library focused on deterministic memory behavior, STL-oriented semantics, and low-level object lifetime control.

NDArray provides:

* compile-time dimensionality
* contiguous storage guarantees
* multidimensional traversal abstractions
* iterator/view-based access
* constrained template APIs
* explicit ownership semantics

> [!NOTE]
> NDArray is designed as a systems-oriented container implementation prioritizing predictable memory layout and explicit control over object lifetime.

---

## Features

| Feature                   | Description                                              |
| ------------------------- | -------------------------------------------------------- |
| Contiguous Storage        | All elements are stored inside a contiguous memory block |
| Header-Only               | No separate compilation required                         |
| STL-Oriented Design       | Familiar container semantics and iterator behavior       |
| Recursive Initialization  | Natural multidimensional initialization syntax           |
| Views & Iterators         | Dedicated multidimensional traversal abstractions        |
| Concepts & SFINAE         | Compile-time constrained APIs                            |
| Explicit Lifetime Control | Manual object construction/destruction model             |
| Unit Tests                | GoogleTest-based validation                              |
| CI Pipeline               | GitHub Actions build/test verification                   |

## Quick Example

```cpp
#include <NDArray.hpp>

NDArray<int, 2> matrix {
    {1, 2, 3},
    {4, 5, 6}
};

for (const auto& row : matrix) {
    for (const auto& value : row) {
        std::cout << value << ' ';
    }
}
```

## Views & Iteration

```cpp
NDArray<int, 2> matrix {
    {1, 2, 3},
    {4, 5, 6}
};

NDArrayView<int, 1> first_row = matrix[0];

for (auto& value : first_row) {
    std::cout << value << ' ';
}
```

> [!TIP]
> `NDArrayView` provides lightweight multidimensional traversal without transferring ownership.

---

## Why NDArray Exists

Modern C++ provides powerful low-level memory primitives and generic programming facilities, but multidimensional containers often introduce one or more of the following problems:

* fragmented memory layouts
* runtime-only dimensionality
* abstraction-heavy traversal models
* weak iterator semantics
* hidden allocations
* limited control over object lifetime
* insufficient compile-time guarantees

Many existing solutions prioritize mathematical APIs or high-level usability while sacrificing deterministic low-level behavior and explicit ownership semantics.

NDArray was designed to solve these problems by providing:

| Problem                                 | NDArray Solution                       |
| --------------------------------------- | -------------------------------------- |
| Non-contiguous multidimensional storage | Strict contiguous memory layout        |
| Runtime dimension ambiguity             | Compile-time dimensionality            |
| Expensive traversal abstractions        | Lightweight recursive views            |
| Weak control over allocation behavior   | Explicit lifetime management           |
| STL incompatibility                     | STL-oriented iterator semantics        |
| Difficult multidimensional traversal    | Recursive view-based access            |
| Unsafe partial initialization           | Deterministic construction/destruction |

NDArray is intended for developers working in:

* systems programming
* low-level infrastructure
* performance-oriented applications
* rendering pipelines
* simulation systems
* memory-sensitive environments
* custom engine/tool development

> [!NOTE]
> NDArray is not intended to replace high-level numerical frameworks. The project focuses on predictable low-level behavior, explicit ownership semantics, and systems-oriented container design.

### Why Use NDArray Instead of Traditional Nested Containers?

Traditional nested containers such as:

```cpp
std::vector<std::vector<T>>
```

often introduce:

* fragmented allocations
* poor cache locality
* indirect traversal
* unpredictable memory behavior

NDArray avoids these issues through deterministic contiguous storage and recursive multidimensional traversal abstractions.

### Why Use NDArray Instead of Numerical Frameworks?

Many numerical frameworks prioritize:

* high-level mathematical APIs
* runtime polymorphism
* dynamic shape systems

NDArray instead focuses on:

* low-level memory behavior
* systems-oriented semantics
* compile-time dimensional guarantees
* explicit ownership control
* predictable traversal cost

> [!NOTE]
> NDArray is designed for developers who need fine-grained control over memory layout, traversal semantics, and multidimensional container behavior.

---

## Technical Overview

NDArray is implemented as a header-only multidimensional container with explicit low-level ownership semantics and recursive traversal abstractions.

The implementation focuses on several core engineering aspects that directly influence performance, memory predictability, and container correctness.

### Contiguous Storage Model

All elements are stored inside a single contiguous allocation.

This enables:

* cache-friendly traversal
* deterministic iteration
* predictable pointer arithmetic
* compatibility with low-level algorithms

### Compile-Time Dimensionality

Dimensions are encoded directly into the type system:

```cpp
NDArray<float, 3>
```

This eliminates runtime dimension ambiguity and enables stronger compile-time constraints.

### Recursive View Architecture

Traversal is implemented through recursive multidimensional views:

```cpp
NDArrayView<T, N>
NDArrayConstView<T, N>
```

The view system:

* avoids unnecessary allocations
* preserves contiguous traversal semantics
* enables recursive multidimensional decomposition
* separates ownership from traversal

### Explicit Object Lifetime Management

NDArray explicitly separates:

* raw memory allocation
* object construction
* object destruction
* memory deallocation

This enables:

* deterministic destruction order
* exception-safe partial construction rollback
* allocator-oriented semantics
* predictable ownership behavior

### Constrained Generic APIs

The library actively uses:

* concepts
* requires expressions
* SFINAE constraints

to enforce:

* dimensional correctness
* iterator compatibility
* construction safety
* compile-time API validation

> [!TIP]
> The project intentionally favors explicit low-level semantics over abstraction-heavy convenience layers.

---

## Build

### Configure

```bash
cmake -B build
```

### Build

```bash
cmake --build build
```

### Run Tests

```bash
ctest --test-dir build --output-on-failure
```

---

## Documentation

| Document                                           | Description                             |
| -------------------------------------------------- | --------------------------------------- |
| [Getting Started](./docs/getting-started.md)       | Basic setup and usage                   |
| [Views & Iteration](./docs/iterators-and-views.md) | Iterator and traversal system           |
| [Initialization](./docs/initialization.md)         | Recursive multidimensional construction |
| [Memory Management](./docs/memory-management.md)   | Allocation and lifetime model           |
| [API Reference](./docs/api-reference.md)           | Public API overview                     |
| [Testing & CI](./docs/testing-and-ci.md)           | Tests and GitHub Actions                |

## Testing

The project includes:

* GoogleTest unit tests
* GitHub Actions CI
* clang-format verification

> [!NOTE]
> All tests are executed automatically on every push and pull request.

## License
This project is licensed under the **MIT License**. See the [LICENSE](./LICENSE) file for more details. 

---

## Contacts & Support

### Author

**Kuzyakin Ivan** — C Developer & System Architect  
Feel free to reach out for collaboration or questions regarding the implementation.

[![GitHub](https://img.shields.io/badge/GitHub-100000?style=for-the-badge&logo=github&logoColor=white)](https://github.com/JohnyDeve)
[![LinkedIn](https://img.shields.io/badge/LinkedIn-0077B5?style=for-the-badge&logo=linkedin&logoColor=white)](www.linkedin.com/in/ivan-kuzyakin-1010011100000001010000110000101011011)
[![Telegram](https://img.shields.io/badge/Telegram-2CA5E0?style=for-the-badge&logo=telegram&logoColor=white)](https://t.me/kuzak1n)

### Support the Project
If this tool helped you or saved you some time, you can support its further development:

[![DonationAlerts](https://img.shields.io/badge/DonationAlerts-F57607?style=for-the-badge)](https://www.donationalerts.com/r/kuzakinc)

---
*Made with precision, care and love by JohnyDeve|Kuzak*
