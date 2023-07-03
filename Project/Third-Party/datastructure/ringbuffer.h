#ifndef RINGBUFFER_H__
#define RINGBUFFER_H__

#ifdef __cplusplus
extern "C" {
#endif

//#define RINGBUFFER_USING_HEAP
//#define RINGBUFFER_USING_LOCK

/* ring buffer */
typedef struct ringbuffer_s
{
    uint8_t *buffer_ptr;
    /* use the msb of the {read,write}_index as mirror bit. You can see this as
     * if the buffer adds a virtual mirror and the pointers point either to the
     * normal or to the mirrored buffer. If the write_index has the same value
     * with the read_index, but in a different mirror, the buffer is full.
     * While if the write_index and the read_index are the same and within the
     * same mirror, the buffer is empty. The ASCII art of the ringbuffer is:
     *
     *          mirror = 0                    mirror = 1
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     * | 0 | 1 | 2 | 3 | 4 | 5 | 6 ||| 0 | 1 | 2 | 3 | 4 | 5 | 6 | Full
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     *  read_idx-^                   write_idx-^
     *
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     * | 0 | 1 | 2 | 3 | 4 | 5 | 6 ||| 0 | 1 | 2 | 3 | 4 | 5 | 6 | Empty
     * +---+---+---+---+---+---+---+|+~~~+~~~+~~~+~~~+~~~+~~~+~~~+
     * read_idx-^ ^-write_idx
     *
     * The tradeoff is we could only use 32KiB of buffer for 16 bit of index.
     * But it should be enough for most of the cases.
     *
     * Ref: http://en.wikipedia.org/wiki/Circular_buffer#Mirroring */
    uint16_t read_mirror    : 1;
    uint16_t read_index     : 15;
    uint16_t write_mirror   : 1;
    uint16_t write_index    : 15;

    /* as we use msb of index as mirror bit, the size should be signed and
     * could only be positive. */
    int16_t buffer_size;
    
#ifdef RINGBUFFER_USING_LOCK
	(void *)lock(void);
	(void *)unlock(void);
#endif
}ringbuffer_t;

typedef enum
{
    RINGBUFFER_EMPTY,
    RINGBUFFER_FULL,
    /* half full is neither full nor empty */
    RINGBUFFER_HALFFULL,
}ringbuffer_state_t;

/**
 * RingBuffer for DeviceDriver
 *
 * Please note that the ring buffer implementation of RT-Thread
 * has no thread wait or resume feature.
 */
void ringbuffer_init(ringbuffer_t *rb, uint8_t *pool, int16_t size);
void ringbuffer_reset(ringbuffer_t *rb);
size_t ringbuffer_put(ringbuffer_t *rb, const uint8_t *ptr, uint16_t length);
size_t ringbuffer_put_force(ringbuffer_t *rb, const uint8_t *ptr, uint16_t length);
size_t ringbuffer_putchar(ringbuffer_t *rb, const uint8_t ch);
size_t ringbuffer_putchar_force(ringbuffer_t *rb, const uint8_t ch);
size_t ringbuffer_get(ringbuffer_t *rb, uint8_t *ptr, uint16_t length);
size_t ringbuffer_peak(ringbuffer_t *rb, uint8_t **ptr);
size_t ringbuffer_getchar(ringbuffer_t *rb, uint8_t *ch);
size_t ringbuffer_data_len(ringbuffer_t *rb);

#ifdef RINGBUFFER_USING_HEAP
ringbuffer_t* ringbuffer_create(uint16_t length);
void ringbuffer_destroy(ringbuffer_t *rb);
#endif

#ifdef __cplusplus
}
#endif

#endif
