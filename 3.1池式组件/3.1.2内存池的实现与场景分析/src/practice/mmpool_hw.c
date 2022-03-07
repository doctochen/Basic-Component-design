/**
 * @File Name: mmpool_hw.c
 * @Brief : ��д�ڴ�ؼ�ע��
 * @Author : hewei (hewei_1996@qq.com)
 * @Version : 1.0
 * @Creat Date : 2022-03-05
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>

// �涨��4096�ֽ�Ϊ����
// ����4096��mp_large_s
// С��4096��mp_node_s

// ����
#define MP_ALIGNMENT 32
// ҳ��С
#define MP_PAGE_SIZE 4096
// һ�������������ڴ�
#define MP_MAX_ALLOC_FROM_POOL (MP_PAGE_SIZE - 1)

// �ڴ����
#define mp_align(n, alignment) (((n) + (alignment - 1)) & ~(alignment - 1))
#define mp_align_ptr(p, alignment) (void *)((((size_t)p) + (alignment - 1)) & ~(alignment - 1))

// ���ṹ��
struct mp_large_s
{
    struct mp_large_s *next;
    void *alloc;
};

// С��ṹ��
struct mp_node_s
{
    unsigned char *last;
    unsigned char *end;

    struct mp_node_s *next;
    size_t failed;
};

// �ڴ�ؽṹ��
struct mp_pool_s
{
    size_t max;

    struct mp_node_s *current;
    struct mp_large_s *large;

    struct mp_node_s head[0];
};

struct mp_pool_s *mp_create_pool(size_t size);
void mp_destory_pool(struct mp_pool_s *pool);
void *mp_alloc(struct mp_pool_s *pool, size_t size);
void *mp_nalloc(struct mp_pool_s *pool, size_t size);
void *mp_calloc(struct mp_pool_s *pool, size_t size);
void mp_free(struct mp_pool_s *pool, void *p);

/*
��̬�����ڴ����

int posix_memalign (void **memptr,
                    size_t alignment,
                    size_t size);

����posix_memalign( )�ɹ�ʱ�᷵��size�ֽڵĶ�̬�ڴ棬��������ڴ�ĵ�ַ��alignment�ı���������alignment������2���ݣ�
����voidָ��Ĵ�С�ı��������ص��ڴ��ĵ�ַ������memptr���棬��������ֵ��0.

����ʧ��ʱ��û���ڴ�ᱻ���䣬memptr��ֵû�б����壬�������´�����֮һ��

EINVAL
��������2���ݣ����߲���voidָ��ı�����

ENOMEM
û���㹻���ڴ�ȥ���㺯��������

Ҫע����ǣ��������������errno���ᱻ���ã�ֻ��ͨ������ֵ�õ���

��posix_memalign( )��õ��ڴ�ͨ��free( )�ͷš�
*/

struct mp_pool_s *mp_create_pool(size_t size)
{
    struct mp_pool_s *p;
    // ��̬�ڴ����
    int ret = posix_memalign((void **)&p, MP_ALIGNMENT, size + sizeof(struct mp_pool_s) + sizeof(struct mp_node_s));
    if (ret)
    {
        return NULL;
    }
    // Ĭ�ϴ���һ��С��
    p->max = (size < MP_MAX_ALLOC_FROM_POOL) ? size : MP_MAX_ALLOC_FROM_POOL;
    p->current = p->head;
    p->large = NULL;
    // ����ʱָ������ռ��ʼ�ĵط�
    p->head->last = (unsigned char *)p + sizeof(struct mp_pool_s) + sizeof(struct mp_node_s);
    p->head->end = p->head->last + size;

    p->head->failed = 0;

    return p;
}
// �����ڴ��
void mp_destory_pool(struct mp_pool_s *pool)
{
    struct mp_node_s *h, *n;
    struct mp_large_s *l;
    // ���ͷŴ��
    for (l = pool->large; l; l = l->next)
    {
        if (l->alloc)
        {
            free(l->alloc);
        }
    }
    // ���ͷ�С��
    h = pool->head->next;

    while (h)
    {
        n = h->next;
        free(h);
        h = n;
    }
    // �ͷ��ڴ��
    free(pool);
}
// ���ò��ͷ��ڴ��
void mp_reset_pool(struct mp_pool_s *pool)
{
    struct mp_node_s *h;
    struct mp_large_s *l;
    // ���ͷŴ��
    for (l = pool->large; l; l = l->next)
    {
        if (l->alloc)
        {
            free(l->alloc);
        }
    }

    pool->large = NULL;
    // ���ͷ�С��
    for (h = pool->head; h; h = h->next)
    {
        h->last = (unsigned char *)h + sizeof(struct mp_node_s);
    }
}
// ����һ��С��
static void *mp_alloc_block(struct mp_pool_s *pool, size_t size)
{
    unsigned char *m;
    struct mp_node_s *h = pool->head;
    struct mp_node_s *p, *new_node, *current;
    // ������һ���ڵ��ʣ��ռ�
    size_t psize = (size_t)(h->end - (unsigned char *)h);
    // �����ڴ����
    int ret = posix_memalign((void **)&m, MP_ALIGNMENT, psize);
    if (ret)
        return NULL;

    // �½ڵ�
    new_node = (struct mp_node_s *)m;
    new_node->end = m + psize;
    new_node->next = NULL;
    new_node->failed = 0;

    m += sizeof(struct mp_node_s);
    m = mp_align_ptr(m, MP_ALIGNMENT);
    // �˴�����Ŀ�Ľ�β
    new_node->last = m + size;

    current = pool->current;
    for (p = current; p->next; p = p->next)
    {
        if (p->failed++ > 4)
        {
            current = p->next;
        }
    }

    p->next = new_node;

    pool->current = current ? current : new_node;

    return m;
}

