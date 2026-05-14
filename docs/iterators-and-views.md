
# Iterators & Views

NDArray implements a dedicated multidimensional traversal system based on reusable iterator/view abstractions.

* `NDArrayView`        -\
                         |>- `GeneralNDArrayView` 
* `NDArrayConstView`   -/

---

## General View Abstraction

Traversal behavior is centralized inside:

```cpp
template<typename T, size_t N, bool IsConstClass>
class GeneralNDArrayView;
```

This abstraction provides the foundation for:

* mutable multidimensional views
* const multidimensional views
* recursive dimensional traversal
* iterator-compatible navigation

## NDArrayView

`NDArrayView` provides mutable access to multidimensional subregions.

Characteristics:

* lightweight abstraction
* non-owning traversal interface
* recursive dimensional access
* contiguous traversal semantics

```cpp
NDArray<int, 2> matrix {
    {1, 2, 3},
    {4, 5, 6}
};

NDArrayView<int, 1> row = matrix[0];

for (auto& value : row) {
    std::cout << value << ' ';
}
```

## NDArrayConstView

`NDArrayConstView` provides const-correct traversal behavior.

The implementation shares the same generalized traversal model while restricting mutation.

## Iterator Model

| Property          | Description      |
| ----------------- | ---------------- |
| Ownership         | Non-owning       |
| Traversal         | Multidimensional |
| Access            | Recursive        |
| Layout            | Contiguous       |
| Const Correctness | Supported        |

> [!NOTE]
> Views do not allocate memory and do not own storage.

---

# Design Motivation

The view system exists to:

* avoid duplicated traversal logic
* preserve const correctness
* centralize iterator semantics
* minimize abstraction overhead

---