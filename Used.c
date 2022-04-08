#include "CmpStd.h"
#include "CmpErrors.h"
#include "CmpItf.h"
#include "CmpModbusTCPDep.h"

#include <errno.h>
#include <unistd.h>
#include <modbus.h>

#define LOOP             1
#define SERVER_ID        1
#define ADDRESS_START    0
#define ADDRESS_END     10
#define master 1
#define slave 1

void CDECL CDECL_EXT modbus_tcp_master__main(modbus_tcp_master_main_struct *p)
{
    printf("I'm in master!\n");

#if master
    modbus_t *ctx;
    int rc;
    int nb_fail;
    int nb_loop;
    int addr;
    int nb;
    uint8_t *tab_rq_bits;
    uint8_t *tab_rp_bits;
    uint16_t *tab_rq_registers;
    uint16_t *tab_rw_rq_registers;
    uint16_t *tab_rp_registers;

    if(!p->pInstance->Enable)
    {
        return -1;
    }

/* TCP */
    ctx = modbus_new_tcp(p->pInstance->IP,p->pInstance->Port);
    modbus_set_debug(ctx, TRUE);
 
/* set Slave ID*/
    rc = modbus_set_slave(ctx, SERVER_ID);
    if (rc == -1) {
        fprintf(stderr, "Invalid slave ID\n");
        modbus_free(ctx);
        return -1;
    }

    if (modbus_connect(ctx) == -1) 
    {
        fprintf(stderr, "Connection failed: %s\n",
               modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }
 
    /* Allocate and initialize the different memory spaces */
    nb = ADDRESS_END - ADDRESS_START;
 
    tab_rq_bits = (uint8_t *) malloc(nb * sizeof(uint8_t));
    memset(tab_rq_bits, 0, nb * sizeof(uint8_t));
 
    tab_rp_bits = (uint8_t *) malloc(nb * sizeof(uint8_t));
    memset(tab_rp_bits, 0, nb * sizeof(uint8_t));
 
    tab_rq_registers = (uint16_t *) malloc(nb * sizeof(uint16_t));
    memset(tab_rq_registers, 0, nb * sizeof(uint16_t));
 
    tab_rp_registers = (uint16_t *) malloc(nb * sizeof(uint16_t));
    memset(tab_rp_registers, 0, nb * sizeof(uint16_t));
 
    tab_rw_rq_registers = (uint16_t *) malloc(nb * sizeof(uint16_t));
    memset(tab_rw_rq_registers, 0, nb * sizeof(uint16_t));
 
    nb_loop = nb_fail = 0;
    while (nb_loop++ < LOOP) 
    {
        for (addr = ADDRESS_START; addr < ADDRESS_END; addr++) 
        {
            int i;
            /* Random numbers (short) */
            for (i=0; i<nb; i++) 
            {
                tab_rq_registers[i] = (uint16_t) p->pInstance->Data;
                tab_rw_rq_registers[i] = p->pInstance->Data;
                tab_rq_bits[i] = 6;
            }
            nb = ADDRESS_END - addr;

            switch(p->pInstance->Functions)
            {
            
            case(0x05):
                /* WRITE BIT */
                rc = modbus_write_bit(ctx, addr, tab_rq_bits[0]);
                if (rc != 1) 
                {
                    printf("ERROR modbus_write_bit (%d)\n", rc);
                    printf("Address = %d, value = %d\n", addr, tab_rq_bits[0]);
                    nb_fail++;
                } 
                else 
                {
                    rc = modbus_read_bits(ctx, addr, 1, tab_rp_bits);
                    if (rc != 1 || tab_rq_bits[0] != tab_rp_bits[0]) 
                    {
                        printf("ERROR modbus_read_bits single (%d)\n", rc);
                        printf("address = %d\n", addr);
                        nb_fail++;
                    }
                }
            break;

            case(0x0f):
                /* MULTIPLE BITS */
                rc = modbus_write_bits(ctx, addr, nb, tab_rq_bits);
                if (rc != nb) 
                {
                    printf("ERROR modbus_write_bits (%d)\n", rc);
                    printf("Address = %d, nb = %d\n", addr, nb);
                    nb_fail++;
                } 
                else 
                {
                    rc = modbus_read_bits(ctx, addr, nb, tab_rp_bits);
                    if (rc != nb) 
                    {
                        printf("ERROR modbus_read_bits\n");
                        printf("Address = %d, nb = %d\n", addr, nb);
                        nb_fail++;
                    } 
                    else 
                    {
                        for (i=0; i<nb; i++) 
                        {
                            if (tab_rp_bits[i] != tab_rq_bits[i]) 
                            {
                                printf("ERROR modbus_read_bits\n");
                                printf("Address = %d, value %d (0x%X) != %d (0x%X)\n",
                                    addr, tab_rq_bits[i], tab_rq_bits[i],
                                    tab_rp_bits[i], tab_rp_bits[i]);
                                nb_fail++;
                            }
                        }
                    }
                }
            break;

            case(0x06):
                /* SINGLE REGISTER */
                rc = modbus_write_register(ctx, addr, tab_rq_registers[0]);
                if (rc != 1) 
                {
                    printf("ERROR modbus_write_register (%d)\n", rc);
                    printf("Address = %d, value = %d (0x%X)\n",
                        addr, tab_rq_registers[0], tab_rq_registers[0]);
                    nb_fail++;
                } 
                else 
                {
                    rc = modbus_read_registers(ctx, addr, 1, tab_rp_registers);
                    if (rc != 1) 
                    {
                        printf("ERROR modbus_read_registers single (%d)\n", rc);
                        printf("Address = %d\n", addr);
                        nb_fail++;
                    } 
                    else 
                    {
                        if (tab_rq_registers[0] != tab_rp_registers[0]) 
                        {
                            printf("ERROR modbus_read_registers single\n");
                            printf("Address = %d, value = %d (0x%X) != %d (0x%X)\n",
                                addr, tab_rq_registers[0], tab_rq_registers[0],
                                tab_rp_registers[0], tab_rp_registers[0]);
                            nb_fail++;
                        }
                    }
                }
            break;

            case(0x10):
                /* MULTIPLE REGISTERS */
                rc = modbus_write_registers(ctx, addr, nb, tab_rq_registers);
                if (rc != nb) 
                {
                    printf("ERROR modbus_write_registers (%d)\n", rc);
                    printf("Address = %d, nb = %d\n", addr, nb);
                    nb_fail++;
                } 
                else 
                {
                    rc = modbus_read_registers(ctx, addr, nb, tab_rp_registers);
                    if (rc != nb) 
                    {
                        printf("ERROR modbus_read_registers (%d)\n", rc);
                        printf("Address = %d, nb = %d\n", addr, nb);
                        nb_fail++;
                    } 
                    else 
                    {
                        for (i=0; i<nb; i++) 
                        {
                            if (tab_rq_registers[i] != tab_rp_registers[i]) 
                            {
                                printf("ERROR modbus_read_registers\n");
                                printf("Address = %d, value %d (0x%X) != %d (0x%X)\n",
                                    addr, tab_rq_registers[i], tab_rq_registers[i],
                                    tab_rp_registers[i], tab_rp_registers[i]);
                                nb_fail++;
                            }
                        }
                    }
                }
            break;


            /* R/W MULTIPLE REGISTERS */
            /*rc = modbus_write_and_read_registers(ctx,
                                                 addr, nb, tab_rw_rq_registers,
                                                 addr, nb, tab_rp_registers);
            if (rc != nb) 
            {
                printf("ERROR modbus_read_and_write_registers (%d)\n", rc);
                printf("Address = %d, nb = %d\n", addr, nb);
                nb_fail++;
            } else {
                for (i=0; i<nb; i++) {
                    if (tab_rp_registers[i] != tab_rw_rq_registers[i]) {
                        printf("ERROR modbus_read_and_write_registers READ\n");
                        printf("Address = %d, value %d (0x%X) != %d (0x%X)\n",
                               addr, tab_rp_registers[i], tab_rw_rq_registers[i],
                               tab_rp_registers[i], tab_rw_rq_registers[i]);
                        nb_fail++;
                    }
                }
 
                rc = modbus_read_registers(ctx, addr, nb, tab_rp_registers);
                if (rc != nb) {
                    printf("ERROR modbus_read_registers (%d)\n", rc);
                    printf("Address = %d, nb = %d\n", addr, nb);
                    nb_fail++;
                } else {
                    for (i=0; i<nb; i++) {
                        if (tab_rw_rq_registers[i] != tab_rp_registers[i]) {
                            printf("ERROR modbus_read_and_write_registers WRITE\n");
                            printf("Address = %d, value %d (0x%X) != %d (0x%X)\n",
                                   addr, tab_rw_rq_registers[i], tab_rw_rq_registers[i],
                                   tab_rp_registers[i], tab_rp_registers[i]);
                            nb_fail++;
                        }
                    }
                }
            }
            */
             /*result*/
             //sleep(1);
        }
       
        printf("Test: ");
        if (nb_fail)
            printf("%d FAILS\n", nb_fail);
        else
            printf("SUCCESS\n");
    }
 
    /* Free the memory */
    free(tab_rq_bits);
    free(tab_rp_bits);
    free(tab_rq_registers);
    free(tab_rp_registers);
    free(tab_rw_rq_registers);
 
    /* Close the connection */
    modbus_close(ctx);
    modbus_free(ctx);
 
    return 0;
}    
#endif

}


void CDECL CDECL_EXT modbus_tcp_slave__main(modbus_tcp_slave_main_struct *p)
{
    printf("I'm in master!\n");

#if slave
    if(!p->pInstance->Enable)
    {
        printf("modbusTCP unable\n");
        return -1;
    }
    int socket = -1;
    modbus_t *ctx;
    modbus_mapping_t *mb_mapping;

    ctx = modbus_new_tcp(p->pInstance->IP,p->pInstance->Port);
     modbus_set_debug(ctx, TRUE); 

    mb_mapping = modbus_mapping_new(500, 500, 500, 500);
    if (mb_mapping == NULL) {
        fprintf(stderr, "Failed to allocate the mapping: %s\n",
                modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }

    socket = modbus_tcp_listen(ctx, 1);
    modbus_tcp_accept(ctx, &socket);
    while(p->pInstance->Enable)
    {
        //p->pInstance->First
        uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
        int rc;
        rc = modbus_receive(ctx, query);
        if (rc > 0) {
            /* rc is the query size */
            modbus_reply(ctx, query, rc, mb_mapping);
        } else if (rc == -1) {
            /* Connection closed by the client or error */
            break;
        }
      //  sleep(1);
    }

    printf("Quit the loop: %s\n", modbus_strerror(errno));

    if (socket != -1) {
        close(socket);
    }
    modbus_mapping_free(mb_mapping);
    modbus_close(ctx);
    modbus_free(ctx);
#endif

}


void CDECL CDECL_EXT modbus_rtu_master__main(modbus_rtu_master_main_struct *p)
{
    modbus_t *mb;
    uint16_t tab_reg[64]={0};

    //1-打开端口
    mb = modbus_new_rtu("/dev/ttyO2",9600,'N',8,1);

    //2-设置从地址
    modbus_set_slave(mb,1);

    //3-建立连接
    modbus_connect(mb);

    //4-设置应答延时
    struct timeval t;
    t.tv_sec=0;
    t.tv_usec=1000000;//1000ms
    modbus_set_response_timeout(mb,&t);

    //5-循环读
    int num = 0;
    while(1)
    {   
        memset(tab_reg,0,64*2);

        //6-读寄存器设置：寄存器地址、数量、数据缓冲
        int regs=modbus_read_registers(mb, 0, 20, tab_reg);
       
        printf("-------------------------------------------\n");
        printf("[%4d][read num = %d]",num,regs);
        num++;
        
        int i;
        for(i=0; i<20; i++)
        {
            printf("<%#x>",tab_reg[i]);
        }
        printf("\n");
        printf("-------------------------------------------\n");
        sleep(1);
    }

    //7-关闭modbus端口
    modbus_close(mb);

    //8-释放modbus资源
    modbus_free(mb);
    return 0;
}


void CDECL CDECL_EXT modbus_rtu_slave__main(modbus_rtu_slave_main_struct *p)
{
 //   printf("Input 6 nums you want send\n");
    int s = -1;
    modbus_t *ctx = NULL;
    modbus_mapping_t *mb_mapping = NULL;
    int rc;
    int use_backend;
    //初始化modbus rtu
    ctx = modbus_new_rtu("/dev/ttyO2", 9600, 'N', 8, 1);
    //设定从设备地址
    modbus_set_slave(ctx, 1);
    //modbus连接
    modbus_connect(ctx);
    //寄存器map初始化
    mb_mapping = modbus_mapping_new(MODBUS_MAX_READ_BITS, 0,
                                    MODBUS_MAX_READ_REGISTERS, 0);
    if (mb_mapping == NULL) {
        fprintf(stderr, "Failed to allocate the mapping: %s\n",
                modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }
 
   //初始几个寄存器
   mb_mapping->tab_registers[0] =1;
   mb_mapping->tab_registers[1] =2;
   mb_mapping->tab_registers[2] =3;
   mb_mapping->tab_registers[3] =4;
   mb_mapping->tab_registers[4] =5;
   mb_mapping->tab_registers[5] =6;
   mb_mapping->tab_registers[6] =7;
   //循环
   while( 1 ){
        uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
        //轮询接收数据，并做相应处理
        rc = modbus_receive(ctx, query);
        if (rc > 0) {
            modbus_reply(ctx, query, rc, mb_mapping);
        } else if (rc  == -1) {
            /* Connection closed by the client or error */
            break;
        }
    }
 
    printf("Quit the loop: %s\n", modbus_strerror(errno));
 
    modbus_mapping_free(mb_mapping);
    if (s != -1) {
        close(s);
    }
    /* For RTU, skipped by TCP (no TCP connect) */
    modbus_close(ctx);
    modbus_free(ctx);
 
    return 0;
}


