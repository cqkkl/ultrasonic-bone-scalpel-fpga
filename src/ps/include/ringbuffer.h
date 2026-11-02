#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__

/*******************************************************************************
 * File Includes
 ******************************************************************************/

/*******************************************************************************
 * Data Structure
 ******************************************************************************/
// 环形缓存的数据结构
typedef struct
{
    short read;        // 读取的位置索引
    short write;       // 写入的位置索引， 当前的无效的
    short unit_size;   // 数据结构的大小
    short buffer_size; // 缓存大小， 可以存储的数据结构数量
    void *buffer;      // 缓存
} ringbuffer_t;

/*******************************************************************************
 * Funciton Exports
 ******************************************************************************/
short ringbuffer_init(ringbuffer_t *rb, void *buffer, short unit_size, short buffer_size);
short ringbuffer_push(ringbuffer_t *rb, void *unit);
short ringbuffer_prefetch(ringbuffer_t *rb, void *result, short request_length);
short ringbuffer_pop(ringbuffer_t *rb, void *result, short request_length);
short ringbuffer_delete(ringbuffer_t *rb, short length);
short ringbuffer_info_size(ringbuffer_t *rb);
short ringbuffer_info_used(ringbuffer_t *rb);

#endif
