#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <pcap.h>

// Структура, описывающая псевдозаголовок, имеет фиксированный размер
struct header_psd
{
    int sip;        // IP источника
    int dip;        // IP назначения
    char zeros;     // Нули
    char proto;     // Протокол
    short length;   // Длина (заголовок + данные)
};

/*
 * Структура, описывающая заколовок udp пакета, имеющего фиксированный размер 
 * заголовка
 */
struct header_udp
{
    short sport;    // Порт источника
    short dport;    // Порт назанчения
    short length;   // Длина пакета (без псевдозаголовка)
    short chsum;    // Контрольная сумма пакета (с псевдозаголовком)
};

/*
 * Модуль udp.c
 * Обеспечивает функционал для работы с протоколом udp
 */
u_short udp_checksum(char *hdr_udp, char *hdr_psd);
char *udp_build(char *sip, char *dip, short sport, short dport, char *data, 
                int dlen);