/**
 * @File Name: sync_dns_client_hw.c
 * @Brief : ͬ�������(Ŀǰֻ������dns����)
 * @Author : hewei (hewei_1996@qq.com)
 * @Version : 1.0
 * @Creat Date : 2022-03-11
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <sys/epoll.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <pthread.h>

#define DNS_SVR "114.114.114.114"

#define DNS_HOST 0x01
#define DNS_CNAME 0x05

// ��������Ӧ�ĸ�ʽ��һ�µģ�
// �������ΪHeader��Question��Answer��Authority��Additional 5���֣�

// Header������һ���еģ����ȹ̶�Ϊ12���ֽڣ�
// ����4���ֿ�����Ҳ����û�У����ҳ���Ҳ��һ���������Header��������ָ����
// 1.ID��ռ16λ����ֵ�ɷ���DNS����ĳ������ɣ�DNS����������Ӧʱ��ʹ�ø�ID��������������������ֲ�ͬ��DNS��Ӧ��
// QR��ռ1λ��ָʾ����Ϣ����������Ӧ��0��ʾ����1��ʾ��Ӧ��
// OPCODE��ռ4λ��ָʾ��������ͣ������������趨����Ӧ��Ϣ�и��ø�ֵ��0��ʾ��׼��ѯ��1��ʾ��ת��ѯ��2��ʾ������״̬��ѯ��3~15Ŀǰ�������Ա�����ʹ�á�
// AA��Authoritative Answer��Ȩ��Ӧ�𣩣�ռ1λ����ʾ��Ӧ�ķ������Ƿ���Ȩ��DNS��������ֻ����Ӧ��Ϣ����Ч��
// TC��TrunCation���ضϣ���ռ1λ��ָʾ��Ϣ�Ƿ���Ϊ�����С���ƶ����ضϡ�
// RD��Recursion Desired�������ݹ飩��ռ1λ����ֵ��������Ϣ�б����ã���Ӧ��Ϣ���ø�ֵ����������ã���ʾϣ���������ݹ��ѯ������������һ��֧�ֵݹ��ѯ��
// RA��Recursion Available���ݹ�����ԣ���ռ1λ����ֵ����Ӧ��Ϣ�б����û�������Ա����������Ƿ�֧�ֵݹ��ѯ��
// Z��ռ3λ���������á�
// RCODE��Response code����ռ4λ����ֵ����Ӧ��Ϣ�б����á�ȡֵ���������£�
// 0��No error condition��û�д���������
// 1��Format error�������ʽ���󣬷������޷���������
// 2��Server failure������������
// 3��Name Error��ֻ��Ȩ��DNS����������Ӧ�������壬��ʾ�����е����������ڡ�
// 4��Not Implemented����������֧�ָ��������͡�
// 5��Refused���������ܾ�ִ�����������
// 6~15���������á�
// QDCOUNT��ռ16λ���޷��ţ���ָ��Question���ֵİ�����ʵ��������
// ANCOUNT��ռ16λ���޷��ţ���ָ��Answer���ֵİ�����RR��Resource Record��������
// NSCOUNT��ռ16λ���޷��ţ���ָ��Authority���ֵİ�����RR��Resource Record��������
// ARCOUNT��ռ16λ���޷��ţ���ָ��Additional���ֵİ�����RR��Resource Record��������
struct dns_header
{
    unsigned short id;
    unsigned short flags;
    unsigned short qdcount;
    unsigned short ancount;
    unsigned short nscount;
    unsigned short arcount;
};

// QNAME���ֽ�����������0x00��Ϊ����������ʾ��ѯ����������ע�⣺������֪����������"."�ŷָ���˶�α�ǩ����QNAME�У�ÿ�α�ǩǰ���һ�����֣���ʾ��������ǩ�ĳ��ȡ����磺api.sina.com.cn��ʾ��QNAMEʱ������"api"ǰ�����һ���ֽ�0x03��"sina"ǰ�����һ���ֽ�0x04��"com"ǰ�����һ���ֽ�0x03����"cn"ǰ�����һ���ֽ�0x02��
// QTYPE��ռ2���ֽڡ���ʾRR���ͣ�������RR���ܣ�
// QCLASS��ռ2���ֽڡ���ʾRR���࣬������RR���ܡ�
struct dns_question
{
    int length;
    unsigned short qtype;
    unsigned short qclass;
    char *qname;
};

struct dns_item
{
    char *domain;
    char *ip;
};

int dns_create_header(struct dns_header *header)
{
    return 0;
}

int dns_create_question(struct dns_question *question, const char *hostname)
{
    return 0;
}

int dns_build_request(struct dns_header *header, struct dns_question *question, char *request)
{
    return 0;
}

static int is_pointer(int in)
{
}

static void dns_parse_name(unsigned char *chunk, unsigned char *ptr, char *out, int *len)
{
}

static int dns_parse_response(char *buffer, struct dns_item **domains)
{
    int cnt = 0;
    return cnt;
}

int dns_client_commit(const char *domain)
{
    int socketfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketfd < 0)
    {
        perror("create socket failed \n");
        exit(1);
    }

    printf("url: %s \n", domain);

    struct sockaddr_in dest;
    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(53);
    dest.sin_addr.s_addr = inet_addr(DNS_SVR);
}

char *domain[] = {
    //	"www.ntytcp.com",
    "bojing.wang",
    "www.baidu.com",
    "tieba.baidu.com",
    "news.baidu.com",
    "zhidao.baidu.com",
    "music.baidu.com",
    "image.baidu.com",
    "v.baidu.com",
    "map.baidu.com",
    "baijiahao.baidu.com",
    "xueshu.baidu.com",
    "cloud.baidu.com",
    "www.163.com",
    "open.163.com",
    "auto.163.com",
    "gov.163.com",
    "money.163.com",
    "sports.163.com",
    "tech.163.com",
    "edu.163.com",
    "www.taobao.com",
    "q.taobao.com",
    "sf.taobao.com",
    "yun.taobao.com",
    "baoxian.taobao.com",
    "www.tmall.com",
    "suning.tmall.com",
    "www.tencent.com",
    "www.qq.com",
    "www.aliyun.com",
    "www.ctrip.com",
    "hotels.ctrip.com",
    "hotels.ctrip.com",
    "vacations.ctrip.com",
    "flights.ctrip.com",
    "trains.ctrip.com",
    "bus.ctrip.com",
    "car.ctrip.com",
    "piao.ctrip.com",
    "tuan.ctrip.com",
    "you.ctrip.com",
    "g.ctrip.com",
    "lipin.ctrip.com",
    "ct.ctrip.com"};

int main(int argc, char *argv[])
{

    int count = sizeof(domain) / sizeof(domain[0]);
    int i = 0;

    for (i = 0; i < count; i++)
    {
        dns_client_commit(domain[i]);
    }

    getchar();
}
