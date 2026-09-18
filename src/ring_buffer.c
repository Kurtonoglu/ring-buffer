/**
 * @file ring_buffer.c
 * @brief Implementation of the fixed-size ring buffer.
 */

#include "ring_buffer.h"

/**
 * @brief Compute the storage index of the next free slot (the tail).
 *
 * The modulo keeps the index inside the storage array, which is what
 * makes the buffer circular (RB-REQ-011).
 */
static size_t rb_tail_index(const ring_buffer_t *rb)
{
    return (rb->head + rb->count) % rb->capacity;
}

rb_status_t rb_init(ring_buffer_t *rb, uint8_t *storage, size_t capacity)
{
    if ((rb == NULL) || (storage == NULL)) {
        return RB_ERR_NULL;
    }
    if (capacity == 0U) {
        return RB_ERR_PARAM;
    }

    rb->storage  = storage;
    rb->capacity = capacity;
    rb->head     = 0U;
    rb->count    = 0U;

    return RB_OK;
}

rb_status_t rb_push(ring_buffer_t *rb, uint8_t value)
{
    if ((rb == NULL) || (rb->storage == NULL)) {
        return RB_ERR_NULL;
    }
    if (rb->count == rb->capacity) {
        return RB_ERR_FULL;
    }

    rb->storage[rb_tail_index(rb)] = value;
    rb->count++;

    return RB_OK;
}

rb_status_t rb_pop(ring_buffer_t *rb, uint8_t *out)
{
    if ((rb == NULL) || (rb->storage == NULL) || (out == NULL)) {
        return RB_ERR_NULL;
    }
    if (rb->count == 0U) {
        return RB_ERR_EMPTY;
    }

    *out = rb->storage[rb->head];
    rb->head = (rb->head + 1U) % rb->capacity;
    rb->count--;

    return RB_OK;
}

rb_status_t rb_peek(const ring_buffer_t *rb, uint8_t *out)
{
    if ((rb == NULL) || (rb->storage == NULL) || (out == NULL)) {
        return RB_ERR_NULL;
    }
    if (rb->count == 0U) {
        return RB_ERR_EMPTY;
    }

    *out = rb->storage[rb->head];

    return RB_OK;
}

rb_status_t rb_clear(ring_buffer_t *rb)
{
    if (rb == NULL) {
        return RB_ERR_NULL;
    }

    rb->head  = 0U;
    rb->count = 0U;

    return RB_OK;
}

size_t rb_count(const ring_buffer_t *rb)
{
    return (rb == NULL) ? 0U : rb->count;
}

bool rb_is_empty(const ring_buffer_t *rb)
{
    return (rb == NULL) ? true : (rb->count == 0U);
}

bool rb_is_full(const ring_buffer_t *rb)
{
    return (rb == NULL) ? false : (rb->count == rb->capacity);
}