/**
 * @file test_ring_buffer.c
 * @brief Unit tests for the ring buffer, derived from docs/test-specification.md.
 */

#include "unity.h"
#include "ring_buffer.h"

#define TEST_CAPACITY 4U

static ring_buffer_t rb;
static uint8_t storage[TEST_CAPACITY];

/** Runs before every test case: start from a known, initialised state. */
void setUp(void)
{
    (void)rb_init(&rb, storage, TEST_CAPACITY);
}

/** Runs after every test case. */
void tearDown(void)
{
}

/* TC-001 — RB-REQ-001, RB-REQ-002 */
void test_init_returns_ok_and_empty_buffer(void)
{
    ring_buffer_t local;
    uint8_t local_storage[TEST_CAPACITY];

    TEST_ASSERT_EQUAL(RB_OK, rb_init(&local, local_storage, TEST_CAPACITY));
    TEST_ASSERT_EQUAL_size_t(0U, rb_count(&local));
    TEST_ASSERT_TRUE(rb_is_empty(&local));
    TEST_ASSERT_FALSE(rb_is_full(&local));
}

/* TC-002 — RB-REQ-003 */
void test_init_rejects_zero_capacity(void)
{
    ring_buffer_t local;
    uint8_t local_storage[TEST_CAPACITY];

    TEST_ASSERT_EQUAL(RB_ERR_PARAM, rb_init(&local, local_storage, 0U));
}

/* TC-003 — RB-REQ-016 */
void test_init_rejects_null_arguments(void)
{
    ring_buffer_t local;
    uint8_t local_storage[TEST_CAPACITY];

    TEST_ASSERT_EQUAL(RB_ERR_NULL, rb_init(NULL, local_storage, TEST_CAPACITY));
    TEST_ASSERT_EQUAL(RB_ERR_NULL, rb_init(&local, NULL, TEST_CAPACITY));
}

/* TC-004 — RB-REQ-004, RB-REQ-005 */
void test_push_then_pop_returns_same_value(void)
{
    uint8_t value = 0U;

    TEST_ASSERT_EQUAL(RB_OK, rb_push(&rb, 0x5AU));
    TEST_ASSERT_EQUAL_size_t(1U, rb_count(&rb));

    TEST_ASSERT_EQUAL(RB_OK, rb_pop(&rb, &value));
    TEST_ASSERT_EQUAL_HEX8(0x5AU, value);
    TEST_ASSERT_EQUAL_size_t(0U, rb_count(&rb));
}

/* TC-005 — RB-REQ-006 */
void test_pop_order_is_fifo(void)
{
    uint8_t value = 0U;

    TEST_ASSERT_EQUAL(RB_OK, rb_push(&rb, 1U));
    TEST_ASSERT_EQUAL(RB_OK, rb_push(&rb, 2U));
    TEST_ASSERT_EQUAL(RB_OK, rb_push(&rb, 3U));

    TEST_ASSERT_EQUAL(RB_OK, rb_pop(&rb, &value));
    TEST_ASSERT_EQUAL_UINT8(1U, value);
    TEST_ASSERT_EQUAL(RB_OK, rb_pop(&rb, &value));
    TEST_ASSERT_EQUAL_UINT8(2U, value);
    TEST_ASSERT_EQUAL(RB_OK, rb_pop(&rb, &value));
    TEST_ASSERT_EQUAL_UINT8(3U, value);
}

/* TC-006 — RB-REQ-007 */
void test_push_on_full_buffer_is_rejected(void)
{
    uint8_t value = 0U;

    for (uint8_t i = 0U; i < TEST_CAPACITY; i++) {
        TEST_ASSERT_EQUAL(RB_OK, rb_push(&rb, i));
    }
    TEST_ASSERT_TRUE(rb_is_full(&rb));

    TEST_ASSERT_EQUAL(RB_ERR_FULL, rb_push(&rb, 0xFFU));
    TEST_ASSERT_EQUAL_size_t(TEST_CAPACITY, rb_count(&rb));

    /* The rejected push must not have overwritten the oldest byte. */
    TEST_ASSERT_EQUAL(RB_OK, rb_peek(&rb, &value));
    TEST_ASSERT_EQUAL_UINT8(0U, value);
}

