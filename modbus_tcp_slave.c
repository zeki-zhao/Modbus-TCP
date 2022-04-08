#include<stdio.h>
#include<stdlib.h>
#include <errno.h>
#include"modbus.h"
 
int main(int argc, char *argv[])
{
    modbus_t *mb = NULL;
    uint16_t tab_reg[32]={0};

    modbus_mapping_t *mb_mapping = NULL;
    int regs,rc;
  
    mb = modbus_new_tcp("192.168.0.200",601);//由于是tcp client 连接，在同一程序中相同的端口可以连接多次
    modbus_set_slave(mb,1);//从机地址
    modbus_connect(mb);
  
    struct timeval t;
    t.tv_sec=0;
    t.tv_usec=3000000;  //设置modbus超时时间为1000毫秒  
    modbus_set_response_timeout(mb,&t);
 
 //寄存器map初始化
    mb_mapping = modbus_mapping_new(MODBUS_MAX_READ_BITS, 0,
                                    MODBUS_MAX_READ_REGISTERS, 0);
    if (mb_mapping == NULL) {
        fprintf(stderr, "Failed to allocate the mapping: %s\n",
                modbus_strerror(errno));
        modbus_free(mb);
        return -1;
    }
 
 //初始几个寄存器
    mb_mapping->tab_registers[0] =argv[1];
    mb_mapping->tab_registers[1] =argv[2];
    mb_mapping->tab_registers[2] =argv[3];
    mb_mapping->tab_registers[3] =argv[4];
    mb_mapping->tab_registers[4] =argv[5];
    mb_mapping->tab_registers[5] =argv[6];
    mb_mapping->tab_registers[6] =argv[7];
  //int regs=modbus_read_registers(mb,0,20,tab_reg);
while(1)
{
    int8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
    //轮询接收数据，并做相应处理
    rc = modbus_receive(mb, query);
    if (rc > 0) 
    {
        modbus_reply(mb, query, rc, mb_mapping);
    } 
    else if (rc  == -1) 
    {
      /* Connection closed by the client or error */
       break;
    }
}
    printf("Quit the loop: %s\n", modbus_strerror(errno));
    modbus_mapping_free(mb_mapping);
    modbus_close(mb);
    modbus_free(mb);
    return 0;
}
