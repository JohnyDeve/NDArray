# View & Iterator API

NDArray implements a dedicated multidimensional traversal system based on reusable iterator/view abstractions.

- `NDArrayView`
  
--    \
    
---    |>- `GeneralNDArrayView`

--    /

- `NDArrayConstView`

The library provides:

* mutable multidimensional views
* const multidimensional views
* iterator-compatible traversal semantics
* recursive multidimensional access

## Type Hierarchy

```cpp id="yckfca"
template<typename T, size_t N, bool IsConstClass>
class GeneralNDArrayView;
```

```cpp id="j9n7rt"
template<typename T, size_t N>
using NDArrayView = GeneralNDArrayView<T, N, false>;
```

```cpp id="l9epr3"
template<typename T, size_t N>
using NDArrayConstView = GeneralNDArrayView<T, N, true>;
```

> [!NOTE]
> `NDArrayView` and `NDArrayConstView` are lightweight non-owning traversal abstractions.

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

# Constructors

## Container Constructor

```cpp id="xblm3d"
template<typename Container>
GeneralNDArrayView(const Container& ndarray) noexcept
```

Constructs a view from a compatible multidimensional container.

### Parameters

| Parameter | Description                       |
| --------- | --------------------------------- |
| ndarray   | Source multidimensional container |

### Purpose

Allows:

* traversal without ownership transfer
* lightweight multidimensional access
* iterator-compatible interaction with NDArray

### Example

```cpp id="e7mohf"
NDArray<int, 2> matrix(3, 3);

NDArrayView<int, 2> view(matrix);
```

---

## Raw Pointer Constructor

```cpp id="jys0hf"
GeneralNDArrayView(
    pointer data_ptr,
    const std::array<size_type, N>& view_dimensions
) noexcept
```

Constructs a view from raw storage pointer and dimension metadata.

### Parameters

| Parameter       | Description                   |
| --------------- | ----------------------------- |
| data_ptr        | Pointer to contiguous storage |
| view_dimensions | Dimensions of the view        |

### Purpose

Used internally for:

* recursive traversal
* subview generation
* iterator arithmetic

> [!NOTE]
> This constructor does not allocate memory and does not own storage.

---

## Iterator Range Constructor

```cpp id="9v85q4"
template<typename Iterator>
GeneralNDArrayView(
    Iterator first,
    Iterator last,
    const std::array<size_t, N>& view_dimensions
) noexcept
```

Constructs a view from contiguous iterator range.

### Parameters

| Parameter       | Description               |
| --------------- | ------------------------- |
| first           | Iterator to first element |
| last            | Iterator to end sentinel  |
| view_dimensions | Dimensions of the view    |

### Purpose

Provides STL-style range traversal semantics.

### Requirements

| Requirement           | Description                                 |
| --------------------- | ------------------------------------------- |
| contiguous_iterator   | Iterator must provide contiguous traversal  |
| pointer compatibility | Iterator value type must match storage type |

---

## Default Constructor

```cpp id="d2kqmo"
GeneralNDArrayView() noexcept
```

Constructs an empty view.

### Behavior

* initializes null traversal state
* does not reference storage
* dimensions are initialized with zeros

### Purpose

Used for:

* deferred initialization
* iterator placeholders
* moved-from states

---

## Copy Constructor

```cpp id="f9s6pa"
GeneralNDArrayView(const GeneralNDArrayView& other) noexcept
```

Constructs a view by copying traversal state.

### Parameters

| Parameter | Description |
| --------- | ----------- |
| other     | Source view |

### Behavior

* copies pointer state
* copies dimension metadata
* does not duplicate storage

---

## Move Constructor

```cpp id="x0uq7w"
GeneralNDArrayView(GeneralNDArrayView&& other) noexcept
```

Transfers traversal state from another view.

### Parameters

| Parameter | Description |
| --------- | ----------- |
| other     | Source view |

---

# Assignment Operators

## Copy Assignment

```cpp id="l9j1fh"
GeneralNDArrayView& operator=(
    const GeneralNDArrayView& other
) noexcept
```

Copies traversal state from another view.

### Returns

| Type                | Description               |
| ------------------- | ------------------------- |
| GeneralNDArrayView& | Reference to current view |

---

## Move Assignment

```cpp id="1n4g40"
GeneralNDArrayView& operator=(
    GeneralNDArrayView&& other
) noexcept
```

Transfers traversal state from another view.

### Returns

| Type                | Description               |
| ------------------- | ------------------------- |
| GeneralNDArrayView& | Reference to current view |

---

# Element Access

## operator*() — 1D Dereference

```cpp id="t2nl0z"
reference operator*() const&
    requires(N == 1)
```

Returns reference to current element.

### Returns

| Type      | Description                  |
| --------- | ---------------------------- |
| reference | Reference to current element |

---

## operator*() — Multidimensional Dereference

```cpp id="c3r7a7"
GeneralNDArrayView<T, N - 1, IsConstClass>
operator*() const&
    requires(N > 1)
```

