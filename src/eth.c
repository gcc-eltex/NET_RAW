#include "../header/raw.h"

u_char *eth_build(u_char *dmac, u_char *smac, u_char *data, int dlen)
{
	struct header_eth 	*hdr_eth;	// Указатель на заголовок ethernet
	u_char 				*frame;		// Сформированный фрейм
	int 				flength;	// Длина фрейма

	flength = dlen + sizeof(struct header_eth);
	// Выделяем память под фрейм, копируя данные
	frame = calloc(sizeof(u_char), flength);
	memcpy(frame + sizeof(struct header_eth), data, dlen);

	//Заполняем заголовок
	hdr_eth = (struct header_eth *)frame;
	memcpy(hdr_eth->smac, smac, 6);
	memcpy(hdr_eth->dmac, dmac, 6);
	hdr_eth->proto = 0x08;

	// Подчищаем старые данные, поскольку выделена новая память
	free(data);
	return frame;
}