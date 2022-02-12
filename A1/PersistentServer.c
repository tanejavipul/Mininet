#include "ServerHelper.c"

int main( int argc, char *argv[] )  {
    //Get Arguments
    int port_number = atoi(argv[1]);
    char *root_address = argv[2];
    //printf("Port Number:  %d\n", port_number);
    //printf("Root Address: %s\n", root_address);

    if (argc != 3) {
        fprintf(stderr, "Invalid Number of Arguments!\n");
        return -1;
    }

    if(access(root_address, F_OK) != 0) {
        fprintf(stderr, "http root path invalid with Error Code: %d\n", access(root_address, F_OK));
        return -1;
    }


    int server, new_socket;
    struct sockaddr_in serverAddress;
    struct Header header;

    //header.http_version = 1; //TODO: fix, cant hardcode for Persistent server, need to parse header for version
    header.accept = NULL;
    header.filename = NULL;
    header.filetype = NULL;
    header.type = NULL;
    header.if_modified_since = NULL;
    header.if_unmodified_since = NULL;
    header.connectiontype = NULL;

    if ((server = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(-1);
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port_number); //port number provided
    serverAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK); //INADDR_LOOPBACK is local host

    bind(server, (struct sockaddr *) &serverAddress, sizeof(serverAddress));

    int get_header_output;
    int listening = listen(server, 10);
    if (listening < 0)
    {
        printf("Error: The server is not listening.\n");
        return 1;
    }
    //printf("listening output: %d\n", listening);
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;

    //set the timeout on client socket

    while(1) { //while loop so it can listen to more connections
        printf("--------------REQUESTS--------------\n");

        if ((new_socket = accept(server, NULL, NULL)) <0)
        {
            perror("ACCEPT FAILED");
            exit(EXIT_FAILURE);
        }

        char buffer[30000];
        setsockopt(new_socket, SOL_SOCKET, SO_RCVTIMEO, (const char *) &timeout, sizeof timeout);

        while(read(new_socket, buffer, 30000) > 0) {

            get_header_output = get_header(&header, buffer);

            if (get_header_output > 0) {

                //take out the last / if it exists because we add it in request filename.
                if (root_address[strlen(root_address) - 1] == '/') {
                    root_address[strlen(root_address) - 1] = '\0';
                }
                //printf("root_address: %s\n", root_address);
                handler(new_socket, &header, root_address);
            }
            else {
                char *error_response = (char *) malloc((strlen(RESPONSE_404_0)) * sizeof(char));
                if (header.http_version == 0) {
                    strcpy(error_response, RESPONSE_404_0);
                } else {
                    strcpy(error_response, RESPONSE_404_1);
                }
                write(new_socket, error_response,strlen(RESPONSE_404_0));
                free(error_response);
            }

            if( contains(header.connectiontype, TYPE_CLOSE) == 0 ) { //socket only closes when 1. socket times out or 2. client sends Connection: close header inside a header"
                free_memory(&header);
                break;
            }

            free_memory(&header);
        }
        //printf("CONNECTION CLOSED\n");
        close(new_socket);
    }
}