Returns lower-dimensional subview.

### Returns

| Type               | Description                        |
| ------------------ | ---------------------------------- |
| GeneralNDArrayView | Recursive multidimensional subview |

### Purpose

Enables recursive multidimensional traversal semantics.

---

## operator[] — 1D Access

```cpp id="vw9w8r"
reference operator[](size_type index) const&
    requires(N == 1)
```

Provides indexed access to elements.

### Parameters

| Parameter | Description   |
| --------- | ------------- |
| index     | Element index |

### Returns

| Type      | Description          |
| --------- | -------------------- |
| reference | Reference to element |

---

## operator[] — Multidimensional Access

```cpp id="08yyl0"
GeneralNDArrayView<T, N - 1, IsConstClass>
operator[](size_type index) const&
    requires(N > 1)
```

Returns lower-dimensional subview.

### Parameters

| Parameter | Description     |
| --------- | --------------- |
| index     | Dimension index |

### Returns

| Type               | Description                     |
| ------------------ | ------------------------------- |
| GeneralNDArrayView | Recursive multidimensional view |

### Example

```cpp id="w1l7kl"
NDArray<int, 2> matrix {
    {1, 2, 3},
    {4, 5, 6}
};

auto row = matrix[0];
```

---

# Iterator Operations

## Prefix Increment

```cpp id="j4h3f7"
GeneralNDArrayView& operator++() noexcept
```

Advances the view to the next logical multidimensional block.

### Returns

| Type                | Description                   |
| ------------------- | ----------------------------- |
| GeneralNDArrayView& | Reference to updated iterator |

---

## Postfix Increment

```cpp id="a8w57z"
GeneralNDArrayView operator++(int) noexcept
```

Returns previous iterator state before increment.

---

## Prefix Decrement

```cpp id="0a4i0g"
GeneralNDArrayView& operator--() noexcept
```

Moves iterator backward.

---

## Postfix Decrement

```cpp id="6o4ydj"
GeneralNDArrayView operator--(int) noexcept
```

Returns previous iterator state before decrement.

---

## operator+=

```cpp id="0fcz9u"
GeneralNDArrayView& operator+=(difference_type n) noexcept
```

Advances iterator by `n` multidimensional blocks.

### Parameters

| Parameter | Description |
| --------- | ----------- |
| n         | Offset      |

---

## operator-=

```cpp id="6l5e1k"
GeneralNDArrayView& operator-=(difference_type n) noexcept
```

Moves iterator backward by `n` multidimensional blocks.

---

## operator+

```cpp id="1axtba"
GeneralNDArrayView operator+(difference_type n) const noexcept
```

Returns shifted iterator.

---

## operator-

```cpp id="b5jcrw"
GeneralNDArrayView operator-(difference_type n) const noexcept
```

Returns backward-shifted iterator.

---

## Distance Operator

```cpp id="8xcrsx"
difference_type operator-(
    const GeneralNDArrayView& other
) const noexcept
```

Computes distance between two iterators.

### Returns

| Type            | Description                |
| --------------- | -------------------------- |
| difference_type | Distance between iterators |

---

# Comparison Operators

| Operator | Description                 |
| -------- | --------------------------- |
| `==`     | Equality comparison         |
| `!=`     | Inequality comparison       |
| `<`      | Less-than comparison        |
| `>`      | Greater-than comparison     |
| `<=`     | Less-or-equal comparison    |
| `>=`     | Greater-or-equal comparison |

> [!NOTE]
> Comparisons are based on underlying storage pointer positions.

---

# Pointer Access

## operator->()

```cpp id="nlbcl1"
pointer operator->() const noexcept
    requires(N == 1)
```

Provides direct pointer-style access.

### Returns

| Type    | Description                |
| ------- | -------------------------- |
| pointer | Pointer to current element |

---

# Dimensions & Size

## count()

```cpp id="m40q4g"
size_type count() const noexcept
```

Returns size of the first dimension.

---

## total_count()

```cpp id="pwz4qf"
size_type total_count() const noexcept
```

Returns total number of elements inside the view.

---

## getDimensions()

```cpp id="l2u6l5"
const std::array<size_type, N>&
getDimensions() const noexcept
```

Returns dimension metadata.

### Returns

| Type                     | Description      |
| ------------------------ | ---------------- |
| std::array<size_type, N> | Dimensions array |

---

# Recursive Coordinate Access

## at()

```cpp id="85jlwm"
template<size_type K = N, typename InitIterator>
auto at(InitIterator it) const noexcept
```

Provides recursive multidimensional coordinate-based access.

### Parameters

| Parameter | Description                     |
| --------- | ------------------------------- |
| it        | Iterator to coordinate sequence |

### Purpose

Allows recursive traversal using coordinate iterators.

### Example

```cpp id="m9p4a5"
std::array<size_t, 2> indexes {1, 2};

view.at(indexes.begin());
```

> [!TIP]
> `at()` recursively descends through dimensions while preserving contiguous traversal semantics.