/* TC-007 — RB-REQ-008 */
void test_pop_on_empty_buffer_is_rejected(void)
{
    uint8_t value = 0xA5U;

    TEST_ASSERT_EQUAL(RB_ERR_EMPTY, rb_pop(&rb, &value));
    TEST_ASSERT_EQUAL_HEX8(0xA5U, value);
}

/* TC-008 — RB-REQ-009 */
void test_peek_returns_oldest_without_removing(void)
{
    uint8_t value = 0U;

    TEST_ASSERT_EQUAL(RB_OK, rb_push(&rb, 7U));
    TEST_ASSERT_EQUAL(RB_OK, rb_push(&rb, 8U));

    TEST_ASSERT_EQUAL(RB_OK, rb_peek(&rb, &value));
    TEST_ASSERT_EQUAL_UINT8(7U, value);
    TEST_ASSERT_EQUAL(RB_OK, rb_peek(&rb, &value));
    TEST_ASSERT_EQUAL_UINT8(7U, value);
    TEST_ASSERT_EQUAL_size_t(2U, rb_count(&rb));
}

/* TC-009 — RB-REQ-010 */
void test_peek_on_empty_buffer_is_rejected(void)
{
    uint8_t value = 0xA5U;

    TEST_ASSERT_EQUAL(RB_ERR_EMPTY, rb_peek(&rb, &value));
    TEST_ASSERT_EQUAL_HEX8(0xA5U, value);
}

/* TC-010 — RB-REQ-011 */
void test_wrap_around_preserves_fifo_order(void)
{
    uint8_t value = 0U;

    /* Fill the buffer completely: 1 2 3 4 */
    for (uint8_t i = 1U; i <= TEST_CAPACITY; i++) {
        TEST_ASSERT_EQUAL(RB_OK, rb_push(&rb, i));
    }

    /* Free two slots at the start of the storage array. */
    TEST_ASSERT_EQUAL(RB_OK, rb_pop(&rb, &value));
    TEST_ASSERT_EQUAL_UINT8(1U, value);
    TEST_ASSERT_EQUAL(RB_OK, rb_pop(&rb, &value));
    TEST_ASSERT_EQUAL_UINT8(2U, value);

    /* These two writes wrap around to the beginning of the array. */
    TEST_ASSERT_EQUAL(RB_OK, rb_push(&rb, 5U));
    TEST_ASSERT_EQUAL(RB_OK, rb_push(&rb, 6U));

    /* Drain: order must still be 3 4 5 6. */
    for (uint8_t expected = 3U; expected <= 6U; expected++) {
        TEST_ASSERT_EQUAL(RB_OK, rb_pop(&rb, &value));
        TEST_ASSERT_EQUAL_UINT8(expected, value);
    }
    TEST_ASSERT_TRUE(rb_is_empty(&rb));
}

/* TC-011 — RB-REQ-012 */
void test_count_tracks_number_of_elements(void)
{
    uint8_t value = 0U;

    TEST_ASSERT_EQUAL_size_t(0U, rb_count(&rb));
    (void)rb_push(&rb, 1U);
    (void)rb_push(&rb, 2U);
    TEST_ASSERT_EQUAL_size_t(2U, rb_count(&rb));
    (void)rb_pop(&rb, &value);
    TEST_ASSERT_EQUAL_size_t(1U, rb_count(&rb));
    TEST_ASSERT_EQUAL_size_t(0U, rb_count(NULL));
}

/* TC-012 — RB-REQ-013 */
void test_is_empty_reflects_buffer_state(void)
{
    uint8_t value = 0U;

    TEST_ASSERT_TRUE(rb_is_empty(&rb));
    (void)rb_push(&rb, 1U);
    TEST_ASSERT_FALSE(rb_is_empty(&rb));
    (void)rb_pop(&rb, &value);
    TEST_ASSERT_TRUE(rb_is_empty(&rb));
    TEST_ASSERT_TRUE(rb_is_empty(NULL));
}

/* TC-013 — RB-REQ-014 */
void test_is_full_reflects_buffer_state(void)
{
    for (uint8_t i = 0U; i < (TEST_CAPACITY - 1U); i++) {
        (void)rb_push(&rb, i);
    }
    TEST_ASSERT_FALSE(rb_is_full(&rb));

    (void)rb_push(&rb, 0xFFU);
    TEST_ASSERT_TRUE(rb_is_full(&rb));
    TEST_ASSERT_FALSE(rb_is_full(NULL));
}

