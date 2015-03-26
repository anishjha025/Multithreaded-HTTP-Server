#include <netdb.h> 
#include <stdio.h>
#include <string.h>    
#include <stdlib.h>    
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h>    
#include <pthread.h> 
#define MAX 10000


void *connection_handler(void *); 
 
int main(int argc , char *argv[])
{

    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;
     
    
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);

    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    
    puts("Socket created");
     
   
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8000);
     
   
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("bind failed. Error");
        return 1;
    }

    puts("bind done");
     
   
    listen(socket_desc , 3);
     
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
    
    while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
    {
        puts("Connection accepted");
         
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;
         
        if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
     
        puts("Handler assigned");
    }
     
    if (client_sock < 0)
    {
        perror("accept failed");
        return 1;
    }
     
    return 0;
}
 
/*
 * This fuction will handle connection for each client
 * */

void *connection_handler(void *socket_desc)
{
    
    int read_size;
    
    char *message , client_message[100000];
    
    
    int sock = *(int*)socket_desc;

     
   
    while( (read_size = recv(sock , client_message , 4096 , 0)) > 0 )
    {
            printf("%s",client_message);

            char fname[100];
            int i=0;
            int j=0;
            char temp[100];

            while(client_message[i]!='\n')
            {
                temp[j++]=client_message[i];
                i++;
            }

            temp[j]='\0';
            j=0;

            for(i=5;temp[i]!=' ';i++)
            {
                fname[j++]=temp[i];
            }

            fname[j]='\0';

            printf("File to be sent :%s\n",fname);
	
	
            /*
            Ipc client code.
        
            Ipc client try to connect ipc server for file i.e stored in fname[]
            Ipc client sends file name to ipc server
            Ipc server send file to ipc client.
            */  

            int ipccl,port;
            struct sockaddr_in ipc;
            struct hostent *ipc_cl;

            port=12345;
            ipccl=socket(AF_INET,SOCK_STREAM,0);
            
            if(ipccl<0)
            {
        	   printf("Error opening socket!!");

            }
            ipc_cl = gethostbyname("127.0.0.1");
            
            if(ipc_cl==NULL)
            {
        	   printf("IPC server not running\n");
        	   exit(0);
            }

            bzero((char *) &ipc, sizeof(ipc));

            ipc.sin_family=AF_INET;
        
            bcopy((char *)ipc_cl->h_addr,(char *)&ipc.sin_addr.s_addr,ipc_cl->h_length);
        
            ipc.sin_port=htons(port);

            if(connect(ipccl,(struct sockaddr *)&ipc,sizeof(ipc))<0)
            {
        	   printf("Error!!\n");
            }

            write(ipccl,fname,sizeof(fname));
        
            char buffer[MAX];

            int sz=recv(ipccl,buffer,MAX,0);
       
            if(sz<0)
            {
        	   printf("File not found!!\n");
            }

            write(sock, "HTTP/1.1 200 OK\n", 16);
            write(sock, "Content-length: 100000\n", 19);
            write(sock, "Content-Type: text/html\n\n", 25);

            printf("Sending file\n");
            write(sock,buffer,sz);

            close(ipccl);

    }
     
    if(read_size == 0)
    {
        puts("Client disconnected");
        fflush(stdout);
    }
    else if(read_size == -1)
    {
        perror("recv failed");
    }
        
    free(socket_desc);
     
    return 0;
}
