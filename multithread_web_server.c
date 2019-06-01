 #include <string.h> // for strlen
#include <sys/socket.h>
#include <arpa/inet.h> // for inet_addr
#include <unistd.h>    // for write
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <fcntl.h> // open function
#include <stdlib.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define OK_IMAGE "HTTP/1.0 200 OK\nContent-Type:image/gif\n\n"
#define BUF_SIZE 1024

#define PATH "/home/gold/Documents/homework/" //path of html file

void *connection(void *p);
void *picture(int *connfd_thread);
void *text(int *connfd_thread);
void *produce_filename(int *connfd_thread);
char *token;
int size;
char *tempp;
int v;
int thread_count = 0;
pthread_t threads[10];
int connfd[10];
int i;
char file_name[100];
int img_file_descriptor;

char *ex;



int main(int argc, char *argv[])
{
    int socket_desc, new_socket, c;
    struct sockaddr_in server, client;
    char *message;

    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc == -1)
    {
        puts("Could not create socket");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888); // server will listen to 8888 port

    // Bind
    if (bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        puts("Binding failed");
        return 1;
    }
    puts("Socket is binded");

    listen(socket_desc, 3);

    puts("Waiting for incoming connections...");

    c = sizeof(struct sockaddr_in);

	while(new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t *)&c)){ 

	   if (new_socket < 0)
    {
        puts("Accept failed");
        return 1;
    }



pthread_create(&threads[thread_count], NULL, connection, &new_socket);	
	pthread_join(threads[thread_count], NULL);	//join the finished thread and continue



thread_count++;
printf(" tc:%d \n",thread_count);
}



    return 0;
}

void *connection(void *p)
{

    FILE *File;
    char send_buf_temp[1024];
    char *send_buf;
   
    char *file_data;
    
   
    int recv_len;
    int file_size;

    file_size = 0;

    
    int *connfd_thread = (int *)p;  //type cast the argument for use in this thread

    produce_filename(connfd_thread);
    printf("extension:%s",ex);

    if(strcmp(ex, "jpeg") == 0){

     picture(connfd_thread);

   }
    else if(strcmp(ex, "html") == 0){  

    text(connfd_thread);

    }

else {
	printf("\n Another extension");
}
 


}


void *produce_filename(int *connfd_thread){


    char recv_buf[1024];
    char *http_method;
    char *url;
    char *request;
    char *name;

    int temp;

    temp = (recv(*connfd_thread, recv_buf, sizeof(recv_buf), 0));

    http_method = strtok(recv_buf, " ");
    url = strtok(NULL, " ");

    request = strtok(NULL, " ");
    name = strtok(url, "/");
    strcpy(file_name, name);

ex=strtok(name,".");
ex=strtok(NULL,".");



}


void *text(int *connfd_thread){

FILE *File;
    char send_buf_temp[1024];
    char *send_buf;
    char *file_data;
    int file_size;


    File = fopen(file_name, "r");
    if (File == NULL)
    { 
        strcpy(send_buf, "HTTP/1.1 404 Not Found\n\n");
        send(*connfd_thread, send_buf, 24, 0);
    }
    else
    {
        fseek(File, 0, SEEK_END);               //seek to the end of the tfile
        file_size = ftell(File);                //get the byte offset of the pointer(the size of the file)
        fseek(File, 0, SEEK_SET);               //seek back to the begining of the file
        file_data = (char *)malloc(file_size + 1);   //allocate memmory for the file data
        fread(file_data, 1, (file_size), File); //read the file data into a string
        file_data[file_size] = '\0';

        sprintf(send_buf_temp, "HTTP/1.1 200 OK\nContent Length: %d\nConnection: close\nContent-Type: text/html\n\n%s\n\n", file_size, file_data); //format and create string for output to client

        int a = strlen(send_buf_temp);                            //
        send_buf = (char *)malloc(a);                             //allocate space for the reply
        strcpy(send_buf, send_buf_temp);                          //copy to the correctly sized string
        send(*connfd_thread, send_buf, a, 0);                    //send the info to the client

    }

  
    fclose(File);                                               //close the html file
 

    close(*connfd_thread); //close the current connection
    fflush(stdout);        //make sure all output is printed


}

void *picture(int *connfd_thread){

    char *send_buf2;
    unsigned int fd1;
    fd1 =  open(file_name, O_RDONLY);


 if (fd1 == -1)
    { 

        strcpy(send_buf2, "HTTP/1.1 404 Not Found\n\n");
        send(*connfd_thread, send_buf2, 24, 0);
    }


else{

    	unsigned int fd1; 
fd1 =  open(file_name, O_RDONLY);
int buffer_length;
char out_buf[BUF_SIZE];
memset(out_buf, 0, sizeof(out_buf));
strcpy(out_buf, OK_IMAGE);
send(*connfd_thread, out_buf, strlen(out_buf), 0);

buffer_length = 1;  
while (buffer_length > 0) { 

    buffer_length = read(fd1, out_buf, BUF_SIZE);

    if (buffer_length > 0)  {
        send(*connfd_thread, out_buf, buffer_length, 0);

    }
}


}
    close(*connfd_thread); //close the current connection
}