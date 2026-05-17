# API Reference

# Container

```cpp
template<typename T, size_t N>
class NDArray;
```

`NDArray` is a multidimensional contiguous container with compile-time dimensionality and STL-oriented traversal semantics.

---

# Constructors

## Default Constructor

```cpp
NDArray() noexcept
```

Constructs an empty NDArray.

### Behavior

* creates container without allocated storage
* initializes dimensions with zero values
* does not perform element construction

### Purpose

Used for:

* deferred initialization
* move semantics
* empty-state construction
* container reassignment workflows

### Example

```cpp
NDArray<int, 2> matrix;
```

> [!NOTE]
> The constructor is `noexcept` and does not allocate memory.

---

## Initializer List Constructor (1D)

```cpp
NDArray(std::initializer_list<T> initializer)
    requires(N == 1)
```

Constructs a one-dimensional NDArray from an initializer list.

### Parameters

| Parameter   | Description             |
| ----------- | ----------------------- |
| initializer | Source element sequence |

### Example

```cpp
NDArray<int, 1> vector {1, 2, 3, 4};
```

### Behavior

* allocates contiguous storage
* constructs elements sequentially
* preserves insertion order

### Purpose

Provides natural STL-style initialization syntax for one-dimensional containers.

---

## Recursive Initializer List Constructor

```cpp
NDArray(std::initializer_list<NDArray<T, N - 1>> initializer)
    requires(N > 1)
```

Constructs a multidimensional NDArray using recursive nested initialization.

### Parameters

| Parameter   | Description                         |
| ----------- | ----------------------------------- |
| initializer | Nested NDArray initializer sequence |

### Example

```cpp
NDArray<int, 2> matrix {
    {1, 2, 3},
    {4, 5, 6}
};
```

### Higher-Dimensional Example

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

### Behavior

* recursively constructs lower-dimensional NDArrays
* preserves contiguous memory layout
* validates dimensional consistency
* performs deterministic element construction

### Purpose

Provides expressive multidimensional initialization while preserving compile-time dimensional guarantees.

> [!TIP]
> Recursive initialization enables natural tensor-like construction syntax without runtime dimension ambiguity.


## Variadic Value Constructor

```cpp
template<typename... Args>
explicit NDArray(Args... args)
```

Constructs an NDArray using variadic arguments where:

* the first `N` arguments describe dimensions
* the last argument is used as the initial value

### Parameters

| Parameter | Description                                 |
| --------- | ------------------------------------------- |
| args      | Dimensions followed by initialization value |

### Example

```cpp
NDArray<int, 2> matrix(3, 4, 0);
```

Creates:

* 3x4 matrix
* initialized with `0`

> [!NOTE]
> The constructor is constrained using concepts/requires expressions to guarantee compile-time dimensional correctness.

---

## View Constructor

```cpp
template<typename VT, size_t VN>
NDArray(const NDArrayView<VT, VN>& ndview)
```

Constructs an NDArray by copying data from an `NDArrayView`.

### Parameters

| Parameter | Description                  |
| --------- | ---------------------------- |
| ndview    | Source multidimensional view |

### Purpose

Allows:

* materialization of non-owning views
* copying multidimensional subregions
* conversion from traversal abstractions to owning containers

---

## Iterator Constructor

```cpp
template<typename Iterator>
NDArray(
    Iterator first,
    Iterator last,
    std::array<size_type, N> dims = {}
)
```

Constructs an NDArray from an iterator range.

### Parameters

| Parameter | Description                       |
| --------- | --------------------------------- |
| first     | Iterator to first element         |
| last      | Iterator to end sentinel          |
| dims      | Dimensions of resulting container |

### Purpose

Provides STL-style range construction semantics.

### Example

```cpp
std::vector<int> values {1, 2, 3, 4};

NDArray<int, 2> matrix(
    values.begin(),
    values.end(),
    {2, 2}
);
```

---

## Array Dimension Constructor

```cpp
NDArray(const std::array<size_type, N>& dimensions)
```

Constructs an NDArray using explicit dimension metadata.

### Parameters

| Parameter  | Description                          |
| ---------- | ------------------------------------ |
| dimensions | Array describing each dimension size |

### Example

```cpp
NDArray<int, 3> tensor({4, 5, 6});
```

---

## Variadic Dimension Constructor

```cpp
template<typename... Dims>
explicit NDArray(Dims... dims)
```

Constructs an NDArray using variadic dimension arguments.

### Parameters

| Parameter | Description     |
| --------- | --------------- |
| dims      | Dimension sizes |

### Example

```cpp
NDArray<int, 2> matrix(3, 4);
```

Creates:

* 3x4 multidimensional container

---

## Copy Constructor

```cpp
NDArray(const NDArray& other)
```

Performs deep-copy construction.

### Parameters

| Parameter | Description      |
| --------- | ---------------- |
| other     | Source container |

### Behavior

* allocates new storage
* copies all elements
* preserves dimensions

---

## Move Constructor

```cpp
NDArray(NDArray&& other) noexcept
```

Transfers ownership from another NDArray.

### Parameters

| Parameter | Description      |
| --------- | ---------------- |
| other     | Source container |

### Behavior

