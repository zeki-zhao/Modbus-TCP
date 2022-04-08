#include "CmpStd.h"
#include "CmpErrors.h"
#include "CmpItf.h"
#include "CmpModbusTCPDep.h"

#include <errno.h>
#include <unistd.h>
#include <modbus.h>
#include <modbus-private.h>
#include <sys/types.h>
#include <arpa/inet.h>

static int Socket = -1; 
static modbus_t *Server;
struct timeval Server_Time;
static modbus_mapping_t *mb_mapping;
static modbus_mapping_t *temp_mb_mapping;
static int Server_Connect = 1;
unsigned long temp_tab_registers = 0;
unsigned long temp_tab_input_registers = 0;
static fd_set refset;
static fd_set rdset;
int master_socket;
static int fdmax;
static int listenfd, connfd;
static pid_t pid;
static int status;
static socklen_t addrlen;
static struct sockaddr_in clientaddr;
static int newfd;


static modbus_t *Cilent;
struct timeval Cilent_Time;
static int Cilent_Connect = 1;
uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];


typedef struct Node {
    int fd;
    struct Node *pNext;
}NODE, *PNODE;

static PNODE pStart;

PNODE create_head(void)
{
    PNODE pHead = (PNODE)malloc(sizeof(NODE));
    printf("pHead addr is %p\r\n",pHead);
    if(NULL == pHead) 
    {
        printf("head malloc error");
        exit(0);
    }
    else
    {
        pHead->pNext = NULL;
        return pHead;
    }
}

int insert_tail(PNODE pHead,int newfd)
{
    PNODE p = pHead;
    PNODE pNew = (PNODE)malloc(sizeof(NODE));
    //printf("size of int is %d, size of Node is %d\r\n",sizeof(int),sizeof(NODE));
    printf("pNew addr is %p\r\n",pNew);
    if(NULL == pNew)
    {
        printf("malloc error\n");
        return -1;
    }
    while(NULL != p->pNext)  // 目的是找到最后一个  节点，最后一个节点的p指向NULL
    {
        p = p->pNext;
    }
    p->pNext = pNew;
    pNew->fd = newfd;
    pNew->pNext = NULL;
    printf("pNew fd is %d\r\n",pNew->fd);
    return pNew->fd;
}

int delete_list(PNODE pHead,int freefd) 
{
    PNODE p = pHead;
 
    while(NULL != p->pNext && freefd != p->pNext->fd)//we have to use p->pNext instead of p, beacuse we need p 
    {
        p = p->pNext;
    }
    if(NULL == p->pNext)//don't have this fd
    {
        printf("fd is error");
        return -1;
    }
    PNODE temp = p->pNext;
    fprintf(stdout,"fd %d has free\r\n",temp->fd);
    p->pNext = temp->pNext;
    free(temp);
    temp = NULL;
    return 0;
}

void printlist(PNODE pHead)
{
    PNODE p = pHead->pNext;
    while(p)
    {
        printf("fd is %d\r\n",p->fd);
        p = p->pNext;
    }
}

