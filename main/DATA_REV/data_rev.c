//
// Created by 86183 on 2024/10/9.
//

#include "data_rev.h"

RingBuff_t ringBuff;            /* 创建环形缓冲区 */
uint8_t RxBuff[500];            /* 创建接收数据数组 */

DataPos_t dataPos;              /* 创建单电机数据分割结构体 */
DataPosMul_t dataPosMul;        /* 创建多电机数据分割结构体 */

GetResult_t getResult;          /* 创建单电机数据结构体 */
GetResultMul_t getResultMul;    /* 创建多电机数据结构体 */

void initRingBuff(void) {
    /* 初始化环形缓冲区 */
    ringBuff.Head = 0;
    ringBuff.Tail = 0;
    ringBuff.Lenght = 0;
}

/**
 * @brief       将接收数据写入环形缓冲区函数
 * @param       data:要写入缓冲区的数据
 * @retval      无
 */
void writeRingBuff(uint8_t data) {
    /* 判断缓冲区是否已满 */
    if (ringBuff.Lenght >= RINGBUFF_LEN) {
        return;
    }

    /* 将数据写入环形缓冲区 */
    ringBuff.Ring_data[ringBuff.Tail] = data;
    ringBuff.Tail = (ringBuff.Tail + 1) % RINGBUFF_LEN;    /* 防止越界非法访问 */
    ringBuff.Lenght++;
}

/**
 * @brief       将数据从环形缓冲区删除函数
 * @param       size:要删除的数据长度
 * @retval      无
 */
void deleteRingBuff(uint16_t size) {
    /* 判断删除数据长度是否大于环形缓冲区长度 */
    if (size >= ringBuff.Lenght) {
        initRingBuff();
        return;
    }

    /* 删除环形缓冲区中的数据 */
    for (int i = 0; i < size; i++) {
        if (ringBuff.Lenght == 0) {    /* 判断非空 */
            initRingBuff();
            return;
        }
        ringBuff.Head = (ringBuff.Head + 1) % RINGBUFF_LEN;    /* 防止越界非法访问 */
        ringBuff.Lenght--;
    }
}

/**
 * @brief       从环形缓冲区读取1字节数据函数
 * @param       position:要读取的数据位置
 * @retval      读取到的数据
 */
uint8_t read1BFromRingBuff(uint16_t position) {
    uint16_t realPosition = (ringBuff.Head + position) % RINGBUFF_LEN;

    return ringBuff.Ring_data[realPosition];
}

/**
 * @brief       获取环形缓冲区长度函数
 * @param       无
 * @retval      环形缓冲区长度
 */
uint16_t getRingBuffLenght(void) {
    return ringBuff.Lenght;
}

/**
 * @brief       判断环形缓冲区是否溢出函数
 * @param       无
 * @retval      1:溢出 0:未溢出
 */
uint8_t isRingBuffOverflow(void) {
    return ringBuff.Lenght == RINGBUFF_LEN;
}

/**
 * @brief       判断环形缓冲区是否为空且接收到的数据是否符合数据传输协议，若不符合，则将数据一位一位删除，直到满足数据传输协议为止
 * @param       无
 * @retval      无
 */
void isDataTrue(void) {
    while (usize >= 8) {
        if (u(0) != 'U' && u(0) != 'V' && u(0) != 'W') {
            udelete(1);
        } else {
            break;
        }
    }
}

/**
 * @brief       初始化单电机数据分割结构体函数
 * @param       无
 * @retval      无
 */
void initDataPos(void) {
    dataPos.count_1 = 0;
    dataPos.count_2 = 0;
    dataPos.count_3 = 0;
}

/**
 * @brief       初始化多电机数据分割结构体函数
 * @param       无
 * @retval      无
 */
void initDataPosMul(void) {
    dataPosMul.countMul_1 = 0;
    dataPosMul.countMul_2 = 0;
    dataPosMul.countMul_3 = 0;
    dataPosMul.countMul_4 = 0;
    dataPosMul.countMul_5 = 0;
    dataPosMul.countMul_6 = 0;
    dataPosMul.countMul_7 = 0;
    dataPosMul.countMul_8 = 0;
    dataPosMul.countMul_9 = 0;
}

/**
 * @brief       初始化单电机数据结构体函数
 * @param       无
 * @retval      无
 */
void initGetResult(void) {
    getResult.result_1 = 0;
    getResult.result_2 = 0;
}

/**
 * @brief       初始化多电机数据结构体函数
 * @param       无
 * @retval      无
 */
