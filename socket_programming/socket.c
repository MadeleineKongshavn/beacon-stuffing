#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#define IP_PROTOCOL_VAL 0

//Structure of the data which should be sent! :)
struct attr {
	char attr_id[32];
	u_int16_t dis;
};

int main()
{
	//definitions of structures and variables to use in this function.
	struct sockaddr_in ip4addr;
	int sockfd = socket(AF_INET, SOCK_STREAM, IP_PROTOCOL_VAL);



	//int setsockopt(int sockfd, int level, int optname,const void *optval, socklen_t optlen);


//	printf("Hello world");
	return 0;
}