void CDECL CDECL_EXT modbus_tcp_client__main(modbus_tcp_client_main_struct *p)
{
    //printf("I'm Cilent!\n");
    int rc = 1;

    if(!p->pInstance->Enable)
    {
        p->pInstance->Done = 0;
        return -1;
    }

    if((p->pInstance->Enable)&&(!p->pInstance->Done))
    {
        if(Cilent_Connect == 1)
        {
            Cilent = modbus_new_tcp(p->pInstance->IP,p->pInstance->Port);
            if (Cilent == NULL) 
            {
                fprintf(stderr, "Unable to allocate libmodbus context: %s\n",modbus_strerror(errno));
                return -1;
            }
        /* set Slave ID*/        
            rc = modbus_set_slave(Cilent, p->pInstance->Unit);
            if (rc == -1) 
            {
                fprintf(stderr, "Invalid slave ID\n");
                p->pInstance->Error = errno;
                modbus_free(Cilent);
                return -1;
            }

            if (modbus_connect(Cilent) == -1) 
            {
                fprintf(stderr, "Connection failed: %s\n",modbus_strerror(errno));
                p->pInstance->Error = 3;
                modbus_free(Cilent);
                return -1;
            }
            
            Cilent_Time.tv_sec=0;
            Cilent_Time.tv_usec=50000;
            modbus_set_response_timeout(Cilent,&Cilent_Time);
            Cilent_Connect = 0;
        }
    
    switch((p->pInstance->Addr)/10000)
    {
        case(0):
        {
            if(p->pInstance->RW)
            {
                rc = modbus_write_bits(Cilent, (p->pInstance->Addr)-1, p->pInstance->Count, (const uint8_t *)p->pInstance->DataReg);
            }
            if(!(p->pInstance->RW))
            {
                rc = modbus_read_bits(Cilent, (p->pInstance->Addr)-1, p->pInstance->Count, (uint8_t *)p->pInstance->DataReg);
            }
        }
        break;

        case(1):
        {
            if(!(p->pInstance->RW))
            rc = modbus_read_input_bits(Cilent, (p->pInstance->Addr)-10001, p->pInstance->Count, (uint8_t *)p->pInstance->DataReg);
        }
        break;

        case(3):
        {
            if(!(p->pInstance->RW))
            rc = modbus_read_input_registers(Cilent, (p->pInstance->Addr)-30001, p->pInstance->Count, p->pInstance->DataReg);
        }
        break;

        case(4):
        {
            if(p->pInstance->RW)
            {
                rc = modbus_write_registers(Cilent, (p->pInstance->Addr)-40001, p->pInstance->Count, p->pInstance->DataReg);
            }
            else if(!(p->pInstance->RW))
            {
                rc = modbus_read_registers(Cilent, (p->pInstance->Addr)-40001, p->pInstance->Count, p->pInstance->DataReg);
            }          
        }
        break;

        default:
            printf("ERROR Addr\n");
            p->pInstance->Error = errno;
            modbus_close(Cilent);
            modbus_free(Cilent);
            Cilent_Connect = 1;
            return -1;
        break;
    } 
            if(rc == -1)
            {
                fprintf(stderr,"ERROR registers\n");
                p->pInstance->Error = errno;
                modbus_close(Cilent);
                modbus_free(Cilent);
                Cilent_Connect = 1;
                return -1;
            }
            else
            {
                errno = 0;
            }
            
            p->pInstance->Done = 1;
            p->pInstance->Error = errno;
    }
}    



