#include "../header/raw.h"

char *ipv4_build(char *sip, char *dip, char *data, int dlen)
{
    char                *packet;    // Указатель на начало пакета
    int                 pack_len;   // Размер пакета
    struct header_ipv4  *hdr_ip;    // Заголовок IP

    pack_len = sizeof(struct header_ipv4) + dlen;
    // Выделяем память под пакет и записываем передаваемые данные
    packet = calloc(sizeof(char), pack_len);
    memcpy(packet + sizeof(struct header_ipv4), data, dlen);

    // Заполняем заголовок
    hdr_ip = (struct header_ipv4 *)packet;
    hdr_ip->ver_len = 0x5;
    hdr_ip->ver_len |= 0x40;
    hdr_ip->length = htons(pack_len);
    hdr_ip->ttl = 56;
    hdr_ip->proto = IPPROTO_UDP;
    hdr_ip->sip = inet_addr(sip);
    hdr_ip->dip = inet_addr(dip);
    hdr_ip->chsum = htons(ipv4_checksum((char *)hdr_ip));

    // Подчищаем старые данные, поскольку выделена новая память
    free(data);
    return packet;
}


/*
 * Выполняет подсчет контрольной суммы заголовка IP. 
 * 
 * @param hdr_ipv4  Указатель на начало заголовка
 * @retval          Контрольная сумма
 */
u_short ipv4_checksum(char *hdr_ipv4)
{
    int     chsum;  // Контрольная сумма
    u_short fbyte;  // Первый байт 16ти битного блока
    u_short sbyte;  // Второй байт 16ти битного блока
    int     length; // Размер заголовка

    chsum = 0;
    length = sizeof(struct header_ipv4);
    /*
     * Считаем сумму пропуская поле контрольной суммы в заголовке. 
     * Суммирование производим 16ти битными блоками
     */
    for (int i = 0; i < length * 4; i+=2){
        if (i == 10)
            continue;
        fbyte = hdr_ipv4[i];
        sbyte = hdr_ipv4[i + 1];
        chsum += (fbyte<<8)|sbyte;
    }
    
    // Приводим к необходимому виду
    chsum = chsum + (chsum>>16);
    return (u_short)~chsum;
}