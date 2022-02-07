#include "ServerHelper.c"

/*
HTTP/1.0 200 OK
Date: Fri, 08 Aug 2003 08:12:31 GMT
Server: Apache/1.3.27 (Unix)
MIME-version: 1.0
Last-Modified: Fri, 01 Aug 2003 12:45:26 GMT
Content-Type: text/html
Content-Length: 2345
** a blank line *
<HTML> ...
 */

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
struct Header header;
char* root_address;

void * socketThread(void *arg)
{
    int client_socket = *((int *)arg);
    char buffer[BUFFER_SIZE];

    //struct for timeout
    struct timeval timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    //set the timeout on client socket
    setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof timeout);

    //handle requests
    while(read(client_socket, buffer, BUFFER_SIZE) > 0) { //need to break out of this loop if we get an error

        get_header(&header, buffer);

        printf("root_address: %s\n", root_address);
        handler(client_socket, &header, root_address);

        if( strcmp(header.connectiontype, TYPE_CLOSE) == 0 ) {
            free_memory(&header);
            break;
        }

        free_memory(&header);
    }
    sleep(1);
    printf("CLIENT CONNECTION CLOSED\n");
    close(client_socket);
    printf("EXITING THREAD\n");
    pthread_exit(NULL);

}

int main( int argc, char *argv[] )  {
    //Get Arguments
    int port_number = atoi(argv[1]);
    root_address = argv[2];
    printf("Port Number:  %d\n", port_number);
    printf("Root Address: %s\n", root_address);

    if (argc != 3) {
        fprintf(stderr, "Invalid Number of Arguments!\n");
        return -1;
    }

    if(access(root_address, F_OK) != 0) {
        fprintf(stderr, "http root path invalid with Error Code: %d\n", access(root_address, F_OK));
        return -1;
    }

    //take out the last / if it exists because we add it in request filename.
    if (root_address[strlen(root_address) - 1] == '/') {
        root_address[strlen(root_address) - 1] = '\0';
    }
    printf("root_address: %s\n", root_address);

    int server, client_socket, opt;
    struct sockaddr_in serverAddress;

    if ((server = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(-1);
    }

    // This is to lose the pesky "Address already in use" error message
    if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                   &opt, sizeof(opt))) // SOL_SOCKET is the socket layer itself
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port_number); //port number provided
    serverAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK); //INADDR_LOOPBACK is local host

    bind(server, (struct sockaddr *) &serverAddress, sizeof(serverAddress));

    int listening = listen(server, 10);
    if (listening < 0)
    {
        printf("Error: The server is not listening.\n");
        return 1;
    }
    printf("listening output: %d\n", listening);

    pthread_t tid[60];
    int i = 0;
    while(1) { //while loop so it can process more connections that come in
        printf("--------------REQUESTS--------------\n");
        //int accept(int socket, struct sockaddr *restrict address, socklen_t*restrict address_len);
        if ((client_socket = accept(server, NULL, NULL)) <0)
        {
            perror("ACCEPT FAILED");
            exit(EXIT_FAILURE);
        }
        //=================https://dzone.com/articles/parallel-tcpip-socket-server-with-multi-threading========================================
        //for each client request creates a thread and assign the client request to it to process
        //so the main thread can entertain next request
        if( pthread_create(&tid[i++], NULL, socketThread, &client_socket) != 0 )
            printf("Failed to create thread\n");

        if( i >= 50)
        {
            i = 0;
            while(i < 50)
            {
                pthread_join(tid[i++],NULL);
            }
            i = 0;
        }
        //=============================================================
    }
    return 0;

}

