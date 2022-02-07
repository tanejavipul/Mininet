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

struct Header header;
char *root_address;
//might change it to remove global variables later

//int pipeline_handler(int client_socket, char *buffer) {
//
//    printf("inside PIPELIEN HaNDLE\n");
//    char *header_copy = (char *)malloc(sizeof (char )*10000);
//    printf(" header_cpy\n");
////    header.accept = NULL;
////    header.filename = NULL;
////    header.filetype = NULL;
////    header.type = NULL;
////    header.if_modified_since = NULL;
////    header.connectiontype = NULL;
//
//    int connection_check = 1;
//    printf("inside PIPELIEN HaNDLE\n");
//
//    strcpy(header_copy, buffer);
//    printf("inside PIPELIEN HaNDLE\n");
//
//    printf("PROCESS_PIPELINED_REQUEST: header_copy: %s\n", header_copy);
//
//    //====================more work needed if get_header and/or handler returns a value for connection_check===========================
//    get_header(&header, header_copy);
//
//    printf("root_address: %s\n", root_address);
//    printf("going inside handler\n");
//
//    handler(client_socket, &header, root_address);
//    printf("outside handler\n");
//
//    if (strcmp(header.connectiontype, TYPE_CLOSE) == 0) {
//        connection_check = -1; //will result in closing thread
//    }
//
////       free_memory(&header); will cause double free error
//    //======================================================================
//    free(header_copy);
//
//    return connection_check;
//}

void *request_handler(void *socket_desc) {
    printf("inside handler\n");
//    char buffer[BUFFER_SIZE];

    int sock = *(int*)socket_desc;
    //struct for timeout
    struct timeval timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    //set the timeout on client socket
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *) &timeout, sizeof timeout);

    struct Header header;
    header.accept = NULL;
    header.filename = NULL;
    header.filetype = NULL;
    header.type = NULL;
    header.if_modified_since = NULL;
    header.if_unmodified_since = NULL;
    header.connectiontype = NULL;
    header.http_version = 1;
    int x = 1;
    char buffer[30000];
    while(x != 0) {
        x = read(sock, buffer, 30000);
        if (x !=-1){


            printf("return read |%d|\n",x);

        int w = get_header(&header, buffer);

        printf("return header: |%d|",w);
        //take out the last / if it exists because we add it in request filename.
        if (root_address[strlen(root_address) - 1] == '/') {
            root_address[strlen(root_address) - 1] = '\0';
        }

        printf("root_address: |%s|\n", root_address);
        printf("|%d| filename: |%s|", sock, header.filename);
        handler(sock, &header, root_address);
        free_memory(&header);
        }
//
    }
//    free_memory(&header);
    sleep(1);
    printf("CLIENT CONNECTION CLOSED\n");
   // close(sock); //TODO
    printf("EXITING THREAD\n");
    pthread_exit(NULL);

}


int main(int argc, char *argv[]) {
    //Get Arguments
    int port_number = atoi(argv[1]);
    root_address = argv[2];
    printf("Port Number:  %d\n", port_number);
    printf("Root Address: %s\n", root_address);

    if (argc != 3) {
        fprintf(stderr, "Invalid Number of Arguments!\n");
        return -1;
    }

    if (access(root_address, F_OK) != 0) {
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

    if ((server = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(-1);
    }

    // This is to lose the pesky "Address already in use" error message
    if (setsockopt(server, SOL_SOCKET, SO_KEEPALIVE,
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
    if (listening < 0) {
        printf("Error: The server is not listening.\n");
        return 1;
    }
    printf("listening output: %d\n", listening);

    pthread_t thread;
    int *send_sock;
    while (1) { //while loop so it can process more connections that come in
        printf("--------------REQUESTS--------------\n");
        //int accept(int socket, struct sockaddr *restrict address, socklen_t*restrict address_len);
        if ((client_socket = accept(server, NULL, NULL)) < 0) {
            perror("ACCEPT FAILED");
            exit(EXIT_FAILURE);
        }
        send_sock = malloc(1);
        *send_sock = client_socket;

        printf("NEW THREAD\n");
        if (pthread_create(&thread, NULL, request_handler, (void*)send_sock) != 0){
            printf("Failed to create thread\n");
        }

    }
    return 0;

}

