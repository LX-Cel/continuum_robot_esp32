//
// Created by 86183 on 2024/10/9.
//

#ifndef DATA_REV_H
#define DATA_REV_H

#include <stdio.h>

#define RINGBUFF_LEN 10000  /* 定义环形缓冲区长度 */
#define usize getRingBuffLenght()
#define code_c() initRingBuff()
#define udelete(x) deleteRingBuff(x)
#define u(x) read1BFromRingBuff(x)

/* 定义环形缓冲区结构体 */
typedef struct {
    uint16_t Head;
    uint16_t Tail;
    uint16_t Lenght;
    uint8_t Ring_data[RINGBUFF_LEN];
} RingBuff_t;

/* 定义单电机数据分割结构体，count_1（位置）count_2（速度）count_3 */
typedef struct {
    uint16_t count_1;
    uint16_t count_2;
    uint16_t count_3;
} DataPos_t;

/* 定义多电机数据分割结构体，countMul_1（位置1）countMul_2（速度1）countMul_3（位置2）countMul_4（速度2）countMul_5（位置3）countMul_6（速度3）countMul_7（位置4）countMul_8（速度4）countMul_9 */
typedef struct {
    uint16_t countMul_1;
    uint16_t countMul_2;
    uint16_t countMul_3;
    uint16_t countMul_4;
    uint16_t countMul_5;
    uint16_t countMul_6;
    uint16_t countMul_7;
    uint16_t countMul_8;
    uint16_t countMul_9;
} DataPosMul_t;

/* 定义单电机数据结构体，result_1 = 位置，result_2 = 速度 */
typedef struct {
    int result_1;
    int result_2;
} GetResult_t;

/* 定义多电机数据结构体，resultMul_1 = 位置1，resultMul_2 = 速度1，resultMul_3 = 位置2，resultMul_4 = 速度2，resultMul_5 = 位置3，resultMul_6 = 速度3，resultMul_7 = 位置4，resultMul_8 = 速度4 */
typedef struct {
    int resultMul_1;
    int resultMul_2;
    int resultMul_3;
    int resultMul_4;
    int resultMul_5;
    int resultMul_6;
    int resultMul_7;
    int resultMul_8;
} GetResultMul_t;

extern RingBuff_t ringBuff;            /* 创建环形缓冲区 */
extern uint8_t RxBuff[500];            /* 创建接收数据数组 */

extern DataPos_t dataPos;              /* 创建单电机数据分割结构体 */
extern DataPosMul_t dataPosMul;        /* 创建多电机数据分割结构体 */

extern GetResult_t getResult;          /* 创建单电机数据结构体 */
extern GetResultMul_t getResultMul;    /* 创建多电机数据结构体 */

void initRingBuff(void);
void writeRingBuff(uint8_t data);
void deleteRingBuff(uint16_t size);
uint8_t read1BFromRingBuff(uint16_t position);
uint16_t getRingBuffLenght(void);
uint8_t isRingBuffOverflow(void);
void isDataTrue(void);
void initDataPos(void);
void initDataPosMul(void);
void initGetResult(void);
void initGetResultMul(void);
void dataDivision(void);
void dataDivisionMul(void);
void convertResult(void);
void convertResultMul(void);


#endif //DATA_REV_H
