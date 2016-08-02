#include "../header/raw.h"

/*
 * Формирует UDP заголовок добавляя следом данные
 * 
 * @param sip   IP источника (для подсчета контрольной суммы)
 * @param dip   IP назначения (для подсчета контрольной суммы)
 * @param sport Порт источника
 * @param dport Порт назаначения
 * @param data  Передаваемые данные
 * @param dlen  Длина передаваемых данных
 *
 * @retval      Указатель на UDP заголовок и размещенные за ним данные
 */
u_char *udp_build(u_char *sip, u_char *dip, short sport, short dport, 
                  u_char *data, int dlen)
{
    struct header_psd   hdr_psd;    // Псевдозаголовок
    struct header_udp   *hdr_udp;   // Заголовок UDP
    u_char              *packet;    // Указатель на начало заголовка, за 
                                    // которым данные
    int                 pack_len;   // Длина пакета (UDP + Данные)
    
    // Формируем псевдозаголовок (Необходим для подсчета контрольной суммы)
    pack_len = sizeof(struct header_udp) + dlen;
    hdr_psd.sip = inet_addr(sip);
    hdr_psd.dip = inet_addr(dip);
    hdr_psd.zeros = 0;
    hdr_psd.proto = IPPROTO_UDP;
    hdr_psd.length = htons(pack_len);

    // Выделяем память под пакет и записываем передаваемые данные
    packet = calloc(sizeof(u_char), pack_len);
    memcpy(packet + sizeof(struct header_udp), data, dlen);

    // Заполняем заголовок
    hdr_udp = (struct header_udp *)packet;
    hdr_udp->sport = htons(sport);
    hdr_udp->dport = htons(dport);
    hdr_udp->length = htons(pack_len);
    hdr_udp->chsum = htons(udp_checksum((u_char *)hdr_udp, (u_char *)&hdr_psd)
                           );

    return packet;
}


/*
 * Выполняет подсчет контрольной суммы UDP. 
 * 
 * @param hdr_udp   Указатель на заголовок UDP пакет (заголовок + данные)
 * @param hdr_psd   Указатель на псевдозаголовок
 *
 * @retval          Контрольная сумма
 */
u_short udp_checksum(u_char *hdr_udp, u_char *hdr_psd)
{
    u_short fbyte;      // Первый байт 16ти битного блока (Для разворота)
    u_short sbyte;      // Второй байт 16ти битного блока (Для разворота)
    int     chsum;      // Контрольная сумма
    int     length;     // Длина пакета (заголовок + данные)

    // Считаем контрольную сумму псевдозаголовка, разворачивая байты
    chsum = 0;
    for (int i = 0; i < 12; i += 2){
        fbyte = hdr_psd[i];
        sbyte = hdr_psd[i + 1];
        chsum += (fbyte<<8)|sbyte;
    }

    /*
     * Считаем контрольную сумму самого пакета. Если количество байт нечетно,
     * то сразу правильно учтем последний байт и уменьшим length до четности
     */
    length = ntohs(((struct header_psd *)hdr_psd)->length);
    if (length%2 == 1){
        fbyte = hdr_udp[length - 1];
        sbyte = 0;
        chsum += (fbyte<<8)|sbyte;
        length--;
    }

    /*
     * Подсчет всего остального, при этом пропускаем в заголовке поле 
     * контрольной суммы
     */
    for (int i = 0; i < length; i += 2){
         if (i == 6)
            continue;
        fbyte = hdr_udp[i];
        sbyte = hdr_udp[i + 1];
        chsum += (fbyte<<8)|sbyte;
    }

    // Приводим к необходимому виду
    chsum = chsum + (chsum>>16);
    return (u_short)~chsum;
}