#include "../header/raw.h"

#define SRC_IP      "192.168.2.73"  // IP источника
#define DST_IP      "192.168.2.41"  // IP назначения
#define SRC_PORT    3512            // Порт источника
#define DST_PORT    1335            // Порт назначения
#define MSG         "Hello, World!" // Посылаемое сообщение

int main()
{
    int                 sock;   // RAW сокет
    struct sockaddr_ll  addr;   // Данные назначения для физического уровня
    u_char              *data;  // Данные для следующего уровня
    int                 psize;  // Размер данных
    
    // Создаем RAW сокет
    sock = socket(AF_PACKET, SOCK_RAW, IPPROTO_RAW);
    if (sock == -1){
        perror("ERROR create RAW socket");
        exit(-1);
    }

    // Обворачиваем заголовком транспортного уровня
    data = udp_build(SRC_IP, DST_IP, SRC_PORT, DST_PORT, MSG, strlen(MSG));
    psize = strlen(MSG) + sizeof(struct header_udp);

    // Обворачиваем заголовком сетевого уровня
    data = ipv4_build(SRC_IP, DST_IP, data, psize);
    psize += sizeof(struct header_ipv4);

    // Обворачиваем заголовком канального уровня
    u_char smac[] = {0x48, 0x5b, 0x39, 0x7a, 0xcf, 0x7f};   // MAC источника
    u_char dmac[] = {0x38, 0x63, 0xbb, 0xae, 0xcc, 0xbc};   // MAC назначения
    data = eth_build(dmac, smac, data, psize);
    psize += sizeof(struct header_eth);

    //Отправляем
    memset(&addr, 0, sizeof(addr));
    addr.sll_family = AF_PACKET;
    addr.sll_ifindex = 2;
    addr.sll_halen = 6;
    memcpy(addr.sll_addr, dmac, 6);

    //printf("INDEX: %d", if_nametoindex("enp3s0"));
    if (sendto(sock, data, psize, 0, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("sendto");
        close(sock);
        exit(-1);
    }

    return 0;
}