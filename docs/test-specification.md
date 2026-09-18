# Ring Buffer — Test Specification

## 1. Purpose

This document defines the unit test cases derived from the requirements in
`requirements.md` and records the traceability between them. Every functional
requirement is covered by at least one test case.

## 2. Test environment

| Item | Value |
|------|-------|
| Unit under test | `src/ring_buffer.c` |
| Test framework | Unity (ThrowTheSwitch) |
| Compiler | GCC, C99, `-Wall -Wextra -Werror` |
| Static analysis | Cppcheck |
| Coverage | gcov / gcovr (line and branch) |
| Test level | Unit test, host-based (PC), no target hardware required |

## 3. Test design technique

Test cases are derived from the requirements using equivalence partitioning
and boundary value analysis. For each operation the following partitions are
considered: empty buffer, partially filled buffer, full buffer, and invalid
(NULL or out-of-range) input. Wrap-around is tested explicitly because the
internal index arithmetic is the main source of implementation error.

## 4. Test cases

| Test case ID | Test function | Description | Expected result | Requirement |
|--------------|---------------|-------------|-----------------|-------------|
| TC-001 | `test_init_returns_ok_and_empty_buffer` | Initialise with valid storage and capacity | `RB_OK`, count 0, buffer reported empty | RB-REQ-001, RB-REQ-002 |
| TC-002 | `test_init_rejects_zero_capacity` | Initialise with capacity 0 | `RB_ERR_PARAM` | RB-REQ-003 |
| TC-003 | `test_init_rejects_null_arguments` | Initialise with NULL handle and with NULL storage | `RB_ERR_NULL` | RB-REQ-016 |
| TC-004 | `test_push_then_pop_returns_same_value` | Push one byte, pop it | `RB_OK`, popped value equals pushed value | RB-REQ-004, RB-REQ-005 |
| TC-005 | `test_pop_order_is_fifo` | Push three bytes, pop three bytes | Bytes are returned in the order they were pushed | RB-REQ-006 |
| TC-006 | `test_push_on_full_buffer_is_rejected` | Fill to capacity, then push once more | `RB_ERR_FULL`, count unchanged, stored data unchanged | RB-REQ-007 |
| TC-007 | `test_pop_on_empty_buffer_is_rejected` | Pop from an empty buffer | `RB_ERR_EMPTY`, output location unchanged | RB-REQ-008 |
| TC-008 | `test_peek_returns_oldest_without_removing` | Push two bytes, peek twice | Same byte returned both times, count unchanged | RB-REQ-009 |
| TC-009 | `test_peek_on_empty_buffer_is_rejected` | Peek into an empty buffer | `RB_ERR_EMPTY`, output location unchanged | RB-REQ-010 |
| TC-010 | `test_wrap_around_preserves_fifo_order` | Fill the buffer, pop two, push two, drain it | All bytes returned in FIFO order across the array boundary | RB-REQ-011 |
| TC-011 | `test_count_tracks_number_of_elements` | Query count after pushes and pops | Count matches the number of stored bytes | RB-REQ-012 |
| TC-012 | `test_is_empty_reflects_buffer_state` | Query on a new, a filled and a drained buffer | True only when no bytes are stored | RB-REQ-013 |
| TC-013 | `test_is_full_reflects_buffer_state` | Query below capacity and at capacity | True only at capacity | RB-REQ-014 |
| TC-014 | `test_clear_discards_all_elements` | Fill the buffer, then clear it | `RB_OK`, count 0, buffer reported empty | RB-REQ-015 |
| TC-015 | `test_api_rejects_null_handle_and_output` | Call push, pop, peek and clear with NULL arguments | `RB_ERR_NULL` in every case | RB-REQ-016 |
| TC-016 | `test_capacity_one_buffer_behaves_correctly` | Push, push again, pop on a one-byte buffer | `RB_OK`, `RB_ERR_FULL`, `RB_OK` — boundary case | RB-REQ-007, RB-REQ-011 |
| TC-017 | `test_api_rejects_handle_with_null_storage` | Call push, pop and peek on a handle whose storage pointer is NULL | `RB_ERR_NULL` in every case | RB-REQ-016 |

## 5. Requirement coverage matrix

| Requirement | Covered by |
|-------------|------------|
| RB-REQ-001 | TC-001 |
| RB-REQ-002 | TC-001 |
| RB-REQ-003 | TC-002 |
| RB-REQ-004 | TC-004, TC-005 |
| RB-REQ-005 | TC-004, TC-005 |
| RB-REQ-006 | TC-005, TC-010 |
| RB-REQ-007 | TC-006, TC-016 |
| RB-REQ-008 | TC-007 |
| RB-REQ-009 | TC-008 |
| RB-REQ-010 | TC-009 |
| RB-REQ-011 | TC-010, TC-016 |
| RB-REQ-012 | TC-011 |
| RB-REQ-013 | TC-012 |
| RB-REQ-014 | TC-013 |
| RB-REQ-015 | TC-014 |
| RB-REQ-016 | TC-003, TC-015, TC-017 |