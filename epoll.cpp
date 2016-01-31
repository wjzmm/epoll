#include "epoll.h"


int RTask::Run(){
	Data *ptData = (Data*)m_ptrData;
	char buf[MAX_SIZE];
	int nread, ntotal = 0;
	while((nread = read(ptData->fd, buf, MAX_SIZE)) > 0){
		ntotal += nread;
	}
	printf("%s\n", buf);
	if(nread == -1 && errno != EAGAIN){
		printf("read error\n");
	}
	int total_send = send(ptData->fd,buf,sizeof(buf),0);
	struct epoll_event event;
	event.data.fd = ptData->fd;
	event.events = EPOLLIN | EPOLLET;
	epoll_ctl(ptData->epfd, EPOLL_CTL_MOD,ptData->fd, &event);
	return 0;
}

EPollServer::EPollServer(int port){
	serv_port = port;
	printf("123");
	memset(buf, 0, BUF_SIZE);
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_adr.sin_port = htons(serv_port);
//	epfd = epoll_create(EPOLL_SIZE);
	ep_events = static_cast<struct epoll_event*>(malloc(sizeof(struct epoll_event) * EPOLL_SIZE));

	printf("123 ad %d", port);
}
bool EPollServer::init(){
	printf("init");
	if((serv_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
		// 日志记录，以后完成throw "socket() failed";
		printf("socket failed\n");
		return false;
	}
	SetNoBlock(serv_sock);
	if(bind(serv_sock, (sockaddr*)&serv_adr, sizeof(serv_adr)) < 0){
		//throw "bind() error";
		printf("bind failed\n");
		return false;
	}
	if(listen(serv_sock, 10) < 0){
		//throw "listen() error";
		printf("listen error\n");
		return false;
	}
//	epfd = epoll_create(EPOLL_SIZE);
	if (epfd <= 0) {
		printf("epoll create error\r\n");
		return false;
	}
	printf("1");
	CThreadPool pool(20);
	event.events = EPOLLIN;
	event.data.fd = serv_sock;
	epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);
	while(1){
		event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
		printf("waiting... event_size:%u\n",event_cnt);
		if(event_cnt == -1){
			printf("epool_wait() error");
			return false;
		}
		for(int i = 0; i < event_cnt; ++i){
			if(ep_events[i].events & EPOLLIN && ep_events[i].data.fd == serv_sock){
				socklen_t adr_sz = sizeof(clnt_sock);
				clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
				if(clnt_sock > 0){
					SetNoBlock(clnt_sock);
					event.events = EPOLLIN | EPOLLET;
					event.data.fd = clnt_sock;
					SetNoBlock(clnt_sock);
					epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
				} else {
					if(errno == EAGAIN) {
						printf("listen queue is full");
					}
				}
			} else if ((!(ep_events[i].events & EPOLLIN)) || (ep_events[i].events & EPOLLHUP) || (ep_events[i].events & EPOLLERR)) {
				event.events = EPOLLIN;
				epoll_ctl(epfd, EPOLL_CTL_DEL,ep_events[i].data.fd, &event);
				close(ep_events[i].data.fd);
				printf("close fd:%u",ep_events[i].data.fd);
			} else if(ep_events[i].events & EPOLLIN) { 
				printf("read comes\r\n");
				Data mdata;
				mdata.fd = ep_events[i].data.fd;	
				mdata.epfd = epfd;
				mdata.msg = "this is a test msg";
				RTask *rtask = new RTask();
				rtask->SetData((void*)&mdata);
				pool.AddTask(rtask);
				/*char buf[MAX_SIZE];
				memset(buf, 0, MAX_SIZE);
				printf("receive\n");
				int n = recv(ep_events[i].data.fd, buf, MAX_SIZE, 0);
				if(n < 0){
					printf("error");
				}
				printf("%s\n", buf);
				*/
			} else {
				//close(ep_events[i].data.fd);
				printf("else");
				continue;
			} 
				
		}
	}
}

bool EPollServer::SetNoBlock(int fd){
	int flags = fcntl(fd, F_GETFL, 0);
	flags |= O_NONBLOCK;
	if(-1 == fcntl(fd, F_SETFL, flags))
		return false;
	return true;
}

int EPollServer::recv_msg(){
	while(1){
		socklen_t adr_size = sizeof(struct sockaddr_in);
		if((clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_size)) == -1){
			throw "accept() error";
			continue;
		}
		printf("Receive a connection from %s\n", (char *)inet_ntoa(clnt_adr.sin_addr));

		if(!fork()){
			char buf[MAX_SIZE];
			memset(buf, 0, MAX_SIZE);
			if((read(clnt_sock, buf, MAX_SIZE)) < 0){
				throw "read() error";
			}else{
				printf("Received message: %s\n", buf);
				break;
			}
			exit(0);
		}
		close(clnt_sock);
	}
	return 0;
}
