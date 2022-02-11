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
int main( int argc, char *argv[] )  {
    //Get Arguments
    int port_number = atoi(argv[1]);
    char *root_address = argv[2];

 //   printf("Port Number:  %d\n", port_number);
   // printf("Root Address: %s\n", root_address);

    if (argc != 3) {
      //  fprintf(stderr, "Invalid Number of Arguments!\n");
        return -1;
    }

    if(access(root_address, F_OK) != 0) {
     //   fprintf(stderr, "http root path invalid with Error Code: %d\n", access(root_address, F_OK));
        return -1;
    }


    int server, new_socket, header_output;
    struct sockaddr_in serverAddress;
    struct Header header;
    header.accept = NULL;
    header.filename = NULL;
    header.filetype = NULL;
    header.type = NULL;
    header.if_modified_since = NULL;
    header.if_unmodified_since = NULL;
    header.connectiontype = NULL;

    header.http_version = 0;

    if ((server = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        //fprintf(stderr, "socket failed");
        return -1;
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port_number); //port number provided
    serverAddress.sin_addr.s_addr = htonl(INADDR_LOOPBACK); //INADDR_LOOPBACK is local host

    bind(server, (struct sockaddr *) &serverAddress, sizeof(serverAddress));

    int listening = listen(server, 10);
    if (listening < 0)
    {
       // printf("Error: The server is not listening.\n");
        return 1;
    }
  //  printf("listening output: %d\n", listening);

    while(1) { //while loop so it can process more requests that come in
        printf("--------------REQUESTS--------------\n");
        //int accept(int socket, struct sockaddr *restrict address, socklen_t*restrict address_len);
        if ((new_socket = accept(server, NULL, NULL)) <0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        char buffer[30000]; //JUST TO BE SAFE IN CASE
        int input_val = read(new_socket, buffer, 30000);
   //     printf("READ READ %d BYTES\n", input_val);

        if (input_val != 0) {

            buffer[input_val] = '\0';

            header_output = get_header(&header, buffer);
            //HANDLE FOR -1
            if (header_output > 0) {
                //take out the last / if it exists because we add it in request filename.
                if (root_address[strlen(root_address) - 1] == '/') {
                    root_address[strlen(root_address) - 1] = '\0';
                }
            //    printf("root_address: %s\n", root_address);
                handler(new_socket, &header, root_address);
            } else {
                write(new_socket,
                      "HTTP/1.0 404 NOT FOUND\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>404 Not Modified</body></html>",
                      strlen("HTTP/1.0 404 NOT FOUND\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>404 Not Modified</body></html>"));
            }
        }
        close(new_socket);
        free_memory(&header);

    }

}



    /*
     * Questions:
     * - Which headers to use, and which conditional headers should we add
     * - how should we send a get request? - curl? / do we need to write up client file (socket, connect send request etc.)
     * - for persistant do we need to make a c client so the connection doesnt close
     * - do we need thread for simple server as piazza post said we need to handle concurrent requests
     * - If we have HTTP/1.1 do we downgrade to 1.0 and vice versa (1.0 upgrade to 1.1)?
     *
     */
