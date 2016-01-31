#ifndef __EPOLL_SERVER__
#define __EPOLL_SERVER__

#include "headers.h"
#include "task.h"
#include "pool.h"
#define MAX_SIZE 1024
#define EPOLL_SIZE 50
#define BUF_SIZE 100

using namespace std;

struct Data{
	int fd;
	int epfd;
	string msg;
};

class RTask: public CTask{ 
	public:
		RTask(){}
		virtual int Run();
};
class EPollServer{
	private:
		int serv_sock, clnt_sock;
		sockaddr_in serv_adr;
		sockaddr_in clnt_adr;
		int serv_port;
		int epfd, event_cnt;
		struct epoll_event *ep_events;
		struct epoll_event event;
		char buf[BUF_SIZE];
		
	public:
		EPollServer(int port = 31143);
		bool init();
		int recv_msg();
		bool SetNoBlock(int fd);
};

#endif
