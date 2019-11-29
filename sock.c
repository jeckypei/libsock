
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <poll.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <netinet/in.h>
#include "log.h"

#include "sock.h"
#define M_SOCK "sock"


int sock_create_epoll()
{
	int epoll_fd;
	
	
	epoll_fd = epoll_create(1024);
	if (epoll_fd < 0) {
		slog(M_SOCK, LOG_L_ERR, "epoll create error\n");
	}
	
	return epoll_fd;
}


int sock_add_epoll_fd(int epoll_fd, int fd)
{
	struct epoll_event event;
	event.events = EPOLLIN|EPOLLET|EPOLLRDHUP;
	event.data.fd = fd;
	
	//add Event
	if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) < 0)
	{
		slog(M_SOCK, LOG_L_ERR, "add fd:%d to  epoll %d error\n", fd, epoll_fd);
		return -1;
	}

	return 0;
}


int sock_del_epoll_fd(int epoll_fd, int fd)
{
	struct epoll_event event;
	event.events = EPOLLIN|EPOLLET;
	event.data.fd = fd;
	
	//add Event
	if(epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &event) < 0)
	{
		slog(M_SOCK, LOG_L_ERR, "delete fd:%d to  epoll %d error\n", fd, epoll_fd);
		return -1;
	}

	return 0;
}


int sock_close_epoll(int epoll_fd)
{
	close(epoll_fd);
	return 0;
}


int sock_epoll_loop(int epoll_fd, int max_events,int ms_timeout, EPOLL_EVENT_HOOK func, void * func_arg )
{
	struct epoll_event events[max_events];
	while(1)
	{
		int ret = epoll_wait(epoll_fd, events, max_events, ms_timeout);
		
		if(ret < 0)
		{
			slog(M_SOCK, LOG_L_ERR, "epoll wait error\n");
			break;
		}
		else if(ret == 0)
		{
			slog(M_SOCK, LOG_L_DEBUG, "epoll timeout\n");
			continue;
		}

		(*func)(events, ret, func_arg);
	}
	return 0;
}



int sock_create_internal_com_fd(char * name)
{
	int sockfd;
	struct sockaddr_un src;

	unlink (name);
	sockfd = socket (AF_UNIX, SOCK_DGRAM, 0);
	if (sockfd < 0) {
	   slog(M_SOCK, LOG_L_ERR, "crease unix socket: %s error\n", name);
	   return -1;
	}

	memset (&src, 0, sizeof (src));
	src.sun_family = AF_UNIX;
	strcpy (src.sun_path, name);

	if (bind (sockfd, (struct sockaddr *)&src, sizeof(struct sockaddr_un)) < 0) {
	   slog(M_SOCK, LOG_L_ERR, "find unix socket: %s error \n", name);
	   return -1;
	}
	return sockfd;
}


int CreateTcpServer( u_int16_t tcp_port )
{
    struct sockaddr_in addr;
    int bufferSize=0;
    int serversocketid=0;
	//char server_ip1[20] = "192.168.0.100";
	//char server_ip2[20] = "192.168.101.100";

    serversocketid = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
    if( serversocketid < 0 )
    {
        serversocketid = INVALID_SOCKET;
        LOG_ERR("TCPServer socket create error \n");
        return RET_FAILED;
    }
    bufferSize = SOCKET_TCPSOCKET_BUFFERSIZE;
    setsockopt( serversocketid, SOL_SOCKET, SO_RCVBUF, &bufferSize, 4 );
    setsockopt( serversocketid, SOL_SOCKET, SO_SNDBUF, &bufferSize, 4 );
    memset(&addr, 0x0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(tcp_port);
    addr.sin_addr.s_addr = INADDR_ANY;
    /*if(502 == tcp_port)
	{
	    addr.sin_addr.s_addr = inet_addr(server_ip1);
	}
	else
	{
	     addr.sin_addr.s_addr = inet_addr(server_ip2);
	}*/
    if( bind( serversocketid, (struct sockaddr *)&addr, sizeof(addr)) != 0 )
    {
        LOG_ERR("TCPSrever socket bind error \n");
        close(serversocketid);
        serversocketid = INVALID_SOCKET;
        return RET_FAILED;
    }

    if(listen( serversocketid, LAN_TCPCLIENT_MAX ) != 0 )
    {
        LOG_ERR("TCPServer socket listen error! \n");
        close( serversocketid );
        serversocketid = INVALID_SOCKET;
        return RET_FAILED;
    }
	LOG_INFO("listern tcp port %d,  socket  fd %d \n",tcp_port, serversocketid);
    return serversocketid;
}


void Lan_CreateTCPServer(int *pFd, u_int16_t tcp_port)
{
    *pFd = CreateTcpServer( tcp_port );
}
#if 0
void Lan_CreateNTPClient(int *pFd, u_int16_t tcp_port, char *serverip)
{
    *pFd = CreateNTPClient( tcp_port, serverip );
}
#endif


