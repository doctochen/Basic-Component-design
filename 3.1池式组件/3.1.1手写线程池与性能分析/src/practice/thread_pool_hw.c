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