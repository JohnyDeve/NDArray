# API Reference

---

# Container

```cpp
template<typename T, size_t N>
class NDArray;
```

## Constructors

| Constructor             | Description                               |
| ----------------------- | ----------------------------------------- |
| Default Constructor     | Creates empty container                   |
| Dimensional Constructor | Creates container with dimensions         |
| Initializer Constructor | Recursive multidimensional initialization |
| Copy Constructor        | Deep-copy construction                    |
| Move Constructor        | Ownership transfer                        |

## Assignment

| Method          | Description           |
| --------------- | --------------------- |
| Copy Assignment | Deep-copy replacement |
| Move Assignment | Ownership transfer    |

## Element Access

| Method     | Description                  |
| ---------- | ---------------------------- |
| operator[] | Recursive dimensional access |
| data()     | Returns pointer to storage   |

## Iteration

| Method  | Description                |
| ------- | -------------------------- |
| begin() | Iterator to first element  |
| end()   | Iterator past last element |

## Capacity

| Method          | Description           |
| --------------- | --------------------- |
| size()          | Logical size          |
| count()         | Dimension-local count |
| total_count()   | Total element count   |
| getDimensions() | Returns dimensions    |
