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


#define SERVER_PORT 4002
#define IP_PROTOCOL_VAL 0

//Structure of the data which should be sent! :)
struct attr {
	char attr_id[32];
	u_int16_t dis;
};

int main()
{
	//definitions of structures and variables to use in this function.
	int server_id;
	struct sockaddr_in ip4addr;
	int sock_fd = socket(AF_INET, SOCK_STREAM, IP_PROTOCOL_VAL);

	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( PORT );

 	bind(sock_fd, (struct sockaddr *) &ip4addr, sizeof(ip4addr));

	return 0;
}
