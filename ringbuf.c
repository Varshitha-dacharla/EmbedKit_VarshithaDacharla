/*
 * ringbuf.c -- Ring Buffer (Circular Buffer) Implementation
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SIZE   8u // Must be a power of 2
#define BUFFER_MASK   (BUFFER_SIZE - 1u)

#define RB_OK         0
#define RB_ERR_FULL  -1
#define RB_ERR_EMPTY -2

//  Ring Buffer Structure

typedef struct {
    uint8_t  data[BUFFER_SIZE];
    uint8_t  head;
    uint8_t  tail;
    uint8_t  count;
} RingBuffer;

// Initialize ring buffer 

void ringbuf_init(RingBuffer *rb)
{
    rb->head  = 0u;
    rb->tail  = 0u;
    rb->count = 0u;
    memset(rb->data, 0, sizeof(rb->data));
}

 // Check Whether buffer is full

uint8_t ringbuf_is_full(const RingBuffer *rb)
{
    return (rb->count == BUFFER_SIZE) ? 1u : 0u;
}

          // Check Whether buffer is full

uint8_t ringbuf_is_empty(const RingBuffer *rb)
{
    return (rb->count == 0u) ? 1u : 0u;
}

           // Return current number of stored bytes 

uint8_t ringbuf_count(const RingBuffer *rb)
{
    return rb->count;
}

        // Write one byte to the buffer

int ringbuf_write(RingBuffer *rb, uint8_t byte)
{
    if (ringbuf_is_full(rb)) {
        return RB_ERR_FULL;
    }
    rb->data[rb->head] = byte;

        // Fast Wrap - around for ^2 buffer size

    rb->head = (rb->head + 1u) & BUFFER_MASK;
    rb->count++;
    return RB_OK;
}

// Read one byte from the buffer

int ringbuf_read(RingBuffer *rb, uint8_t *out_byte)
{
    if (ringbuf_is_empty(rb)) {
        return RB_ERR_EMPTY;
    }
    *out_byte = rb->data[rb->tail];

    // Fast wrap-around for ^2 buffer size

    rb->tail = (rb->tail + 1u) & BUFFER_MASK;
    rb->count--;
    return RB_OK;
}

int main(void)
{
    RingBuffer rb;
    ringbuf_init(&rb);

    uint8_t byte;
    int     result;

    printf("=== Ring Buffer Demo ===\n\n");

    // Step 1: Write 8 bytes 

    uint8_t write_data[] = {0x41, 0x42, 0x43, 0x44,
                             0x45, 0x46, 0x47, 0x48};
    for (uint8_t i = 0u; i < 8u; i++) {
        result = ringbuf_write(&rb, write_data[i]);
        if (result == RB_OK) {
            if (ringbuf_is_full(&rb))
                printf("[WRITE] 0x%02X -> OK (count=%u) FULL\n", write_data[i], ringbuf_count(&rb));
            else
                printf("[WRITE] 0x%02X -> OK (count=%u)\n", write_data[i], ringbuf_count(&rb));
        }
    }

    // Step 2: Write 0x99 -- must fail 

    result = ringbuf_write(&rb, 0x99u);
    if (result == RB_ERR_FULL)
        printf("[WRITE] 0x99 -> FAIL (buffer full)\n");

    printf("\n");

    // Step 3: Read 3 bytes 

    for (uint8_t i = 0u; i < 3u; i++) {
        result = ringbuf_read(&rb, &byte);
        if (result == RB_OK)
            printf("[READ]  -> 0x%02X (count=%u)\n", byte, ringbuf_count(&rb));
    }

    printf("\n");

    // Step 4: Write 3 new bytes 
    uint8_t new_data[] = {0x49, 0x4A, 0x4B};
    for (uint8_t i = 0u; i < 3u; i++) {
        result = ringbuf_write(&rb, new_data[i]);
        if (result == RB_OK) {
            if (ringbuf_is_full(&rb))
                printf("[WRITE] 0x%02X -> OK (count=%u) FULL\n", new_data[i], ringbuf_count(&rb));
            else
                printf("[WRITE] 0x%02X -> OK (count=%u)\n", new_data[i], ringbuf_count(&rb));
        }
    }

    printf("\n");

    // Step 5: Read all 8 bytes 

    while (!ringbuf_is_empty(&rb)) {
        result = ringbuf_read(&rb, &byte);
        if (result == RB_OK) {
            if (ringbuf_is_empty(&rb))
                printf("[READ]  -> 0x%02X (count=%u) EMPTY\n", byte, ringbuf_count(&rb));
            else
                printf("[READ]  -> 0x%02X (count=%u)\n", byte, ringbuf_count(&rb));
        }
    }

    printf("\n");

    // Step 6: Read from empty -- must fail 
    result = ringbuf_read(&rb, &byte);
    if (result == RB_ERR_EMPTY)
        printf("[READ] (empty) -> FAIL (buffer empty)\n");

    printf("\n=== Demo Complete ===\n");
    return 0;
}