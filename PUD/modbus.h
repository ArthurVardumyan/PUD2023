/*
 * modbus.h
 *
 *  Created on: 6 февр. 2024 г.
 *      Author: 5095
 */

#ifndef INC_MODBUS_H_
#define INC_MODBUS_H_



#endif /* INC_MODBUS_H_ */

#include "type.h"

//Таблица адресов для обмена по Modbus
//#define MBmaxpars 100
#define MBmaxpars 200
//#define FLASHmaxpars 50
#define FLASHmaxpars 100
#define MBBUFLEN  500

void MBpack (BYTE ADR, BYTE FUNC, BYTE pack[], int inlen, BYTE buffer[], int *outlen);
void MBunpack (BYTE buffer [], int *inlen, BYTE *ADR, BYTE *FUNC, int *f_done);
void MBunpack2 (BYTE buffer2 [], int *inlen2, BYTE *ADR2, BYTE *FUNC2, int *f_done22);

WORD MBpackID (BYTE tpFRAME, BYTE idLIST, BYTE numFRAME, BYTE adrNODE);
void MBunpackID (WORD MBID, BYTE *tpFRAME, BYTE *idLIST, BYTE *numFRAME, BYTE *adrNODE);


void addwordtobuf (BYTE buf[], int *pos, WORD w);  //Добавляет слово в буфер
void adddwordtobuf (BYTE buf[], int *pos, DWORD dw);	//Добавляет двойное слово в буфер
void addfloattobuf (BYTE buf[], int *pos, float f); //Добавляет float в буфер

WORD getwordfrombuf (BYTE buf[], int *pos);  //Читаем слово из буфера
DWORD getdwordfrombuf (BYTE buf[], int *pos);	//Добавляет двойное слово в буфер
float getfloatfrombuf (BYTE buf[], int *pos); //Добавляет float в буфер
unsigned char getchar (BYTE B);

WORD getwordbyadr (WORD ADR);

void MBregfloat (float *adr, WORD adrw, int f_FLASH);
void MBreglong (DWORD *adr, WORD adrw, int f_FLASH);
void MBregword (WORD *adr, WORD adrw, int f_FLASH);
void MBregint (short int *adr, WORD adrw, int f_FLASH);

void MBwritedata (BYTE buffer [], BYTE data [],int *pos);
void MBreaddata (BYTE buffer [], BYTE data [],int *pos);
void MBresend (BYTE buffer [], BYTE data [],int *pos);

void FLASHregfloat (float *adr);//Регистрируем переменную FLOAT для записи в FLASH
void FLASHregword (WORD *adr);//Регистрируем переменную WORD для записи в FLASH
void FLASHsavetobuf (void);//Функция сохраняет зарегистрированные параметры в FLASH
int FLASHcheck (void);//Функция проверяет прочитанные параметры из FLASH
void FLASHrestore (void);//Восстанавливаем прочитанные переменные из буфера в параметры
void FLASHreglong (DWORD *adr);
void FLASHregint (short int *adr);






