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

void *thread_handler(void *socket_desc) {

    int sock = *(int *) socket_desc;
    free(socket_desc);

    //struct for timeout
    struct timeval timeout;
    timeout.tv_sec = 3;
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
    int get_header_output;
    char buffer[30000];
    while (read(sock, buffer, 30000) > 0) {
        printf("hello\n");
        get_header_output = get_header(&header, buffer);

        if (get_header_output > 0) {

            printf("hello |%d|\n",get_header_output);

//            if (root_address[strlen(root_address) - 1] == '/') {
//                root_address[strlen(root_address) - 1] = '\0';
//            }
            printf("hell03 |%d|\n",get_header_output);

            handler(sock, &header, root_address);

        }
        else {
            printf("in else\n");

            char *error_response = (char *) malloc((strlen(RESPONSE_404_0)) * sizeof(char));
            if (header.http_version == 0) {
                strcpy(error_response, RESPONSE_404_0);
            } else {
                strcpy(error_response, RESPONSE_404_1);
            }
            write(sock, error_response,strlen(RESPONSE_404_0));
            free(error_response);
        }
        printf("free\n");

        free_memory(&header);

        printf("before free\n");


    }
    free_memory(&header);
    sleep(1);
    //close(sock);
    pthread_exit(NULL);
    }





int main(int argc, char *argv[]) {
    //Get Arguments
    if (argc != 3) {
        fprintf(stderr, "Invalid Number of Arguments!\n");
        return -1;
    }
    int port_number = atoi(argv[1]);
    char *root_address = argv[2];
    printf("Port Number:  %d\n", port_number);
    printf("Root Address: %s\n", root_address);


    if (access(root_address, F_OK) != 0) {
        fprintf(stderr, "http root path invalid with Error Code: %d\n", access(root_address, F_OK));
        return -1;
    }

    //take out the last / if it exists because we add it in request filename.
    if (root_address[strlen(root_address) - 1] == '/') {
        root_address[strlen(root_address) - 1] = '\0';
    }

    int server, client_socket;
    struct sockaddr_in serverAddress;

    if ((server = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(-1);
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
    printf("------------------- Pipeline Server -------------------\n");

    pthread_t thread;
    int *send_sock;
    while (1) { //while loop so it can process more connections that come in
        //int accept(int socket, struct sockaddr *restrict address, socklen_t*restrict address_len);
        if ((client_socket = accept(server, NULL, NULL)) < 0) {
            perror("ACCEPT FAILED");
            exit(EXIT_FAILURE);
        }
        send_sock = malloc(1);
        *send_sock = client_socket;

        if (pthread_create(&thread, NULL, thread_handler, (void *) send_sock) != 0) {
            printf("Failed to create thread\n");
        }
        pthread_detach(thread);

    }
}

