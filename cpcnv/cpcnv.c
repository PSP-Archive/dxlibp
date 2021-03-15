/*
cpcnv.c
Character Code Converter for DXP.
written by yreeen.

This code include and based on libccc written by benhur.
http://www.psp-programming.com/benhur
*/

#include <pspkernel.h>
#include <string.h>
#include <malloc.h>
#include <libc/stdio.h>
#include "../cpcnv.h"
#include "liblzr.h"

DXPCPDATA dxpCpData = {0,0};

int dxpCpSetTable(void* ptr,u32 bytesize,u8 cp,u8 direction) {
	DXPCPTABLE *table;
	if (cp >= DXPCPTABLE_MAX || direction > 1)return -1;
	table = dxpCpData.table[cp] + direction;
	if (table->used && table->ptr ) 
		free(table->ptr);
	table->ptr = ptr;
	table->end = ptr + bytesize;
	table->used = 1;
	return 0;
}

int dxpCpReleaseTable(u8 cp,u8 direction)
{
	if(cp >= DXPCPTABLE_MAX || direction > 1)return -1;
	DXPCPTABLE *table = dxpCpData.table[cp] + direction;
	if(table->used && table->ptr)free(table->ptr);
	table->ptr = NULL;
	table->end = NULL;
	table->used = 0;
	return 0;
}

int dxpCpLoadTable(const char *filename,u8 cp,u8 direction) {
	if (cp >= DXPCPTABLE_MAX || direction > 1) return -1;

	SceUID fd = sceIoOpen(filename, PSP_O_RDONLY, 0777);
    if (fd < 0) return -1;
    unsigned int filesize = sceIoLseek(fd, 0, SEEK_END);
    sceIoLseek(fd, 0, SEEK_SET);
    void* table_data = (void*)malloc(filesize);
	if (!table_data) {
		sceIoClose(fd);
		return -1;
	}
    if (sceIoRead(fd, table_data, filesize) != filesize) {
		sceIoClose(fd);
		free(table_data);
		return -1;
	}
	sceIoClose(fd);

	/* decompress requested tables */
	unsigned int *header = (unsigned int*)table_data;
	if(direction)
	{
		while (header[0])
		{
			if ((cp == 0) || (cp == header[0]))
			{
				if(header[5] == 0)
				{
					free(table_data);
					return -1;
				}
				void* table = (void*)malloc(header[7]);
				if (!table) {
					free(table_data);
					return -1;
				}
				int ret = LZRDecompress(table, header[7], table_data+header[5], NULL);
				if (ret < 0) {
					free(table_data);
					free(table);
					return ret;
				}
				dxpCpSetTable(table, header[7], header[0],DXPCP_FROMUCS2);
			}
			header += 8;
		}
	}
	else
	{
		while (header[0]) {
			if ((cp == 0) || (cp == header[0]))
			{
				void* table = (void*)malloc(header[4]);
				if (!table)
				{
					free(table_data);
					return -1;
				}
				int ret = LZRDecompress(table, header[4], table_data+header[2], NULL);
				if (ret < 0)
				{
					free(table_data);
					free(table);
					return ret;
				}
				dxpCpSetTable(table, header[4], header[0],DXPCP_TOUCS2);
			}
			header += 8;
		}
	}
	free(table_data);
	return 0;
}

void dxpCpInit()
{
	if(dxpCpData.init)return;
	int i;
	for (i = 0;i < DXPCPTABLE_MAX;++i)
	{
		dxpCpData.table[i][0].ptr = 0;
		dxpCpData.table[i][0].end = 0;
		dxpCpData.table[i][0].used = 0;
		dxpCpData.table[i][1].ptr = 0;
		dxpCpData.table[i][1].end = 0;
		dxpCpData.table[i][1].used = 0;
	}
	dxpCpData.init = 1;
}

void dxpCpShutDown(void)
{
	if(!dxpCpData.init)return;
	int i;
	for(i = 0;i < DXPCPTABLE_MAX;++i){dxpCpReleaseTable(i,0);dxpCpReleaseTable(i,1);}
	dxpCpData.init = 0;
}

dxpUcs2 dxpCpSetErrorCharUCS2(dxpUcs2 code)
{
	code ^= dxpCpData.errcode_ucs2;
	dxpCpData.errcode_ucs2 ^= code;
	return code ^ dxpCpData.errcode_ucs2;
}

int dxpCpCode_toUcs2(dxpUcs2 *dst,u32 count,const dxpChar *src,u8 cp)
{
	if (!src || !dst || !count) return 0;

	switch (cp) {
		//multi-byte character sets
		case DXP_CP_SJIS:
			return dxpCpSJIS_toUcs2(dst, count, src);
		case DXP_CP_UTF8:
			return dxpCpUTF8_toUcs2(dst, count, src);
	} 	
	if (cp >= DXPCPTABLE_MAX)return -1;
	if (!dxpCpData.init)dxpCpInit();
	if (!(dxpCpData.table[cp][DXPCP_TOUCS2].used) && (cp > 0))dxpCpLoadTable( DXPCPTABLE_FILENAME, cp,DXPCP_TOUCS2);
	int length;
	for(length = 0;src[length] && length < count;++length)
	{
		if (src[length] < 0x80) 
			dst[length] = (dxpUcs2)src[length];
		else
		{
			dst[length] =  ((dxpCpData.table[cp][DXPCP_TOUCS2].used) ? ((dxpUcs2*)(dxpCpData.table[cp][DXPCP_TOUCS2].ptr))[src[length]-0x80] : dxpCpData.errcode_ucs2); 
			if (!dst[length]) dst[length] = dxpCpData.errcode_ucs2;
		}
	}
	return length;
}

