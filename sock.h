#ifndef __SOCK_H__
#define __SOCK_H__
#ifdef __cplusplus  
extern "C"{
#endif


#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#include <sys/ioctl.h>
#include <stdarg.h>

#include <sys/epoll.h>  
#include <netinet/in.h>  
#include <arpa/inet.h>  
#include <fcntl.h>  
#include <unistd.h>  
#include <stdio.h>  
#include <errno.h>  
#include <sys/un.h>
#include "SVG_typedef.h"


#define SOCKET_TCPSOCKET_BUFFERSIZE    (8*1024)
#define LAN_TCPCLIENT_MAX           1024       /* max 8 tcp socket */

#define SOCK_DEFAULT_MAX_EPOLL_EVENTS  16
#define SOCK_DEFAULT_TIMEOUT_MS  3000
#define IEC_CLIENT_INTERNAL_COM_NAME "/tmp/iec-icom-client-%lu"
#define IEC_SERVER_INTERNAL_COM_NAME "/tmp/iec-icom-server-%lu"
#define MB_CLIENT_INTERNAL_COM_NAME "/tmp/mb-icom-client-%lu"
#define MB_SERVER_INTERNAL_COM_NAME "/tmp/mb-icom-server-%lu"
#define TCP_KEEPALIVE_IDLE_TIME  30
#define TCP_KEEPALIVE_INTERVAL_TIME  5
#define TCP_KEEPALIVE_RETRY_COUNT  3


typedef int (*EPOLL_EVENT_HOOK) ( struct epoll_event events[], int num, void * arg) ;

int sock_create_epoll();
int sock_add_epoll_fd(int epoll_fd, int fd);
int sock_close_epoll(int epoll_fd);
int sock_epoll_loop(int epoll_fd, int max_events,int ms_timeout, EPOLL_EVENT_HOOK func, void * func_arg );
int sock_create_internal_com_fd(char * name);
int CreateTcpServer( u_int16_t tcp_port );
void Lan_CreateTCPServer(int *pFd, u_int16_t tcp_port);
//void Lan_CreateNTPClient(int *pFd, u_int16_t tcp_port, char *serverip);

static inline void sock_addr_ip_str(struct sockaddr_in *addr, char *ipaddr)
{
	unsigned char *p = (unsigned char *)&(addr->sin_addr.s_addr);
	sprintf(ipaddr,"%u.%u.%u.%u",p[0],p[1],p[2],p[3]);
}
static inline u_int16_t sock_addr_port(struct sockaddr_in *addr)
{
	return ntohs(addr->sin_port);
}

static inline int set_tcp_keepalive(int sockfd)
{
	int keepAlive = 1;   
	int keepIdle = TCP_KEEPALIVE_IDLE_TIME;  
	int keepInterval = TCP_KEEPALIVE_INTERVAL_TIME; 
	int keepCount = TCP_KEEPALIVE_RETRY_COUNT;   
	setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepAlive, sizeof(keepAlive));
	setsockopt(sockfd, SOL_TCP, TCP_KEEPIDLE, (void*)&keepIdle, sizeof(keepIdle));
	setsockopt(sockfd, SOL_TCP, TCP_KEEPINTVL, (void *)&keepInterval, sizeof(keepInterval));
	setsockopt(sockfd, SOL_TCP, TCP_KEEPCNT, (void *)&keepCount, sizeof(keepCount));
	return 0;
}




#ifdef __cplusplus  
}
#endif
#endif
