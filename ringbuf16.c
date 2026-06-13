

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SIZE   8u                   // Must be a power of 2  
#define BUFFER_MASK   (BUFFER_SIZE - 1u)   //Used for fast wrap    

#define RB_OK         0
#define RB_ERR_FULL  -1
#define RB_ERR_EMPTY -2


 // Ring Buffer Structure -- stores uint16_t (16-bit) words

typedef struct {
    uint16_t data[BUFFER_SIZE]; /* Storage array (16-bit words)    */
    uint8_t  head;              /* Next write position             */
    uint8_t  tail;              /* Next read position              */
    uint8_t  count;             /* Number of words currently stored*/
} RingBuffer16;

 // ingbuf_init -- Initialise buffer to empty state
void ringbuf_init(RingBuffer16 *rb)
{
    rb->head  = 0u;
    rb->tail  = 0u;
    rb->count = 0u;
    memset(rb->data, 0, sizeof(rb->data));
}

//  ringbuf_is_full -- Returns 1 if buffer is full, else 0

uint8_t ringbuf_is_full(const RingBuffer16 *rb)
{
    return (rb->count == BUFFER_SIZE) ? 1u : 0u;
}


  // ringbuf_is_empty   Returns 1 if buffer is empty, else 0
 
uint8_t ringbuf_is_empty(const RingBuffer16 *rb)
{
    return (rb->count == 0u) ? 1u : 0u;
}

// ringbuf_count -- Returns number of words currently in buffer

uint8_t ringbuf_count(const RingBuffer16 *rb)
{
    return rb->count;
}


int ringbuf_write(RingBuffer16 *rb, uint16_t word)
{
    if (ringbuf_is_full(rb)) {
        return RB_ERR_FULL;
    }

    rb->data[rb->head] = word;
   
    rb->head = (rb->head + 1u) & BUFFER_MASK;
    rb->count++;

    return RB_OK;
}

int ringbuf_read(RingBuffer16 *rb, uint16_t *out_word)
{
    if (ringbuf_is_empty(rb)) {
        return RB_ERR_EMPTY;
    }

    *out_word = rb->data[rb->tail];
   
    rb->tail = (rb->tail + 1u) & BUFFER_MASK;
    rb->count--;

    return RB_OK;
}


int main(void)
{
    RingBuffer16 rb;
    ringbuf_init(&rb);

    uint16_t word;
    int      result;

    printf("=== Ring Buffer Demo (uint16_t) ===\n\n");

    // Step 1: Write 8 uint16_t words
    uint16_t write_data[] = {0x0041, 0x0042, 0x0043, 0x0044,
                              0x0045, 0x0046, 0x0047, 0x0048};

    for (uint8_t i = 0u; i < 8u; i++) {
        result = ringbuf_write(&rb, write_data[i]);
        if (result == RB_OK) {
            if (ringbuf_is_full(&rb)) {
                printf("[WRITE] 0x%04X -> OK (count=%u) FULL\n",
                       write_data[i], ringbuf_count(&rb));
            } else {
                printf("[WRITE] 0x%04X -> OK (count=%u)\n",
                       write_data[i], ringbuf_count(&rb));
            }
        }
    }

    // Step 2: Attempt to write one more word (0x9999) -- must fail

    result = ringbuf_write(&rb, 0x9999u);
    if (result == RB_ERR_FULL) {
        printf("[WRITE] 0x9999 -> FAIL (buffer full)\n");
    }

    printf("\n");

    // Step 3: Read 3 words (expect 0x0041, 0x0042, 0x0043)

    for (uint8_t i = 0u; i < 3u; i++) {
        result = ringbuf_read(&rb, &word);
        if (result == RB_OK) {
            printf("[READ]  -> 0x%04X (count=%u)\n",
                   word, ringbuf_count(&rb));
        }
    }

    printf("\n");

    
     // Step 4: Write 3 new words (0x0049, 0x004A, 0x004B)
   
    uint16_t new_data[] = {0x0049, 0x004A, 0x004B};

    for (uint8_t i = 0u; i < 3u; i++) {
        result = ringbuf_write(&rb, new_data[i]);
        if (result == RB_OK) {
            if (ringbuf_is_full(&rb)) {
                printf("[WRITE] 0x%04X -> OK (count=%u) FULL\n",
                       new_data[i], ringbuf_count(&rb));
            } else {
                printf("[WRITE] 0x%04X -> OK (count=%u)\n",
                       new_data[i], ringbuf_count(&rb));
            }
        }
    }

    printf("\n");

 
     // Step 5: Read all remaining 8 words
    
    while (!ringbuf_is_empty(&rb)) {
        result = ringbuf_read(&rb, &word);
        if (result == RB_OK) {
            if (ringbuf_is_empty(&rb)) {
                printf("[READ]  -> 0x%04X (count=%u) EMPTY\n",
                       word, ringbuf_count(&rb));
            } else {
                printf("[READ]  -> 0x%04X (count=%u)\n",
                       word, ringbuf_count(&rb));
            }
        }
    }

    printf("\n");

    // Step 6: Attempt to read from empty buffer -- must fail
    
    result = ringbuf_read(&rb, &word);
    if (result == RB_ERR_EMPTY) {
        printf("[READ] (empty) -> FAIL (buffer empty)\n");
    }

    printf("\n=== Demo Complete ===\n");
    return 0;
}