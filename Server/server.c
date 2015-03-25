#include <netdb.h> //bzero()
#include <stdio.h>
#include <string.h>    
#include <stdlib.h>    
#include <sys/socket.h> //socket()
#include <arpa/inet.h> //inet_addr
#include <unistd.h>    //write
#include <pthread.h> //for threading , link with lpthread
#define MAX 10000


void *connection_handler(void *); //the thread function
 
int main(int argc , char *argv[])
{

    int socket_desc , client_sock , c , *new_sock;
    struct sockaddr_in server , client;
     
    //Create socket
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);

    if (socket_desc == -1)
    {
        printf("Could not create socket");
    }
    
    puts("Socket created");
     
    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons( 8000);
     
    //Bind process
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
    {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }

    puts("bind done");
     
    //Listen upto 3 
    listen(socket_desc , 3);
     
    //Accept and incoming connection
    puts("Waiting for incoming connections...");
    c = sizeof(struct sockaddr_in);
     
     
    //Accept and incoming connection
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
         
        //Now join the thread , so that we dont terminate before the thread
        //pthread_join( sniffer_thread , NULL);
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
    
    //Get the socket descriptor
    int sock = *(int*)socket_desc;

     
    //Receive a message from client 
    while( (read_size = recv(sock , client_message , 4096 , 0)) > 0 )
    {
            printf("%s",client_message);

    
            //extracting file_name from client message i.e from get method. 

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

            //file name is stored in fname[].
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

            //sending file name to ipc server.
            write(ipccl,fname,sizeof(fname));
        
            char buffer[MAX];

            //file recieve by ipc client in buffer.
            int sz=recv(ipccl,buffer,MAX,0);
       
            if(sz<0)
            {
        	   printf("File not found!!\n");
            }

            //ipc client code finished and file is stored in buffer variable. 


            //server sends first http header to client. 
            write(sock, "HTTP/1.1 200 OK\n", 16);
            write(sock, "Content-length: 100000\n", 19);
            write(sock, "Content-Type: text/html\n\n", 25);

        
            // server send actual file to client.
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
         
    //Free the socket pointer
    free(socket_desc);
     
    return 0;
}