void CDECL CDECL_EXT modbus_tcp_server__main(modbus_tcp_server_main_struct *p)//listen函数不要在主循环中。内核里已经循环监听了。
{
    //fprintf(stdout, "I'm Server!\n");
    if(!p->pInstance->Enable)
    {
        printf("modbusTCP unable\n");
        p->pInstance->Done = 0;
        return -1;
    }
    // signal(SIGINT, close_sigint);
    if(Server_Connect == 1)
    {   
        Server = modbus_new_tcp("127.0.0.1",p->pInstance->Port);
        if (Server == NULL) 
        {
            fprintf(stderr, "Unable to allocate libmodbus context: %s\n",modbus_strerror(errno));
            p->pInstance->Error = errno;
            return -1;
        }
        p->pInstance->Done = 1;
        printf("Server is %d\r\n",Server);
        //modbus_set_debug(Server, TRUE); 

        Socket = modbus_tcp_listen(Server, 10);
        fdmax = Socket;
        printf("socket is %d\r\n",Socket);
        if(Socket == -1)
        {
            fprintf(stderr, "Failed to listen: %s\n",modbus_strerror(errno));
            p->pInstance->Error = errno;
            close(Socket);
            modbus_free(Server);
            return -1;
        }
        
        mb_mapping = modbus_mapping_new(0, 0,(p->pInstance->Outputs__Array__Info->diUpper)+1,(p->pInstance->Inputs__Array__Info->diUpper)+1);//it's necessary
        if(mb_mapping == NULL)
        {
            fprintf(stderr, "Failed to allocate the mapping: %s\n",modbus_strerror(errno));
            p->pInstance->Error = errno;
            close(Socket);
            modbus_free(Server);
            return -1;
        } 
        temp_tab_registers = mb_mapping->tab_registers;
        temp_tab_input_registers = mb_mapping->tab_input_registers;

        mb_mapping->tab_registers = (p->pInstance->Outputs);
        mb_mapping->tab_input_registers = (p->pInstance->Inputs);

        Server_Time.tv_sec = 0;
        Server_Time.tv_usec=1000000;
        modbus_set_response_timeout(Server,&Server_Time);
        
        /* Clear the reference set of socket */
        FD_ZERO(&refset);
        /* Add the server socket */
        FD_SET(Socket, &refset);

        fprintf(stderr, " %s\n",modbus_strerror(errno));
        Server_Connect = 0;
    }

        rdset = refset;
        if (select(fdmax+1, &rdset, NULL, NULL, &Server_Time) == -1) {
            perror("Server select() failure.");
            //close_sigint(1);
        }

        /* Run through the existing connections looking for data to be
         * read */
        for (master_socket = 0; master_socket <= fdmax; master_socket++)
        {

            if (!FD_ISSET(master_socket, &rdset)) 
            {
                continue;
            }
            if (master_socket == Socket)
            {
                /* A client is asking a new connection */
                socklen_t addrlen;
                struct sockaddr_in clientaddr;
                int newfd;

                /* Handle new connections */
                addrlen = sizeof(clientaddr);
                memset(&clientaddr, 0, sizeof(clientaddr));
                newfd = accept(Socket, (struct sockaddr *)&clientaddr, &addrlen);
                if (newfd == -1)
                {
                    perror("Server accept() error");
                } 
                else 
                {
                    FD_SET(newfd, &refset);
                    if (newfd > fdmax)
                    {
                        /* Keep track of the maximum */
                        fdmax = newfd;
                    }
                    printf("New connection from %s:%d on socket %d\n",
                           inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port, newfd);
                }
            } 
            else 
            {
                int rc;
                modbus_set_socket(Server, master_socket);
                rc = modbus_receive(Server, query);
                if (rc > 0)
                {
                    modbus_reply(Server, query, rc, mb_mapping);
                } else if (rc == -1)
                {
                    /* This example server in ended on connection closing or
                     * any errors. */
                    printf("Connection closed on socket %d\n", master_socket);
                    close(master_socket);

                    /* Remove from reference set */
                    FD_CLR(master_socket, &refset);

                    if (master_socket == fdmax)
                    {
                        fdmax--;
                    }
                }
            }
        }
}
// {
//     if(!p->pInstance->Enable)
//     {
//         printf("modbusTCP unable\n");
//         return -1;
//     }

//     //fprintf(stdout, "I'm Server!\n");
//     if(Server_Connect == 1)
//     {   
//         pStart = create_head();
//         Server = modbus_new_tcp("127.0.0.1",p->pInstance->Port);
//         if (Server == NULL) 
//         {
//             fprintf(stderr, "Unable to allocate libmodbus context: %s\n",modbus_strerror(errno));
//             p->pInstance->Error = errno;
//             return -1;
//         }
//         printf("Server is %d\r\n",Server);
//         //modbus_set_debug(Server, TRUE); 

//         Socket = modbus_tcp_listen(Server, 10);
//         fdmax = Socket;
//         insert_tail(pStart,fdmax);
//         printf("socket is %d\r\n",Socket);
//         if(Socket == -1)
//         {
//             fprintf(stderr, "Failed to listen: %s\n",modbus_strerror(errno));
//             p->pInstance->Error = errno;
//             close(Socket);
//             modbus_free(Server);
//             return -1;
//         }
        
