#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <linux/if_packet.h>

/* 
 * Структура, описывающая псевдозаголовок необходимый для подсчета контрольной 
 * суммы UDP и TCP
 */
struct header_psd
{
    int     sip;    // IP источника
    int     dip;    // IP назначения
    u_char  zeros;  // Нули
    u_char  proto;  // Протокол
    short   length; // Длина (заголовок + данные)
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
    u_char  ver_len;    // 4 бита - версия, 4 - длина заголовка в октетах
    u_char  dscp_ecn;   // 6 бит - тип обслуживания , 2 - перегрузка
    short   length;     // Длина пакета в байтах (заголовок + данные)
    short   ident;      // Идентификатор фрагмента
    short   flag_offset;// 3 бита флаги, остальное - смещение фрагмента
    u_char  ttl;        // Время жизни пакета
    u_char  proto;      // Тип инкапсулированного протокола
    short   chsum;      // Контрольная сумма заголовка
    int     sip;        // IP адрес источника
    int     dip;        // IP адрес назначения
};

/*
 * Структура заголовка ethernet. В отличии от других заголовков имеет 
 * фиксированный размер, равный размеру структуры
 */
struct header_eth
{
    u_char  dmac[6];    // MAC адрес получателя    
    u_char  smac[6];    // MAC адрес отправителя
    short   proto;      // Тип инкапсулированного протокала
};

/*
 * Модуль udp.c
 * Обеспечивает функционал для работы с протоколом UDP
 */
u_short udp_checksum(u_char *hdr_udp, u_char *hdr_psd);
u_char *udp_build(u_char *sip, u_char *dip, short sport, short dport, 
                  u_char *data, int dlen);

/*
 * Модуль ipv4.c
 * Обеспечивает функционал для работы с протоколом IP версии 4
 */
u_short ipv4_checksum(u_char *hdr_ipv4);
u_char *ipv4_build(u_char *sip, u_char *dip, u_char *data, int dlen);

/*
 * Модуль eth.c
 * Обеспечивает функционал для работы с протоколом ethernet
 */
u_char *eth_build(u_char *dmac, u_char *smac, u_char *data, int dlen);