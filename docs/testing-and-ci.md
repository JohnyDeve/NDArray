# Testing & Continuous Integration

NDArray includes a full validation pipeline based on GoogleTest and GitHub Actions.

---

## CI Pipeline

The GitHub Actions workflow automatically performs:

| Stage      | Description               |
| ---------- | ------------------------- |
| Configure  | CMake configuration       |
| Build      | Project compilation       |
| Tests      | GoogleTest execution      |
| Formatting | clang-format verification |

> [!NOTE]
> All validation steps run automatically on every push and pull request.

---

## Unit Tests

Tests are located in:

`tests/tests.cpp`

The test suite validates:

* iterator behavior
* recursive initialization
* copy/move semantics
* contiguous traversal
* const-correctness
* exception-sensitive operations

## GoogleTest Integration

The project uses GoogleTest through CMake FetchContent integration.

Tests are automatically registered using:

```cmake
include(GoogleTest)
gtest_discover_tests(ndarray_tests)
```

## Running Tests

```bash
ctest --test-dir build --output-on-failure
```

## clang-format Verification

Formatting consistency is verified automatically in CI.

Example:

```bash
clang-format --dry-run --Werror include/NDArray.hpp tests/tests.cpp
```
