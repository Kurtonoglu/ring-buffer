/**
 * @file ring_buffer.h
 * @brief Fixed-size, statically allocated ring buffer (circular FIFO) for bytes.
 *
 * The caller owns the backing storage. The module performs no dynamic
 * memory allocation, which makes it suitable for resource-constrained
 * embedded targets.
 */

#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/** Status codes returned by the ring buffer API. */
typedef enum {
    RB_OK = 0,      /**< Operation completed successfully. */
    RB_ERR_NULL,    /**< A required pointer argument was NULL. */
    RB_ERR_PARAM,   /**< An argument was outside its valid range. */
    RB_ERR_FULL,    /**< The buffer is full. */
    RB_ERR_EMPTY    /**< The buffer is empty. */
} rb_status_t;

/**
 * @brief Ring buffer instance.
 *
 * The fields are exposed so the struct can be allocated by the caller,
 * but they shall be treated as private and accessed only through the API.
 */
typedef struct {
    uint8_t *storage;  /**< Caller-provided backing array. */
    size_t capacity;   /**< Number of bytes the buffer can hold. */
    size_t head;       /**< Index of the oldest byte (read position). */
    size_t count;      /**< Number of bytes currently stored. */
} ring_buffer_t;

/**
 * @brief Bind caller-provided storage to a ring buffer instance.
 * @param rb        Buffer handle.
 * @param storage   Backing array, at least @p capacity bytes long.
 * @param capacity  Buffer capacity in bytes; shall be greater than 0.
 * @return RB_OK, RB_ERR_NULL or RB_ERR_PARAM.
 * @req RB-REQ-001, RB-REQ-002, RB-REQ-003, RB-REQ-016
 */
rb_status_t rb_init(ring_buffer_t *rb, uint8_t *storage, size_t capacity);

/**
 * @brief Store one byte at the tail of the buffer.
 * @return RB_OK, RB_ERR_NULL or RB_ERR_FULL.
 * @req RB-REQ-004, RB-REQ-007, RB-REQ-011, RB-REQ-016
 */
rb_status_t rb_push(ring_buffer_t *rb, uint8_t value);

/**
 * @brief Remove the oldest byte and copy it to @p out.
 * @return RB_OK, RB_ERR_NULL or RB_ERR_EMPTY.
 * @req RB-REQ-005, RB-REQ-006, RB-REQ-008, RB-REQ-011, RB-REQ-016
 */
rb_status_t rb_pop(ring_buffer_t *rb, uint8_t *out);

/**
 * @brief Copy the oldest byte to @p out without removing it.
 * @return RB_OK, RB_ERR_NULL or RB_ERR_EMPTY.
 * @req RB-REQ-009, RB-REQ-010, RB-REQ-016
 */
rb_status_t rb_peek(const ring_buffer_t *rb, uint8_t *out);

/**
 * @brief Discard all stored bytes.
 * @return RB_OK or RB_ERR_NULL.
 * @req RB-REQ-015, RB-REQ-016
 */
rb_status_t rb_clear(ring_buffer_t *rb);

/**
 * @brief Number of bytes currently stored; 0 if @p rb is NULL.
 * @req RB-REQ-012
 */
size_t rb_count(const ring_buffer_t *rb);

/**
 * @brief True if the buffer holds no bytes, or if @p rb is NULL.
 * @req RB-REQ-013
 */
bool rb_is_empty(const ring_buffer_t *rb);

/**
 * @brief True if the buffer holds @c capacity bytes.
 * @req RB-REQ-014
 */
bool rb_is_full(const ring_buffer_t *rb);

#endif /* RING_BUFFER_H */