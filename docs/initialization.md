# Recursive Initialization

NDArray supports recursive multidimensional initialization.

---

## Example

```cpp
NDArray<int, 2> matrix {
    {1, 2, 3},
    {4, 5, 6}
};
```

## Higher Dimensions

```cpp
NDArray<int, 3> tensor {
    {
        {1, 2},
        {3, 4}
    },
    {
        {5, 6},
        {7, 8}
    }
};
```

## Design Goals

| Goal                    | Description                   |
| ----------------------- | ----------------------------- |
| Compile-Time Dimensions | Dimensions encoded in type    |
| Predictable Layout      | Contiguous storage            |
| Recursive Composition   | N-dimensional construction    |
| STL-Like Semantics      | Familiar initialization model |

> [!TIP]
> Recursive initialization preserves dimensional correctness at compile time.
