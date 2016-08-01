#include "../header/raw.h"

#define SRC_IP      "127.0.0.1"     // IP источника
#define DST_IP      "127.0.0.1"     // IP назначения
#define SRC_PORT    3512            // Порт источника
#define DST_PORT    1335            // Порт назначения
#define MSG         "Hello, World!"  // Посылаемое сообщение

int main()
{
    int                 sock;   // RAW сокет
    struct sockaddr_in  addr;   // Данные назначения
    char                *data;  // Данные для следующего уровня
    int                 psize;  // Размер данных

    // Создаем RAW сокет
    sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (sock == -1){
        perror("ERROR create RAW socket");
        exit(-1);
    }
    /*
     * Заполняем данные назначения, поскольку пока формируем только UDP, а IP 
     * заголовок за нас заполнит система.
     */
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(DST_IP);

    // Формируем пакет транспортного уровня
    data = udp_build(SRC_IP, DST_IP, SRC_PORT, DST_PORT, MSG, strlen(MSG));
    psize = strlen(MSG) + sizeof(struct header_udp);

    //Отправляем
    sendto(sock, data, psize, 0, (struct sockaddr *)&addr, sizeof(addr));

    return 0;
}