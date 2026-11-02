#include "../include/ringbuffer.h"

/********************************************************************************
 函数名: ringbuffer_init
 描述:   初始化
 输入参数:
    rb      - 环形缓存数据结构
    buffer  - 缓存存储空间
    unit_size    - 存储的数据结构的字节大小
    buffer_size  - 缓存的大小，可以存储多少个数据结构。
 输出参数:
 函数返回值:
 历史修改记录:
 <作者><日期><描述>
 ******************************************************************************/
short ringbuffer_init(ringbuffer_t *rb, void *buffer, short unit_size, short buffer_size)
{
    // 无效参数
    if (!rb || !buffer || (unit_size <= 0) || (buffer_size <= 0))
    {
        return -1;
    }

    rb->read = 0;
    rb->write = 0;
    rb->unit_size = unit_size;
    rb->buffer_size = buffer_size;
    rb->buffer = buffer;
    return 0;
}

/********************************************************************************
 函数名: ringbuffer_push
 描述:   添加数据到缓存。
 输入参数:
        rb      - 环形缓存数据结构
        unit    - 将要存储的数据结构。
 输出参数:
 函数返回值:
 历史修改记录:
 <作者><日期><描述>
 ******************************************************************************/
short ringbuffer_push(ringbuffer_t *rb, void *unit)
{
    short next;
    char *dst;

    // 无效参数
    if (!rb || !unit)//逻辑或，有一个成立就return
    {
        return -1;
    }

    next = (rb->write + 1) % rb->buffer_size;
    // 判断缓存是否满了(当写指针的下一个位置等于读指针的位置时，那么 ringbuffer 为满)， 如果满了则需要覆盖一个字节的数据
    if (next == rb->read)
    {
        rb->read = (rb->read + 1) % rb->buffer_size;//读索引加一就覆盖了
    }
    // 将数据复制到缓存
    dst = (char *)(rb->buffer) + rb->write * rb->unit_size;
    memcpy((void *)dst, unit, rb->unit_size);
    rb->write = next;
    return 0;
}

/********************************************************************************
 函数名: ringbuffer_prefetch
 描述:   读取数据，但不移动指针。
 输入参数:
        rb      - 环形缓存数据结构
 输出参数:
 函数返回值:
 历史修改记录:
 <作者><日期><描述>
 ******************************************************************************/
short ringbuffer_prefetch(ringbuffer_t *rb, void *result, short request_length)
{
    short used_length, i, idx;
    char *src, *dst;

    // 无效参数
    if (!rb || !result || (request_length <= 0))
    {
        return -1;
    }

    // 计算缓存中有效数据的长度， 从而决定实际读取的数据长度
    // con1: | ... read ...有效数据... write ... |  0,0(0); 2,3(1)
    // con2: | 有效数据2... write ... read ... 有效数据1 | 1,0(size-1);
    // 假设长度为5， read=3, write=1,  有效数据为： 5-3+1=3
    //
    // 实际读取的长度为: min(有效数据长度, 读取长度)
    used_length = (rb->write >= rb->read) ? (rb->write - rb->read) : (rb->buffer_size - rb->read + rb->write);
    if (used_length < request_length)
    {
        request_length = used_length;
    }

    // 提取缓存中的数据
    for (i = 0; i < request_length; i++)
    {
        idx = (rb->read + i) % rb->buffer_size;
        src = (char *)(rb->buffer) + rb->unit_size * idx;
        dst = (char *)result + rb->unit_size * i;
        memcpy((void *)dst, src, rb->unit_size);
    }
    return request_length;
}

/********************************************************************************
 函数名: ringbuffer_delete
 描述:   移动指针，删除缓存中的有效数据。
 输入参数:
        rb      - 环形缓存数据结构
        length  - 请求删除的数据长度
 输出参数:
 函数返回值:删除的字节长度
 历史修改记录:
 <作者><日期><描述>
 ******************************************************************************/
short ringbuffer_delete(ringbuffer_t *rb, short length)
{
    short used_length;

    // 删除的长度无效
    if (!rb || (length <= 0))
        return 0;

    // 实际删除的长度为: min(有效数据长度, 读取长度)
    used_length = (rb->write >= rb->read) ? (rb->write - rb->read) : (rb->buffer_size - rb->read + rb->write);
    if (used_length < length)
    {
        length = used_length;
    }

    // 删除数据
    rb->read = (rb->read + length) % rb->buffer_size;
    return length;
}

/********************************************************************************
 函数名: ringbuffer_pop
 描述:   读取数据，之后将已读取的数据从缓存中删除。
 输入参数:
        rb      - 环形缓存数据结构
 输出参数:
 函数返回值:删除的字节长度
 历史修改记录:
 <作者><日期><描述>
 ******************************************************************************/
short ringbuffer_pop(ringbuffer_t *rb, void *result, short request_length)
{
    short ret;

    // 读取数据
    ret = ringbuffer_prefetch(rb, result, request_length);
    // 移动指针，删除已读取的数据。
    if (ret >= 0)
    {
        ringbuffer_delete(rb, ret);
    }
    return ret;
}

/********************************************************************************
 函数名: ringbuffer_info_size
 描述:   获取环形缓存长度
 输入参数:
        rb      - 环形缓存数据结构
 输出参数:
 函数返回值:缓存空间长度
 历史修改记录:
 <作者><日期><描述>
 ******************************************************************************/
short ringbuffer_info_size(ringbuffer_t *rb)
{
    return rb->buffer_size;
}

/********************************************************************************
 函数名: ringbuffer_info_used
 描述:   获取环形缓存中已用字节长度
 输入参数:
        rb      - 环形缓存数据结构
 输出参数:
 函数返回值:缓存中有效数据长度
 历史修改记录:
 <作者><日期><描述>
 ******************************************************************************/
short ringbuffer_info_used(ringbuffer_t *rb)
{
    short used_length;
    used_length = (rb->write >= rb->read) ? (rb->write - rb->read) : (rb->buffer_size - rb->read + rb->write);
    return used_length;
}