//         mb_mapping = modbus_mapping_new(0, 0,(p->pInstance->Outputs__Array__Info->diUpper)+1,(p->pInstance->Inputs__Array__Info->diUpper)+1);//it's necessary
//         if(mb_mapping == NULL)
//         {
//             fprintf(stderr, "Failed to allocate the mapping: %s\n",modbus_strerror(errno));
//             p->pInstance->Error = errno;
//             close(Socket);
//             modbus_free(Server);
//             return -1;
//         } 
//         temp_tab_registers = mb_mapping->tab_registers;
//         temp_tab_input_registers = mb_mapping->tab_input_registers;

//         mb_mapping->tab_registers = (p->pInstance->Outputs);
//         mb_mapping->tab_input_registers = (p->pInstance->Inputs);

//         Server_Time.tv_sec = 0;
//         Server_Time.tv_usec=p->pInstance->Delay;
//         modbus_set_response_timeout(Server,&Server_Time);
        
//         /* Clear the reference set of socket */
//         FD_ZERO(&refset);
//         /* Add the server socket */
//         FD_SET(Socket, &refset);

//         fprintf(stderr, " %s\n",modbus_strerror(errno));
//         p->pInstance->Done = 1;
//         Server_Connect = 0;
//     }

//     /* circular from here */
//     rdset = refset;
//     if (select(fdmax+1, &rdset, NULL, NULL, &Server_Time) == -1) {
//         perror("Server select() failure.");
//         //close_sigint(1);
//     }

//     /* Run through the existing connections looking for data to be read */
//     PNODE pnow;
//     for(pnow = pStart->pNext;pnow;pnow=pnow->pNext)
//     {
//         if (!FD_ISSET(pnow->fd, &rdset)) 
//         {
//             continue;
//         }
//         if (pnow->fd == Socket)
//         {
//             /* A client is asking a new connection */  
//             /* Handle new connections */
//             int new;
//             addrlen = sizeof(clientaddr);
//             memset(&clientaddr, 0, sizeof(clientaddr));
//             //printf("clientaddr's add is %p\r\n",&clientaddr);
//             new = insert_tail(pStart,accept(Socket, (struct sockaddr *)&clientaddr, &addrlen));
//             if (new == -1)
//             {
//                 perror("Server accept() error");
//             }
//             else 
//             {
//                 FD_SET(new, &refset);
//                 if (new > fdmax)
//                 {
//                     /* Keep track of the maximum */
//                     fdmax = new;
//                 }
//                 printf("New connection from %s:%d on socket %d\n",inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port, new);
//                 printf("fdmax is %d\r\n",fdmax);
//                 //system("date");
//             }
//         }
//         else 
//         {
//             int rc;
//             modbus_set_socket(Server, pnow->fd);
//             rc = modbus_receive(Server, query);
//             if(rc > 0)
//             {
//                 modbus_reply(Server, query, rc, mb_mapping);
//             } 
//             else if(rc == -1)
//             {
//                 /* This example server in ended on connection closing or
//                  * any errors. */
//                 //system("date");
//                 printf("Connection closed on socket %d\n", pnow->fd);
//                 close(pnow->fd);

//                 /* Remove from reference set */
//                 FD_CLR(pnow->fd, &refset);
//                 if (pnow->fd == fdmax)
//                 {
//                     fdmax--;
//                 }
//                 delete_list(pStart,pnow->fd);
//             }
//         }
//     }
// }



/* fork */
// {
//     //fprintf(stdout, "I'm Server!\n");

//     // signal(SIGINT, close_sigint);
//     if(Server_Connect == 1)
//     {   
//         Server = modbus_new_tcp("127.0.0.1",p->pInstance->Port);
//         if (Server == NULL) 
//         {
//             fprintf(stderr, "Unable to allocate libmodbus context: %s\n",modbus_strerror(errno));
//             p->pInstance->Error = errno;
//             return -1;
//         }
//         //modbus_set_debug(Server, TRUE); 

//         Socket = modbus_tcp_listen(Server, 10);
//         printf("Server is %d\r\n",Server->s);
//         fdmax = Socket;
//         printf("socket is %d\r\n",Socket);
//         if(Socket == -1)
//         {
//             fprintf(stderr, "Failed to listen: %s\n",modbus_strerror(errno));
//             p->pInstance->Error = errno;
//             close(Socket);
//             modbus_free(Server);
//             return -1;
//         }
        