int dxpCpCode_fromUcs2(dxpChar *dst,u32 count,const dxpUcs2 *src,u8 cp)
{
	if(!dst || !count || !src)return 0;
	switch(cp)
	{
		//multi-byte character sets
		case DXP_CP_SJIS:
			return dxpCpSJIS_fromUcs2(dst, count, src);
		case DXP_CP_UTF8:
			return dxpCpUTF8_fromUcs2(dst, count, src);
	}
	if (cp >= DXPCPTABLE_MAX)return -1;
	if (!dxpCpData.init)dxpCpInit();
	if (!(dxpCpData.table[cp][DXPCP_FROMUCS2].used) && (cp > 0))dxpCpLoadTable( DXPCPTABLE_FILENAME, cp,DXPCP_FROMUCS2);
	int length;
	for(length = 0;src[length] && length < count;++length)
	{
		if (src[length] < 0x80) 
			dst[length] = (dxpUcs2)src[length];
		else
		{
			dst[length] =  ((dxpCpData.table[cp][DXPCP_TOUCS2].used) ? ((dxpUcs2*)(dxpCpData.table[cp][DXPCP_TOUCS2].ptr))[src[length]-0x80] : dxpCpData.errcode_ucs2); 
			if (!dst[length]) dst[length] = dxpCpData.errcode_ucs2;
		}
	}
	return length;
}

int dxpCpCode_Strlen(const dxpChar *str,u8 cp)
{
	switch (cp) {
		//multi-byte character sets
		case DXP_CP_SJIS:
			return dxpCpSJIS_Strlen(str);
		case DXP_CP_UTF8:
			return dxpCpUTF8_Strlen(str);
		default:
			return strlen((const char*)str);
	} 	
}


