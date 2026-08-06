#ifndef __PARSER_H__
#define __PARSER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "usart.h"
#include <string.h>


#define PELCO_HEADER           0xff
#define PELCO_LENGTH           7

#define PARSE_ERR_LEN   1   // 数据长度不足
#define PARSE_ERR_HEAD  2   // 帧头错误
#define PARSE_ERR_CHK   3   // 校验和错误

#define COMM_MSG_LENGTH_MAX                     256


#define SET_SENSOR_TEMP                         0xCE0C
#define CONTROL_DEW_SWITCH                      0xCE0B
#define GET_BOARD_TEMP                          0xCE01
#define GET_BOARD_HUMI                          0xCE02
#define GET_TEC_TEMP                            0xCE03
#define GET_AIR_TEMP                            0xCE04
#define GET_DEW_TEMP                            0xCE05

typedef struct
{
    unsigned char header;
    unsigned char data1;
    unsigned char data2;
    unsigned char rqAddr;
    unsigned char rqSeq;
    uint16_t cmd;
    unsigned char checksum;
    unsigned char itemCnt;
    unsigned short netFnBase;
    unsigned short index;
    unsigned short length;
    unsigned short frmLen;
    unsigned char tempVal;
    unsigned char sensor_num;
    unsigned char rqData[COMM_MSG_LENGTH_MAX];
} sCommRqMsg;

extern sCommRqMsg peMsg;

int parse_uart_data(sUart *bus, sCommRqMsg *rqMsg);

#endif
