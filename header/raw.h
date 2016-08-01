#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <pcap.h>

/* 
 * Структура, описывающая псевдозаголовок необходимый для подсчета контрольной 
 * суммы UDP и TCP
 */
struct header_psd
{
    int sip;        // IP источника
    int dip;        // IP назначения
    char zeros;     // Нули
    char proto;     // Протокол
    short length;   // Длина (заголовок + данные)
};

/*
 * Структура, описывающая заколовок UDP пакета, имеющего фиксированный размер 
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
 * Структура, описывающая заголовок IP пакета.
 * При отправке решено не добавлять опций, поэтому они отсутствуют в 
 * описанной структуре, благодаря чему имеем фиксированный размер заголовка
 */ 
struct header_ipv4
{
    char    ver_len;    // 4 бита - версия, 4 - длина заголовка в октетах
    char    dscp_ecn;   // 6 бит - тип обслуживания , 2 - перегрузка
    short   length;     // Длина пакета в байтах (заголовок + данные)
    short   ident;      // Идентификатор фрагмента
    short   flag_offset;// 3 бита флаги, остальное - смещение фрагмента
    char    ttl;        // Время жизни пакета
    char    proto;      // Тип инкапсулированного протокола
    short   chsum;      // Контрольная сумма заголовка
    int     sip;        // IP адрес источника
    int     dip;        // IP адрес назначения
};

/*
 * Модуль udp.c
 * Обеспечивает функционал для работы с протоколом UDP
 */
u_short udp_checksum(char *hdr_udp, char *hdr_psd);
char *udp_build(char *sip, char *dip, short sport, short dport, char *data, 
                int dlen);

/*
 * Модуль ipv4.c
 * Обеспечивает функционал для работы с протоколом IP версии 4
 */
u_short ipv4_checksum(char *hdr_ipv4);
char *ipv4_build(char *sip, char *dip, char *data, int dlen);