//         mb_mapping = modbus_mapping_new(0, 0,(p->pInstance->Outputs__Array__Info->diUpper)+1,(p->pInstance->Inputs__Array__Info->diUpper)+1);//it's necessary
//         if(mb_mapping == NULL)
//         {
//             fprintf(stderr, "Failed to allocate the mapping: %s\n",modbus_strerror(errno));
//             p->pInstance->Error = errno;
//             close(Socket);
//             modbus_free(Server);
//             return -1;
//         } 
//         temp_tab_registers = mb_mapping->tab_registers;
//         temp_tab_input_registers = mb_mapping->tab_input_registers;

//         mb_mapping->tab_registers = (p->pInstance->Outputs);
//         mb_mapping->tab_input_registers = (p->pInstance->Inputs);

//         Server_Time.tv_sec = 0;
//         Server_Time.tv_usec= p->pInstance->Delay;
//         //modbus_set_response_timeout(Server,&Server_Time);
        
//         /* Clear the reference set of socket */
//         FD_ZERO(&refset);
//         /* Add the server socket */
//         FD_SET(Socket, &refset);

//         fprintf(stderr, " %s\n",modbus_strerror(errno));
//         Server_Connect = 0;
//         int a = 2;
//     }
// //                  socklen_t addrlen;
// //                 struct sockaddr_in clientaddr;
// //                 int newfd;

// //                 /* Handle new connections */
// //                 addrlen = sizeof(clientaddr);
// //                 memset(&clientaddr, 0, sizeof(clientaddr));
// //                 newfd = accept(Socket, (struct sockaddr *)&clientaddr, &addrlen);
//     //connfd = modbus_tcp_accept(Server,&Socket);
//     //printf("a addr is %p,a value is %d\r\n",a,a);
//     pid = fork();
//     if(pid == 0)
//     {
//         int rc;
//         close(Socket);
//         modbus_set_socket(Server, connfd);
//         rc = modbus_receive(Server, query);
//         if (rc > 0) 
//         {
//             /* rc is the query size */
//             modbus_reply(Server, query, rc, mb_mapping);
//         } 
//         else if (rc == -1) 
//         {
//          /* Connection closed by the client or error */
//          fprintf(stderr, "Failed to connection with client: %s\n",modbus_strerror(errno));
//         }
//         printf("transmitting\r\n");
//         exit(0);
//     }
//     if(pid>0)
//     {
//         close(connfd);
//     }

    
//     waitpid(pid,&status,0);
//     //exit(0);
// }



/* select with chain table */
// {
//     //fprintf(stdout, "I'm Server!\n");
//     if(Server_Connect == 1)
//     {   
//         pStart = create_head();
//         Server = modbus_new_tcp("127.0.0.1",p->pInstance->Port);
//         if (Server == NULL) 
//         {
//             fprintf(stderr, "Unable to allocate libmodbus context: %s\n",modbus_strerror(errno));
//             p->pInstance->Error = errno;
//             return -1;
//         }
//         printf("Server is %d\r\n",Server);
//         //modbus_set_debug(Server, TRUE); 

//         Socket = modbus_tcp_listen(Server, 10);
//         fdmax = Socket;
//         insert_tail(pStart,fdmax);
//         printf("socket is %d\r\n",Socket);
//         if(Socket == -1)
//         {
//             fprintf(stderr, "Failed to listen: %s\n",modbus_strerror(errno));
//             p->pInstance->Error = errno;
//             close(Socket);
//             modbus_free(Server);
//             return -1;
//         }
        
//         mb_mapping = modbus_mapping_new(0, 0,(p->pInstance->Outputs__Array__Info->diUpper)+1,(p->pInstance->Inputs__Array__Info->diUpper)+1);//it's necessary
//         if(mb_mapping == NULL)
//         {
//             fprintf(stderr, "Failed to allocate the mapping: %s\n",modbus_strerror(errno));
//             p->pInstance->Error = errno;
//             close(Socket);
//             modbus_free(Server);
//             return -1;
//         } 
//         temp_tab_registers = mb_mapping->tab_registers;
//         temp_tab_input_registers = mb_mapping->tab_input_registers;

