#include "epoll.h"
int main() {
	EPollServer server(22222);
	printf("111");
	server.init();
}