// ����һ�����
static void *mp_alloc_large(struct mp_pool_s *pool, size_t size)
{
    // ������ռ�ֱ��ʹ��malloc
    void *p = malloc(size);
    if (p == NULL)
        return NULL;
    // ���ҿ��еĿռ�
    size_t n = 0;
    struct mp_large_s *large;
    for (large = pool->large; large; large = large->next)
    {
        // ����ҵ���
        if (large->alloc == NULL)
        {
            large->alloc = p;
            return p;
        }
        // ��3���Ͳ�����
        if (n++ > 3)
            break;
    }
    // ���ҵ��Ľڵ�����ռ�
    large = mp_alloc(pool, sizeof(struct mp_large_s));
    if (large == NULL)
    {
        free(p);
        return NULL;
    }
    // ͷ�巨
    large->alloc = p;
    large->next = pool->large;
    pool->large = large;

    return p;
}

// �����飬�Զ����ڴ�����С
void *mp_memalign(struct mp_pool_s *pool, size_t size, size_t alignment)
{
    void *p;

    int ret = posix_memalign(&p, alignment, size);
    if (ret)
    {
        return NULL;
    }
    // ���ڵ�ṹ������ռ�
    struct mp_large_s *large = mp_alloc(pool, sizeof(struct mp_large_s));
    if (large == NULL)
    {
        free(p);
        return NULL;
    }
    // ͷ�巨
    large->alloc = p;
    large->next = pool->large;
    pool->large = large;

    return p;
}

// �ڴ�ط��䣨��ʼǰ�����ڴ���룩
void *mp_alloc(struct mp_pool_s *pool, size_t size)
{
    unsigned char *m;
    struct mp_node_s *p;

    // ���������С���ַ����黹��С��
    if (size <= pool->max)
    {
        p = pool->current;
        do
        {
            m = mp_align_ptr(p->last, MP_ALIGNMENT);
            // ����ⲿ��ʣ��ռ��㹻�󣬾Ͳ�����һ����
            if ((size_t)(p->end - m) >= size)
            {
                p->last = m + size;
                return m;
            }
            // ����һ���ڵ�
            p = p->next;
        } while (p);

        return mp_alloc_block(pool, size);
    }

    return mp_alloc_large(pool, size);
}

// �ڴ�ط��䣨��ʼǰ�������ڴ���룩
void *mp_nalloc(struct mp_pool_s *pool, size_t size)
{
    unsigned char *m;
    struct mp_node_s *p;

    // ���������С���ַ����黹��С��
    if (size <= pool->max)
    {
        p = pool->current;
        do
        {
            m = p->last;
            // ����ⲿ��ʣ��ռ��㹻�󣬾Ͳ�����һ����
            if ((size_t)(p->end - m) >= size)
            {
                p->last = m + size;
                return m;
            }
            // ����һ���ڵ�
            p = p->next;
        } while (p);

        return mp_alloc_block(pool, size);
    }

    return mp_alloc_large(pool, size);
}

// ����Ŀռ����
void *mp_calloc(struct mp_pool_s *pool, size_t size)
{
    void *p = mp_alloc(pool, size);
    if (p)
    {
        memset(p, 0, size);
    }

    return p;
}

// �ͷŴ�ҳ�ڴ�
void mp_free(struct mp_pool_s *pool, void *p)
{
    struct mp_large_s *l;
    for (l = pool->large; l; l = l->next)
    {
        if (p == l->alloc)
        {
            free(l->alloc);
            l->alloc = NULL;

            return;
        }
    }
}

int main(int argc, char *argv[])
{
    int size = 1 << 12;

    struct mp_pool_s *p = mp_create_pool(size);

    int i = 0;
    for (i = 0; i < 10; i++)
    {
        void *mp = mp_alloc(p, 512);
    }

    // printf("mp_align(123, 32): %d, mp_align(17, 32): %d\n", mp_align(123, 32), mp_align(17, 32));

    int j = 0;
    for (i = 0; i < 5; i++)
    {
        char *pp = mp_calloc(p, 32);
        for (j = 0; j < 32; j++)
        {
            if (pp[j])
            {
                printf("calloc wrong\n");
            }
            printf("calloc success\n");
        }
    }

    for (i = 0; i < 5; i++)
    {
        void *l = mp_alloc(p, 8192);
        mp_free(p, l);
    }

    mp_reset_pool(p);

    for (i = 0; i < 58; i++)
    {
        mp_alloc(p, 256);
    }

    mp_destory_pool(p);

    return 0;
}