#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <pthread.h>
//ʹ�ú궨�彫ĳ��������ӵ�������
#define LL_ADD(item, list) \
    do                     \
    {                      \
        item->prev = NULL; \
        item->next = list; \
        list = item;       \
    } while (0)

//ʹ�ú궨�彫ĳ�������Ӷ�����ȥ��
#define LL_REMOVE(item, list)              \
    do                                     \
    {                                      \
        if (item->prev != NULL)            \
            item->prev->next = item->next; \
        if (item->next != NULL)            \
            item->next->prev = item->prev; \
        if (list == item)                  \
            list = item->next;             \
        item->prev = item->next = NULL;    \
    } while (0)
