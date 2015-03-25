#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#define MAX 100000

int main(int argc, char *argv[])
{
	int sockfd,portno;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	char filename[20];

	char buffer[MAX];
	char ipaddr[20];

	printf("Enter the IP address and port of server\n");
	scanf("%s %d",ipaddr,&portno);

	sockfd=socket(AF_INET,SOCK_STREAM,0);

	if(sockfd<0)
	{
		printf("Error\n");
		exit(0);
	}

	server=gethostbyname(ipaddr);
	if(server==NULL)
	{
		printf("No such host!!\n");
		exit(0);
	}

	bzero((char *)&serv_addr,sizeof(serv_addr));
	serv_addr.sin_family=AF_INET;

	bcopy((char*)server->h_addr,(char *)&serv_addr.sin_addr.s_addr,server->h_length);

	serv_addr.sin_port=htons(portno);

	if(connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr))<0)
	{
		printf("Error connection");
		exit(0);
	}
	
	printf("Enter the filename\n");
	scanf("%s",filename);

	char tosend[100]="GET /";
	strcat(tosend,filename);
	strcpy(filename," HTTP/1.1\n");
	strcat(tosend,filename);

	int status=write(sockfd,tosend,strlen(tosend));
	if(status<0)
	{
		printf("Error ! write failed!!\n");
	}
	printf("here\n");
	while(1)
	{
		int rec=recv(sockfd,buffer,8192,0);
		printf("rec=%d\n",rec);
		if(rec<=0)
			break;
		printf("%s\n",buffer);
	
	}

	close(sockfd);

	return 0;
	
}