/* TC-014 — RB-REQ-015 */
void test_clear_discards_all_elements(void)
{
    for (uint8_t i = 0U; i < TEST_CAPACITY; i++) {
        (void)rb_push(&rb, i);
    }

    TEST_ASSERT_EQUAL(RB_OK, rb_clear(&rb));
    TEST_ASSERT_EQUAL_size_t(0U, rb_count(&rb));
    TEST_ASSERT_TRUE(rb_is_empty(&rb));
}

/* TC-015 — RB-REQ-016 */
void test_api_rejects_null_handle_and_output(void)
{
    uint8_t value = 0U;

    TEST_ASSERT_EQUAL(RB_ERR_NULL, rb_push(NULL, 1U));
    TEST_ASSERT_EQUAL(RB_ERR_NULL, rb_pop(NULL, &value));
    TEST_ASSERT_EQUAL(RB_ERR_NULL, rb_pop(&rb, NULL));
    TEST_ASSERT_EQUAL(RB_ERR_NULL, rb_peek(NULL, &value));
    TEST_ASSERT_EQUAL(RB_ERR_NULL, rb_peek(&rb, NULL));
    TEST_ASSERT_EQUAL(RB_ERR_NULL, rb_clear(NULL));
}

/* TC-016 — RB-REQ-007, RB-REQ-011 */
void test_capacity_one_buffer_behaves_correctly(void)
{
    ring_buffer_t single;
    uint8_t single_storage[1];
    uint8_t value = 0U;

    TEST_ASSERT_EQUAL(RB_OK, rb_init(&single, single_storage, 1U));

    TEST_ASSERT_EQUAL(RB_OK, rb_push(&single, 0x11U));
    TEST_ASSERT_EQUAL(RB_ERR_FULL, rb_push(&single, 0x22U));

    TEST_ASSERT_EQUAL(RB_OK, rb_pop(&single, &value));
    TEST_ASSERT_EQUAL_HEX8(0x11U, value);

    /* After wrapping, the single slot must be reusable. */
    TEST_ASSERT_EQUAL(RB_OK, rb_push(&single, 0x33U));
    TEST_ASSERT_EQUAL(RB_OK, rb_pop(&single, &value));
    TEST_ASSERT_EQUAL_HEX8(0x33U, value);
}

/* TC-017 — RB-REQ-016 (defensive check: handle not initialised) */
void test_api_rejects_handle_with_null_storage(void)
{
    ring_buffer_t uninitialised = { NULL, TEST_CAPACITY, 0U, 0U };
    uint8_t value = 0U;

    TEST_ASSERT_EQUAL(RB_ERR_NULL, rb_push(&uninitialised, 1U));
    TEST_ASSERT_EQUAL(RB_ERR_NULL, rb_pop(&uninitialised, &value));
    TEST_ASSERT_EQUAL(RB_ERR_NULL, rb_peek(&uninitialised, &value));
}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_init_returns_ok_and_empty_buffer);
    RUN_TEST(test_init_rejects_zero_capacity);
    RUN_TEST(test_init_rejects_null_arguments);
    RUN_TEST(test_push_then_pop_returns_same_value);
    RUN_TEST(test_pop_order_is_fifo);
    RUN_TEST(test_push_on_full_buffer_is_rejected);
    RUN_TEST(test_pop_on_empty_buffer_is_rejected);
    RUN_TEST(test_peek_returns_oldest_without_removing);
    RUN_TEST(test_peek_on_empty_buffer_is_rejected);
    RUN_TEST(test_wrap_around_preserves_fifo_order);
    RUN_TEST(test_count_tracks_number_of_elements);
    RUN_TEST(test_is_empty_reflects_buffer_state);
    RUN_TEST(test_is_full_reflects_buffer_state);
    RUN_TEST(test_clear_discards_all_elements);
    RUN_TEST(test_api_rejects_null_handle_and_output);
    RUN_TEST(test_capacity_one_buffer_behaves_correctly);
    RUN_TEST(test_api_rejects_handle_with_null_storage);

    return UNITY_END();
}