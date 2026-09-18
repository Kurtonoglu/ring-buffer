# Ring Buffer — Software Requirements Specification

## 1. Scope

This document specifies the functional requirements for a fixed-size, statically
allocated ring buffer (circular FIFO) for 8-bit data, intended for use in
resource-constrained embedded systems.

## 2. Design constraints

| ID | Constraint |
|----|------------|
| RB-CON-001 | The module shall not perform any dynamic memory allocation. |
| RB-CON-002 | The caller shall provide the backing storage array and its capacity. |
| RB-CON-003 | The module shall be written in C99 and shall be free of compiler warnings when built with `-Wall -Wextra -Werror`. |

## 3. Functional requirements

| ID | Requirement |
|----|-------------|
| RB-REQ-001 | `rb_init` shall bind a caller-provided storage array and capacity to a ring buffer instance and shall return `RB_OK`. |
| RB-REQ-002 | After a successful `rb_init`, the buffer shall be empty and its element count shall be 0. |
| RB-REQ-003 | `rb_init` shall return `RB_ERR_PARAM` if the requested capacity is 0, and the buffer shall remain unusable. |
| RB-REQ-004 | `rb_push` shall store one byte at the tail of the buffer, increase the element count by 1, and return `RB_OK`. |
| RB-REQ-005 | `rb_pop` shall remove the oldest byte from the buffer, write it to the caller-provided output location, decrease the element count by 1, and return `RB_OK`. |
| RB-REQ-006 | Bytes shall be retrieved in first-in-first-out order. |
| RB-REQ-007 | When the buffer is full, `rb_push` shall return `RB_ERR_FULL`, and the stored data and element count shall remain unchanged. |
| RB-REQ-008 | When the buffer is empty, `rb_pop` shall return `RB_ERR_EMPTY` and shall not modify the caller-provided output location. |
| RB-REQ-009 | `rb_peek` shall write the oldest byte to the caller-provided output location without removing it and without changing the element count. |
| RB-REQ-010 | When the buffer is empty, `rb_peek` shall return `RB_ERR_EMPTY` and shall not modify the caller-provided output location. |
| RB-REQ-011 | The buffer shall continue to operate correctly when the internal write or read index wraps around the end of the storage array. |
| RB-REQ-012 | `rb_count` shall return the current number of stored bytes. |
| RB-REQ-013 | `rb_is_empty` shall report true if and only if the element count is 0. |
| RB-REQ-014 | `rb_is_full` shall report true if and only if the element count equals the configured capacity. |
| RB-REQ-015 | `rb_clear` shall discard all stored bytes, set the element count to 0, and return `RB_OK`. |
| RB-REQ-016 | Any API function called with a NULL buffer handle or a NULL required output pointer shall return `RB_ERR_NULL` and shall have no other effect. |

## 4. Status codes

| Code | Meaning |
|------|---------|
| `RB_OK` | Operation completed successfully. |
| `RB_ERR_NULL` | A required pointer argument was NULL. |
| `RB_ERR_PARAM` | An argument was outside its valid range. |
| `RB_ERR_FULL` | The operation failed because the buffer is full. |
| `RB_ERR_EMPTY` | The operation failed because the buffer is empty. |