//int cccStrlen(dxpCode const * str) {
//	if (!str) return 0;
//
//	return strlen((char*)str);
//}
//
//int cccStrlenSJIS(dxpCode const * str) {
//	if (!str) return 0;
//
//	int i = 0, length = 0;
//	while (str[i]) {
//		length++;
//		i += (str[i] <= 0x80 || (str[i] >= 0xA0 && str[i] <= 0xDF) || str[i] >= 0xFD) ? 1 : 2; //single or double byte
//	}
//	return length;
//}
//
//int cccStrlenGBK(dxpCode const * str) {
//	if (!str) return 0;
//
//	int i = 0, length = 0;
//	while (str[i]) {
//		length++;
//		i += (str[i] <= 0x80 || str[i] == 0xFF) ? 1 : 2; //single or double byte
//	}
//	return length;
//}
//
//int cccStrlenKOR(dxpCode const * str) {
//	return cccStrlenGBK(str);
//}
//
//int cccStrlenBIG5(dxpCode const * str) {
//	return cccStrlenGBK(str);
//}
//
//int cccStrlenUTF8(dxpCode const * str) {
//	if (!str) return 0;
//
//	int i = 0, length = 0;
//	while (str[i]) {
//		if      (str[i] <= 0x7F) { i++;    length++; } //ASCII
//		else if (str[i] <= 0xC1) { i++;              } //part of multi-byte or overlong encoding ->ignore
//		else if (str[i] <= 0xDF) { i += 2; length++; } //2-byte
//		else if (str[i] <= 0xEF) { i += 3; length++; } //3-byte
//		else                     { i++;              } //4-byte, restricted or invalid range ->ignore
//	}
//	return length;
//}
//
//int dxpCpCode_Strlen(dxpCode const * str, unsigned char cp) {
//	if (!str) return 0;
//	
//	int length = 0;
//    switch (cp) {
//		/* multi byte character sets */
//		case CCC_CP932:  length = cccStrlenSJIS(str); break;
//		case CCC_CP936:  length = cccStrlenGBK (str); break;
//		case CCC_CP949:  length = cccStrlenKOR (str); break;
//		case CCC_CP950:  length = cccStrlenBIG5(str); break;
//		case CCC_CPUTF8: length = cccStrlenUTF8(str); break;
//		/* single byte character sets */
//		default:        length = cccStrlen(str); break;
//	}
//	return length;
//}
//
//int cccStrlenUCS2(cccUCS2 const * str) {
//	if (!str) return 0;
//
//	int length = 0;
//	while (str[length]) length++;
//	return length;
//}
//
//
//
//int cccGBKtoUCS2(cccUCS2 * dst, size_t count, dxpCode const * str) {
//	if (!str || !dst) return 0;
//	if (!cccInitialized) cccInit();
//	if (!(__table_ptr__[CCC_CP936])) cccLoadTable("flash0:/vsh/etc/cptbl.dat", CCC_CP936);
//
//	unsigned char* entry;
//	unsigned short code;
//	int i = 0, length = 0;
//	while (str[i] && length < count) {
//		if (str[i] <= 0x7f) {
//			dst[length] = (cccUCS2)str[i]; 
//		} else if (str[i] <= 0x80) {
//			dst[length] = 0x20ac;
//        } else if (str[i] <= 0xfe) {
//			if (__table_ptr__[CCC_CP936]) { //table is present
//				code = 0x0100 * str[i] + str[i+1];			
//				for (entry = (unsigned char*)(__table_ptr__[CCC_CP936]); (entry < (unsigned char*)(__table_end__[CCC_CP936])) && ((entry[0]+0x100*entry[1] + entry[4]) <= code); entry += 5);			
//				if ((entry >= (unsigned char*)(__table_end__[CCC_CP936])) || (code < entry[0]+0x100*entry[1])) {
//					dst[length] = __error_char_ucs2__;
//				} else {
//					dst[length] = entry[2]+0x100*entry[3] + (code - entry[0]-0x100*entry[1]);
//				}
//			} else {
//				dst[length] = __error_char_ucs2__;
//			}
//		} else {
//			dst[length] = __error_char_ucs2__;
//		}
//        length++;
//        i += (str[i] <= 0x80 || str[i] == 0xFF) ? 1 : 2; //single or double byte
//	}
//	return length;
//}
//
//int cccKORtoUCS2(cccUCS2 * dst, size_t count, dxpCode const * str) {
//	if (!str || !dst) return 0;
//	if (!cccInitialized) cccInit();
//	if (!(__table_ptr__[CCC_CP949])) cccLoadTable("flash0:/vsh/etc/cptbl.dat", CCC_CP949);
//
//	unsigned char* entry;
//	unsigned short code;
//	int i = 0, length = 0;
//	while (str[i] && length < count) {
//		if (str[i] <= 0x7f) {
//			dst[length] = (cccUCS2)str[i]; 
//		} else if (str[i] <= 0x80) {
//			dst[length] = __error_char_ucs2__;
//		} else if (str[i] <= 0xfd) {
//			if (__table_ptr__[CCC_CP949]) { //table is present
//				code = 0x0100 * str[i] + str[i+1];			
//				for (entry = (unsigned char*)(__table_ptr__[CCC_CP949]); (entry < (unsigned char*)(__table_end__[CCC_CP949])) && ((entry[0]+0x100*entry[1] + entry[4]) <= code); entry += 5);			
//				if ((entry >= (unsigned char*)(__table_end__[CCC_CP949])) || (code < entry[0]+0x100*entry[1])) {
//					dst[length] = __error_char_ucs2__;
//				} else {
//					dst[length] = entry[2]+0x100*entry[3] + (code - entry[0]-0x100*entry[1]);
//				}
//			} else {
//				dst[length] = __error_char_ucs2__;
//			}
//		} else {
//			dst[length] = __error_char_ucs2__;
//		}
//        length++;
//        i += (str[i] <= 0x80 || str[i] == 0xFF) ? 1 : 2; //single or double byte
//	}
//	return length;
//}
//
//int cccBIG5toUCS2(cccUCS2 * dst, size_t count, dxpCode const * str) {
//	if (!str || !dst) return 0;
//	if (!cccInitialized) cccInit();
//	if (!(__table_ptr__[CCC_CP950])) cccLoadTable("flash0:/vsh/etc/cptbl.dat", CCC_CP950);
//
//	typedef struct {
//		unsigned short code;
//		cccUCS2 ucs2;
//	} cccTblEntry;
//	cccTblEntry* entry;
//	unsigned short code;
//	int i = 0, length = 0;
//	while (str[i] && length < count) {
//		if (str[i] <= 0x7f) {
//			dst[length] = (cccUCS2)str[i]; 
//        } else if (str[i] <= 0xa0) {
//			dst[length] = __error_char_ucs2__;
//		} else if (str[i] <= 0xf9) {
//			if (__table_ptr__[CCC_CP950]) { //table is present
//				code = 0x0100 * str[i] + str[i+1];			
//				for (entry = (cccTblEntry*)(__table_ptr__[CCC_CP950]); (entry < (cccTblEntry*)(__table_end__[CCC_CP950])) && (entry->code < code); entry++);
//				if ((entry >= (cccTblEntry*)(__table_end__[CCC_CP950])) || (code < entry->code)) {
//					dst[length] = __error_char_ucs2__;
//				} else {
//					dst[length] = entry->ucs2;
//				}		
//			} else {
//				dst[length] = __error_char_ucs2__;
//			}
//		} else {
//			dst[length] = __error_char_ucs2__;
//		}
//        length++;
//        i += (str[i] <= 0x80 || str[i] == 0xFF) ? 1 : 2; //single or double byte
//	}
//	return length;
//}
//
//