//         mb_mapping->tab_registers = (p->pInstance->Outputs);
//         mb_mapping->tab_input_registers = (p->pInstance->Inputs);

//         Server_Time.tv_sec = 0;
//         Server_Time.tv_usec=0;
//         modbus_set_response_timeout(Server,&Server_Time);
        
//         /* Clear the reference set of socket */
//         FD_ZERO(&refset);
//         /* Add the server socket */
//         FD_SET(Socket, &refset);

//         fprintf(stderr, " %s\n",modbus_strerror(errno));
//         Server_Connect = 0;
//     }

//     /* circular from here */
//     rdset = refset;
//     if (select(fdmax+1, &rdset, NULL, NULL, &Server_Time) == -1) {
//         perror("Server select() failure.");
//         //close_sigint(1);
//     }

//     /* Run through the existing connections looking for data to be read */
//     PNODE pnow;
//     for(pnow = pStart->pNext;pnow;pnow=pnow->pNext)
//     {
//         if (!FD_ISSET(pnow->fd, &rdset)) 
//         {
//             continue;
//         }
//         if (pnow->fd == Socket)
//         {
//             /* A client is asking a new connection */  
//             /* Handle new connections */
//             int new;
//             addrlen = sizeof(clientaddr);
//             memset(&clientaddr, 0, sizeof(clientaddr));
//             //printf("clientaddr's add is %p\r\n",&clientaddr);
//             new = insert_tail(pStart,accept(Socket, (struct sockaddr *)&clientaddr, &addrlen));
//             if (new == -1)
//             {
//                 perror("Server accept() error");
//             }
//             else 
//             {
//                 FD_SET(new, &refset);
//                 if (new > fdmax)
//                 {
//                     /* Keep track of the maximum */
//                     fdmax = new;
//                 }
//                 printf("New connection from %s:%d on socket %d\n",inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port, new);
//                 printf("fdmax is %d\r\n",fdmax);
//             }
//         }
//         else 
//         {
//             int rc;
//             modbus_set_socket(Server, pnow->fd);
//             rc = modbus_receive(Server, query);
//             if(rc > 0)
//             {
//                 modbus_reply(Server, query, rc, mb_mapping);
//             } 
//             else if(rc == -1)
//             {
//                 /* This example server in ended on connection closing or
//                  * any errors. */
//                 printf("Connection closed on socket %d\n", pnow->fd);
//                 close(pnow->fd);

//                 /* Remove from reference set */
//                 FD_CLR(pnow->fd, &refset);
//                 if (pnow->fd == fdmax)
//                 {
//                     fdmax--;
//                 }
//                 delete_list(pStart,pnow->fd);
//             }
//         }
//     }
// }



/* select with fd*/
// {
//     //fprintf(stdout, "I'm Server!\n");

//     // signal(SIGINT, close_sigint);
//     if(Server_Connect == 1)
//     {   
//         Server = modbus_new_tcp("127.0.0.1",p->pInstance->Port);
//         if (Server == NULL) 
//         {
//             fprintf(stderr, "Unable to allocate libmodbus context: %s\n",modbus_strerror(errno));
//             p->pInstance->Error = errno;
//             return -1;
//         }
//         printf("Server is %d\r\n",Server);
//         //modbus_set_debug(Server, TRUE); 

//         Socket = modbus_tcp_listen(Server, 10);
//         fdmax = Socket;
//         printf("socket is %d\r\n",Socket);
//         if(Socket == -1)
//         {
//             fprintf(stderr, "Failed to listen: %s\n",modbus_strerror(errno));
//             p->pInstance->Error = errno;
//             close(Socket);
//             modbus_free(Server);
//             return -1;
//         }
        
