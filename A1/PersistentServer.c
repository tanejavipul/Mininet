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

char client_message[2000];
char buffer[1024];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
struct Header header;
char* root_address;

//void * socketThread(void *arg)
//{
//    int new_socket = *((int *)arg);
//    char buffer[30000];
//    int n = read(new_socket, buffer, 30000);
//    if (n <= 0) { printf("read() ends\n"); pthread_exit(NULL); } //might exit with return value instead of null to indicate error
//
//    //pthread_mutex_lock(&lock);
//    while(1) {
//
//        get_header(&header, buffer);
//
//        printf("root_address: %s\n", root_address);
//        handler(new_socket, &header, root_address);
//
//        if( strcmp(header.connectiontype, TYPE_CLOSE) == 0 ) { //socket only closes when 1. socket times out or 2. client sends Connection: close header inside a header"
//            free_memory(&header);
//            break;
//        }
//
//        free_memory(&header);
//    }
//    //pthread_mutex_unlock(&lock);
//    sleep(1);
//
//    printf("CONNECTION CLOSED\n");
//    close(new_socket);
//    pthread_exit(NULL);
//
//}

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

    int server, new_socket;
    struct sockaddr_in serverAddress;


    header.http_version = 1; //TODO: fix, cant hardcode for Persistent server, need to parse header for version
    header.connectiontype = TYPE_KEEPALIVE; //REMOVE AFTER YOU POPULATE THIS VALUE IN ServerHelper.c

    if ((server = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(-1);
    }

    // This is to lose the pesky "Address already in use" error message
//    if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
//                   &opt, sizeof(opt))) // SOL_SOCKET is the socket layer itself
//    {
//        perror("setsockopt");
//        exit(EXIT_FAILURE);
//    }

    //Should be working, sets timeout on socket so the connection closes after a set time
    struct timeval timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    if (setsockopt (server, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                    sizeof timeout) < 0)
        perror("setsockopt failed\n");

    if (setsockopt (server, SOL_SOCKET, SO_SNDTIMEO, &timeout,
                    sizeof timeout) < 0)
        perror("setsockopt failed\n");


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

    while(1) { //while loop so it can process more requests that come in
        printf("--------------REQUESTS--------------\n");
        //int accept(int socket, struct sockaddr *restrict address, socklen_t*restrict address_len);
        if ((new_socket = accept(server, NULL, NULL)) <0)
        {
            perror("ACCEPT FAILED");
            exit(EXIT_FAILURE);
        }
        //======================================================================================
        //for each client request creates a thread and assign the client request to it to process
        //so the main thread can entertain next request
//        if( pthread_create(&tid[i++], NULL, socketThread, &newSocket) != 0 )
//            printf("Failed to create thread\n");
//
//        if( i >= 50)
//        {
//            i = 0;
//            while(i < 50)
//            {
//                pthread_join(tid[i++],NULL);
//            }
//            i = 0;
//        }
        //=============================================================
        char buffer[30000];


        while(1) {
            int n = read(new_socket, buffer, 30000);
            if (n <= 0) { printf("read() ends\n"); break; }

            get_header(&header, buffer);
            handler(new_socket, &header, root_address);

            if( strcmp(header.connectiontype, TYPE_CLOSE) == 0 ) { //socket only closes when 1. socket times out or 2. client sends Connection: close header inside a header"
                free_memory(&header);
                break;
            }

            free_memory(&header);
        }
        printf("CONNECTION CLOSED\n");
        close(new_socket);

    }
    return 0;

}

