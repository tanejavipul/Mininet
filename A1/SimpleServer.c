#include "Helper.c"


int main( int argc, char *argv[] )  {
    //Get Arguments
    int port_number = atoi(argv[1]);
    char *root_address = argv[2];
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


    int server, new_socket;
    struct sockaddr_in serverAddress;
    struct Request request;

    if ((server = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(-1);
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
    printf("--------------REQUESTS--------------\n");

    if ((new_socket = accept(server, NULL, NULL))<0) //int accept(int socket, struct sockaddr *restrict address, socklen_t*restrict address_len);
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    char buffer[30000];
    read( new_socket , buffer, 30000);
    close(new_socket);
        printf("testing\n");
    get_header(&request, buffer);

}



    /*
     * Questions:
     * - Which headers to use, and which conditional headers should we add
     * - how should we send a get request? - curl? / do we need to write up client file (socket, connect send request etc.)
     * - for persistant do we need to make a c client so the connection doesnt close
     * - do we need thread for simple server as piazza post said we need to handle concurrent requests
     * - If we have HTTP/1.1 do we downgrade to 1.0 and vice versa (1.0 upgrade to 1.1)?
     *
     *
     *
     *
     */