void initGetResultMul(void) {
    getResultMul.resultMul_1 = 0;
    getResultMul.resultMul_2 = 0;
    getResultMul.resultMul_3 = 0;
    getResultMul.resultMul_4 = 0;
    getResultMul.resultMul_5 = 0;
    getResultMul.resultMul_6 = 0;
    getResultMul.resultMul_7 = 0;
    getResultMul.resultMul_8 = 0;
}

/**
 * @brief       单电机数据分割函数，将单电机数据分割为位置、速度，并记录分割点
 * @param       无
 * @retval      无
 */
void dataDivision(void) {
    int i = 0;
    for (i = 0; i < usize; i++) {
        if (u(i) == 'A') {
            dataPos.count_1 = i;
        }
        else if(u(i) == 'B') {
            dataPos.count_2 = i;
        }
        else if(u(i) == 0x0d && u(i + 1) == 0x0a) {
            dataPos.count_3 = i;
        }
    }
}

/**
 * @brief       多电机数据分割函数，将多电机数据分割为位置1、速度1、位置2、速度2、位置3、速度3、位置4、速度4，并记录分割点
 * @param       无
 * @retval      无
 */
void dataDivisionMul(void) {
    int i = 0;
    for (i = 0; i < usize; i++) {
        if (u(i) == 'A') {
            dataPosMul.countMul_1 = i;
        }
        else if(u(i) == 'B') {
            dataPosMul.countMul_2 = i;
        }
        else if(u(i) == 'C') {
            dataPosMul.countMul_3 = i;
        }
        else if(u(i) == 'D') {
            dataPosMul.countMul_4 = i;
        }
        else if(u(i) == 'E') {
            dataPosMul.countMul_5 = i;
        }
        else if(u(i) == 'F') {
            dataPosMul.countMul_6 = i;
        }
        else if(u(i) == 'G') {
            dataPosMul.countMul_7 = i;
        }
        else if(u(i) == 'H') {
            dataPosMul.countMul_8 = i;
        }
        else if(u(i) == 0x0d && u(i + 1) == 0x0a) {
            dataPosMul.countMul_9 = i;
        }
    }
}

void convertResult(void) {
    for (int l = dataPos.count_1 + 1; l < dataPos.count_2; l++) {
        getResult.result_1 = getResult.result_1 * 10 + (u(l) - '0'); // 将字符转换为数字，并存入result
    }

    for (int l = dataPos.count_2 + 1; l < dataPos.count_3; l++) {
        getResult.result_2 = getResult.result_2 * 10 + (u(l) - '0'); // 将字符转换为数字，并存入result
    }
}

void convertResultMul(void) {
    for (int l = dataPosMul.countMul_1 + 1; l < dataPosMul.countMul_2; l++) {
        getResultMul.resultMul_1 = getResultMul.resultMul_1 * 10 + (u(l) - '0'); // 将字符转换为数字，并存入result
    }

    for (int l = dataPosMul.countMul_2 + 1; l < dataPosMul.countMul_3 - 2; l++) {
        getResultMul.resultMul_2 = getResultMul.resultMul_2 * 10 + (u(l) - '0'); // 将字符转换为数字，并存入result
    }

    for (int l = dataPosMul.countMul_3 + 1; l < dataPosMul.countMul_4; l++) {
        getResultMul.resultMul_3 = getResultMul.resultMul_3 * 10 + (u(l) - '0'); // 将字符转换为数字，并存入result
    }

    for (int l = dataPosMul.countMul_4 + 1; l < dataPosMul.countMul_5 - 2; l++) {
        getResultMul.resultMul_4 = getResultMul.resultMul_4 * 10 + (u(l) - '0'); // 将字符转换为数字，并存入result
    }

    for (int l = dataPosMul.countMul_5 + 1; l < dataPosMul.countMul_6; l++) {
        getResultMul.resultMul_5 = getResultMul.resultMul_5 * 10 + (u(l) - '0'); // 将字符转换为数字，并存入result
    }

    for (int l = dataPosMul.countMul_6 + 1; l < dataPosMul.countMul_7 - 2; l++) {
        getResultMul.resultMul_6 = getResultMul.resultMul_6 * 10 + (u(l) - '0'); // 将字符转换为数字，并存入result
    }

    for (int l = dataPosMul.countMul_7 + 1; l < dataPosMul.countMul_8; l++) {
        getResultMul.resultMul_7 = getResultMul.resultMul_7 * 10 + (u(l) - '0'); // 将字符转换为数字，并存入result
    }

    for (int l = dataPosMul.countMul_8 + 1; l < dataPosMul.countMul_9; l++) {
        getResultMul.resultMul_8 = getResultMul.resultMul_8 * 10 + (u(l) - '0'); // 将字符转换为数字，并存入result
    }
}
