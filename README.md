## datastructs

Library with variety of datastructures.

Currently following datastructures are implemented:

**`avl tree`** **`hash table`** **`priority queue`** **`queue`**

### Building

---

| Option                     | Description     | Default |
| :------------------------- | :-------------- | :-----: |
| `MATH_EVAL_NOLOG`          | Disable logging |   OFF   |
| `MATH_EVAL_BUILD_EXAMPLES` | Build examples  |   OFF   |
| `MATH_EVAL_BUILD_TESTS`    | Build tests     |   OFF   |

#### Run tests

    cmake -DCMAKE_BUILD_TYPE=Release -S . -B build -G Ninja
    cmake --build build
    ctest --test-dir build/tests --verbose --output-on-failure