* transfers ownership
* avoids deep copy
* leaves source in valid moved-from state

---

# Assignment Operators

## Copy Assignment

```cpp
NDArray& operator=(const NDArray& other)
```

Performs deep-copy replacement.

### Parameters

| Parameter | Description      |
| --------- | ---------------- |
| other     | Source container |

### Returns

| Type     | Description                    |
| -------- | ------------------------------ |
| NDArray& | Reference to current container |

---

## Move Assignment

```cpp
NDArray& operator=(NDArray&& other) noexcept
```

Transfers ownership from another container.

### Parameters

| Parameter | Description      |
| --------- | ---------------- |
| other     | Source container |

### Returns

| Type     | Description                    |
| -------- | ------------------------------ |
| NDArray& | Reference to current container |

---

# Element Access

## operator[] (1D)

```cpp
reference operator[](size_type index) const&
```

Provides direct access to elements for one-dimensional containers.

### Parameters

| Parameter | Description   |
| --------- | ------------- |
| index     | Element index |

### Returns

| Type      | Description          |
| --------- | -------------------- |
| reference | Reference to element |

---

## operator[] (Multidimensional Mutable)

```cpp
auto operator[](size_type index) noexcept
```

Returns a mutable multidimensional view.

### Parameters

| Parameter | Description     |
| --------- | --------------- |
| index     | Dimension index |

### Returns

| Type        | Description                   |
| ----------- | ----------------------------- |
| NDArrayView | Mutable multidimensional view |

### Purpose

Provides recursive multidimensional traversal.

---

## operator[] (Multidimensional Const)

```cpp
auto operator[](size_type index) const noexcept
```

Returns a const multidimensional view.

### Parameters

| Parameter | Description     |
| --------- | --------------- |
| index     | Dimension index |

### Returns

| Type             | Description                 |
| ---------------- | --------------------------- |
| NDArrayConstView | Const multidimensional view |

---

## at()

```cpp
auto at(std::initializer_list<size_type> indexes) const
```

Provides multidimensional indexed access.

### Parameters

| Parameter | Description                      |
| --------- | -------------------------------- |
| indexes   | List of multidimensional indexes |

### Purpose

Allows explicit multidimensional coordinate-based access.

### Example

```cpp
matrix.at({1, 2});
```

---

# Shape & Dimensions

## count()

```cpp
size_type count() const noexcept
```

Returns size of the first dimension.

### Returns

| Type      | Description          |
| --------- | -------------------- |
| size_type | First dimension size |

---

## total_count()

```cpp
size_type total_count() const noexcept
```

Returns total element count.

### Returns

| Type      | Description              |
| --------- | ------------------------ |
| size_type | Total number of elements |

---

## dim()

```cpp
size_type dim() const noexcept
```

Returns dimensionality of the container.

### Returns

| Type      | Description          |
| --------- | -------------------- |
| size_type | Number of dimensions |

---

## getDimensions()

```cpp
const std::array<size_type, N>& getDimensions() const noexcept
```

Returns dimension metadata.

### Returns

| Type                     | Description     |
| ------------------------ | --------------- |
| std::array<size_type, N> | Dimension array |

---

# Comparison

## is_equal()

```cpp
bool is_equal(const NDArray& other) const
```

Compares two NDArrays element-by-element.

### Parameters

| Parameter | Description          |
| --------- | -------------------- |
| other     | Container to compare |

### Returns

| Type | Description     |
| ---- | --------------- |
| bool | Equality result |

> [!NOTE]
> Available only for equality-comparable element types.

---

# Reshaping

## reshape()

```cpp
template<typename SizeList>
view reshape(const SizeList& dimsContainer)
```

Creates reshaped multidimensional view.

### Parameters

| Parameter     | Description          |
| ------------- | -------------------- |
| dimsContainer | New dimension layout |

### Returns

| Type | Description                    |
| ---- | ------------------------------ |
| view | Reshaped multidimensional view |

### Purpose

Allows reinterpretation of multidimensional layout without reallocating storage.

> [!TIP]
> Reshape preserves contiguous storage and does not perform element copying.

---

# Iterators

## begin()

```cpp
iterator begin() noexcept
```

Returns iterator to first element.

---

## end()

```cpp
iterator end() noexcept
```

Returns iterator past the last element.

---

## begin() const

```cpp
const_iterator begin() const noexcept
```

Returns const iterator to first element.

---

## end() const

```cpp
const_iterator end() const noexcept
```

Returns const iterator past the last element.

---

## cbegin()

```cpp
const_iterator cbegin() const noexcept
```

Returns const iterator to first element.

---

## cend()

```cpp
const_iterator cend() const noexcept
```

Returns const iterator past the last element.

---

# Raw Storage Access

## data()

```cpp
pointer data() noexcept
```

Returns mutable pointer to contiguous storage.

### Returns

| Type    | Description         |
| ------- | ------------------- |
| pointer | Raw storage pointer |

---

## data() const

```cpp
const_pointer data() const noexcept
```

Returns const pointer to contiguous storage.

### Returns

| Type          | Description               |
| ------------- | ------------------------- |
| const_pointer | Const raw storage pointer |

> [!NOTE]
> NDArray guarantees contiguous memory layout.