//         mb_mapping = modbus_mapping_new(0, 0,(p->pInstance->Outputs__Array__Info->diUpper)+1,(p->pInstance->Inputs__Array__Info->diUpper)+1);//it's necessary
//         if(mb_mapping == NULL)
//         {
//             fprintf(stderr, "Failed to allocate the mapping: %s\n",modbus_strerror(errno));
//             p->pInstance->Error = errno;
//             close(Socket);
//             modbus_free(Server);
//             return -1;
//         } 
//         temp_tab_registers = mb_mapping->tab_registers;
//         temp_tab_input_registers = mb_mapping->tab_input_registers;

//         mb_mapping->tab_registers = (p->pInstance->Outputs);
//         mb_mapping->tab_input_registers = (p->pInstance->Inputs);

//         Server_Time.tv_sec = 0;
//         Server_Time.tv_usec=1000000;
//         modbus_set_response_timeout(Server,&Server_Time);
        
//         /* Clear the reference set of socket */
//         FD_ZERO(&refset);
//         /* Add the server socket */
//         FD_SET(Socket, &refset);

//         fprintf(stderr, " %s\n",modbus_strerror(errno));
//         Server_Connect = 0;
//     }

//     //circular from here
//         // _modbus_tcp_select(Server, &read_set, &Server_Time, 0);
//         // int temp = modbus_tcp_accept(Server, &socket);
//         // if(temp!= -1)
//         // {
//         //     errno  = 0; 
//         //     p->pInstance->Error = errno;;
//         // }
//         // fprintf(stderr, "return of accept is %d\r\n",temp);



//         rdset = refset;
//         if (select(fdmax+1, &rdset, NULL, NULL, &Server_Time) == -1) {
//             perror("Server select() failure.");
//             //close_sigint(1);
//         }

//         /* Run through the existing connections looking for data to be
//          * read */
//         for (master_socket = 0; master_socket <= fdmax; master_socket++)
//         {

//             if (!FD_ISSET(master_socket, &rdset)) 
//             {
//                 continue;
//             }
//             if (master_socket == Socket)
//             {
//                 /* A client is asking a new connection */
//                 socklen_t addrlen;
//                 struct sockaddr_in clientaddr;
//                 int newfd;

//                 /* Handle new connections */
//                 addrlen = sizeof(clientaddr);
//                 memset(&clientaddr, 0, sizeof(clientaddr));
//                 newfd = accept(Socket, (struct sockaddr *)&clientaddr, &addrlen);
//                 if (newfd == -1)
//                 {
//                     perror("Server accept() error");
//                 } 
//                 else 
//                 {
//                     FD_SET(newfd, &refset);
//                     if (newfd > fdmax)
//                     {
//                         /* Keep track of the maximum */
//                         fdmax = newfd;
//                     }
//                     printf("New connection from %s:%d on socket %d\n",
//                            inet_ntoa(clientaddr.sin_addr), clientaddr.sin_port, newfd);
//                 }
//             } 
//             else 
//             {
//                 int rc;
//                 modbus_set_socket(Server, master_socket);
//                 rc = modbus_receive(Server, query);
//                 if (rc > 0)
//                 {
//                     modbus_reply(Server, query, rc, mb_mapping);
//                 } else if (rc == -1)
//                 {
//                     /* This example server in ended on connection closing or
//                      * any errors. */
//                     printf("Connection closed on socket %d\n", master_socket);
//                     close(master_socket);

//                     /* Remove from reference set */
//                     FD_CLR(master_socket, &refset);

//                     if (master_socket == fdmax)
//                     {
//                         fdmax--;
//                     }
//                 }
//             }
//         }
// }

/* backups */
// void CDECL CDECL_EXT modbus_tcp_server__main(modbus_tcp_server_main_struct *p)
// {
//     /* codesys controls launch */
//     //fprintf(stdout, "I'm Server!\n");
    
    
//     if(!p->pInstance->Enable)
//     {
//         printf("modbusTCP unable\n");
//         return -1;
//     }

