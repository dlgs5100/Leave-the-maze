#include"tcp.h"  // server
#define LISTENQ 1024

int main(int argc, char **argv)
{
	int listenfd, connfd;
	struct sockaddr_in servaddr;

	if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		perror("socket error");
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = PF_INET;
	servaddr.sin_addr.s_addr = INADDR_ANY;
	servaddr.sin_port        = htons(13000);

	bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	listen(listenfd, LISTENQ);
	connfd = accept(listenfd, (struct sockaddr *) NULL, NULL);
	
	struct msg rcv, snd;
	while(1){

		read(connfd, &rcv, sizeof(rcv));
		
		switch (rcv.cmd)
		{
		case 1:
			if (rcv.status[0] == 2)
				rcv.x -= 1;
			break;
		case 2:
			if (rcv.status[1] == 2)
				rcv.y -= 1;
			break;
		case 3:
			if (rcv.status[2] == 2)
				rcv.y += 1;
			break;
		case 4:
			if (rcv.status[3] == 2)
				rcv.x += 1;
			break;
		default:
			break;
		}

		snd.type = 2;
		snd.cmd = 0;
		snd.x = rcv.x;
		snd.y = rcv.y;

		write(connfd, &snd, sizeof(snd));
	}
	
	close(connfd);
	return 0;
}
