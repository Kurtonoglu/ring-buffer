# Ring Buffer — Requirements-Based Unit Testing in C

![CI](https://github.com/Kurtonoglu/ring-buffer/actions/workflows/ci.yml/badge.svg)

A fixed-size, statically allocated ring buffer (circular FIFO) written in C99,
developed using the verification workflow common in embedded and functional
safety projects: requirements first, then a test specification traced to those
requirements, then implementation, static analysis and coverage measurement.

The module itself is deliberately small. The point of the project is the
verification process around it.

## Verification results

| Metric | Result |
|--------|--------|
| Unit tests | 17 passing |
| Line coverage | 100% |
| Branch coverage | 100% |
| Function coverage | 100% |
| Static analysis | Cppcheck, no findings |
| Compiler warnings | None (`-Wall -Wextra -Werror -Wconversion -Wshadow`) |

## Design decisions

- **No dynamic memory.** The caller supplies the backing array, so the module
  can be used on targets where `malloc` is unavailable or prohibited.
- **Full buffer rejects new data** instead of overwriting the oldest byte, so
  data loss is always reported to the caller.
- **`head` and `count` are stored instead of `head` and `tail`.** With two
  indices, `head == tail` is ambiguous: it means both empty and full. Storing
  the element count removes that ambiguity; the tail is derived when needed.
- **Every public function returns a status code** and validates its pointer
  arguments, so misuse is detected rather than causing undefined behaviour.

## Repository layout

| Path | Contents |
|------|----------|
| `docs/requirements.md` | Software requirements specification (RB-REQ-xxx) |
| `docs/test-specification.md` | Test cases, design technique and traceability matrix |
| `include/ring_buffer.h` | Public API |
| `src/ring_buffer.c` | Implementation |
| `tests/test_ring_buffer.c` | Unity unit tests |
| `.github/workflows/ci.yml` | CI pipeline |

## Building and running

Requires GCC, GNU Make, Cppcheck and gcovr.

```bash
git clone --recurse-submodules https://github.com/Kurtonoglu/ring-buffer.git
cd ring-buffer

make test       # build and run the unit tests
make analyze    # run static analysis
make coverage   # run tests with coverage, write build/coverage.html
make clean      # remove build artefacts
```

## Test approach

Test cases are derived from the requirements using equivalence partitioning
and boundary value analysis. Each test is annotated with the requirement it
verifies, and `docs/test-specification.md` holds the full traceability matrix.
The partitions considered are: empty buffer, partially filled buffer, full
buffer, and invalid input. Wrap-around and a capacity-of-one buffer are tested
explicitly, as the index arithmetic is the most likely source of defects.

Branch coverage is measured rather than line coverage alone, because defensive
checks such as `if (rb == NULL)` can be fully line-covered while the failing
branch is never exercised.

## Possible extensions

- Interrupt-safe variant with a critical section around the index updates
- `rb_push_overwrite` for applications where the newest data matters most
- Port to an STM32 target with a UART receive interrupt feeding the buffer
