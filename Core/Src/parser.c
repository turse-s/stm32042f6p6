#include "parser.h"
#include "usart.h"
#include "sensor.h"

sCommRqMsg peMsg;

int parse_uart_data(sUart *bus, sCommRqMsg *rqMsg)
{
    int err = 0;
    unsigned long checksum = 0;
    float temp = 0;
    
    memset(rqMsg->rqData, 0, sizeof(rqMsg->rqData));
    memmove(rqMsg->rqData, bus->rxBuf, bus->rxCnt);
    
    rqMsg->length = bus->rxCnt;
    rqMsg->index = 0;
    rqMsg->header = rqMsg->rqData[rqMsg->index++];
    rqMsg->index++;
    rqMsg->cmd = (rqMsg->rqData[rqMsg->index]<<8) | rqMsg->rqData[rqMsg->index+1];
    rqMsg->index += 2;
    rqMsg->data1 = rqMsg->rqData[rqMsg->index++];
    rqMsg->data2 = rqMsg->rqData[rqMsg->index++];
    rqMsg->checksum = rqMsg->rqData[rqMsg->index++];
    
    if (rqMsg->length < PELCO_LENGTH) {
        return PARSE_ERR_LEN;
    }
    
    if (rqMsg->header != PELCO_HEADER) {
        return PARSE_ERR_HEAD;
    }
    
    for(int i = 1; i < PELCO_LENGTH - 1; i++) {
        checksum += rqMsg->rqData[i];
    }
    
    if (rqMsg->checksum != (checksum & 0xff)) {
        return PARSE_ERR_CHK;
    }
    
    switch(rqMsg->cmd) {
        case SET_SENSOR_TEMP:
            temp = ((rqMsg->data1 << 8) | rqMsg->data2) / 10;
            sensor.temp = temp;
            break;
        case CONTROL_DEW_SWITCH:
            sensor.dewBtn = rqMsg->data1;
            break;
        case GET_BOARD_TEMP:
            uart_Send(rqMsg->cmd,sensor.boardTemp);
            break;
        case GET_BOARD_HUMI:
            uart_Send(rqMsg->cmd,sensor.boardHumi);
            break;
        case GET_TEC_TEMP:
            uart_Send(rqMsg->cmd,sensor.tecTemp);
            break;
        case GET_AIR_TEMP:
            uart_Send(rqMsg->cmd,sensor.airTemp);
            break;
        case GET_DEW_TEMP:
            uart_Send(rqMsg->cmd,sensor.dewPointTemp);
            break;
        default:
            break;
        
    }
    
    uart2.frameReady = 0;
}
 