//     /* first into */
//     if(Server_Connect == 1)
//     {   
//         p->pInstance->Done = 1;
//         Server = modbus_new_tcp("127.0.0.1",p->pInstance->Port);
//         if (Server == NULL) 
//         {
//             fprintf(stderr, "Unable to allocate libmodbus context: %s\n",modbus_strerror(errno));
//             p->pInstance->Error = errno;
//             return -1;
//         }
//         printf("Server is %d\r\n",Server);
//         //modbus_set_debug(Server, TRUE); 

//         modbus_set_slave(Server,p->pInstance->Unit);

//         socket = modbus_tcp_listen(Server, 1);
//         printf("socket is %d\r\n",socket);
//         if(socket == -1)
//         {
//             fprintf(stderr, "Failed to listen: %s\n",modbus_strerror(errno));
//             p->pInstance->Error = errno;
//             close(socket);
//             modbus_free(Server);
//             return -1;
//         }
//         int temp = modbus_tcp_accept(Server, &socket);
//         if(temp!= -1)
//         {
//             errno  = 0; 
//             p->pInstance->Error = errno;;
//         }
//         fprintf(stderr, "return of accept is %d\r\n",temp);

//         // Server_Time.tv_sec = 0;
//         // Server_Time.tv_usec=p->pInstance->Delay;
//         // modbus_set_response_timeout(Server,&Server_Time);
//         // Server_Connect = 0;
        
//         mb_mapping = modbus_mapping_new(0, 0,(p->pInstance->Outputs__Array__Info->diUpper)+1,(p->pInstance->Inputs__Array__Info->diUpper)+1);//it's necessary
//         if(mb_mapping == NULL)
//         {
//             fprintf(stderr, "Failed to allocate the mapping: %s\n",modbus_strerror(errno));
//             p->pInstance->Error = errno;
//             close(socket);
//             modbus_free(Server);
//             return -1;
//         } 
// // printf("1 num is %d,2 num is %d,3 num is %d,4 num is %d\r\n ",mb_mapping->nb_bits,mb_mapping->nb_input_bits,mb_mapping->nb_input_registers,mb_mapping->nb_registers);
// // printf("former tab_bits add is %p\r\n",mb_mapping->tab_bits);printf("former tab_input_bits add is %p\r\n",mb_mapping->tab_input_bits);
// // printf("former tab_input_registers add is %p\r\n",mb_mapping->tab_input_registers);printf("former tab_registers add is %p\r\n",mb_mapping->tab_registers);
//         temp_tab_registers = mb_mapping->tab_registers;
//         temp_tab_input_registers = mb_mapping->tab_input_registers;

//         mb_mapping->tab_registers = (p->pInstance->Outputs);
//         mb_mapping->tab_input_registers = (p->pInstance->Inputs);
        
//         fprintf(stderr, " %s\n",modbus_strerror(errno));
//         Server_Connect = 0;
//     }

//     /* massage transmission*/
//     int rc;
//     rc = modbus_receive(Server, query);
//     if (rc > 0) 
//     {
//         /* rc is the query size */
//         modbus_reply(Server, query, rc, mb_mapping);
//     } 
//     else if (rc == -1) 
//     {
//         /* Connection closed by the client or error */
//         fprintf(stderr, "Failed to connection with client: %s\n",modbus_strerror(errno));
//         p->pInstance->Error = errno;
//         mb_mapping->tab_registers = temp_tab_registers;
//         mb_mapping->tab_input_registers = temp_tab_input_registers;
// 		modbus_mapping_free(mb_mapping);
//         close(socket);
// 		modbus_close(Server);
// 		modbus_free(Server);
//         Server_Connect = 1;
//     }
//     //printf("bye!\r\n");
// }


/* chain table */
// if(Server_Connect == 1)
// {
//     PNODE pHead = create_head();
//     for(int i=0;i<10;i++)
//     {
//         insert_tail(pHead,i);
//     }
//     delete_list(pHead,0);
//     delete_list(pHead,9);
//     delete_list(pHead,7);
//     insert_tail(pHead,10);
//     delete_list(pHead,8);
//     insert_tail(pHead,9);
//     printlist(pHead);
//     Server_Connect = 0;
// }