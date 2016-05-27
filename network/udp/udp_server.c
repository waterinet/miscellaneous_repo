#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUFLEN 512
#define NPACK 10
#define PORT 9930

void diep(char *s)
{
    perror(s);
    exit(1);
}

int main(void)
{
	/* 
	 * si_me defines the socket where the server will listen.
	 * si_other defines the socket at the other end of the link
	 */
    struct sockaddr_in si_me, si_other;
	int s, i, slen = sizeof(si_other);
	char buf[BUFLEN];

	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
		diep("socket");

	/*
	 * Most machines have more than one network interface, 
	 * Use INADDR_ANY to accept packets from any interface.
	 */
	memset((char *) &si_me, 0, sizeof(si_me));
	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(PORT);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(s, &si_me, sizeof(si_me)) == -1)
		diep("bind");

	/*
	 * Use no special flags, data about the sender should be stored in si_other.
	 * Note that recvfrom() will set slen to the number of bytes actually stored.
	 */
	for (i = 0; i < NPACK; i++) {
		if (recvfrom(s, buf, BUFLEN, 0, &si_other, &slen)==-1)
			diep("recvfrom()");
		printf("Received packet from %s:%d\nData: %s\n\n", 
				inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port), buf);
	}

	close(s);
	return 0;
}
