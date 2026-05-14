# Memory Management

NDArray uses explicit low-level object lifetime management instead of delegating construction semantics to higher-level container abstractions.

---

## Construction Model

The implementation allocates raw memory for the storage region and manually constructs objects using placement new.
Construction follows a deterministic sequential initialization model.

Simplified construction flow:

```cpp
for (const auto& elem : source) {
    new (&data_[constructed++]) value_type(elem);
}
```

This approach allows:

* explicit construction ordering
* precise lifetime control
* exception-safe rollback
* partial construction cleanup


## Why Manual Construction Is Used

| Reason                 | Description                       |
| ---------------------- | --------------------------------- |
| Lifetime Control       | Explicit construction/destruction |
| Exception Safety       | Partial rollback support          |
| Deterministic Behavior | Predictable initialization        |
| Low-Level Semantics    | Direct storage control            |

## Exception Safety

If object construction fails:

* already-constructed elements are destroyed
* allocated memory is released
* ownership remains valid

> [!NOTE]
> NDArray separates raw allocation from object construction similarly to STL allocator-oriented containers.

## Ownership Model

NDArray fully owns its memory.

The implementation guarantees:

* deterministic destruction
* contiguous storage
* explicit object lifetime
* predictable traversal semantics
