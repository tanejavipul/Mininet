#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>

//Request
char *END_OF_HEADER = "\r\n\r\n";
char *END_OF_LINE = "\r\n";
char *GET = "GET";
char *HOST = "Host:";
char *USER_AGENT = "User-Agent:";
char *ACCEPT = "Accept: ";
char *WHITE_SPACE = " ";
char *ACCEPT_EMPTY = "*/*";

//Response
char *OK = "200 OK";
char *NOT_FOUND = "404 NOT FOUND";
char *BAD_REQUEST = "400 BAD_REQUEST";
const char *VERSION_NOT_SUPPORTED = "505 HTTP VERSION NOT SUPPORTED";


struct Request {
    char* filename;
    char* accept;
};


struct Response {
    //Example Response
    //HTTP/1.1 200 OK\r\n
    //Date: Sun, 26 Sep 2010 20:09:20 GMT\r\n
    //Server: Apache/2.0.52 (CentOS)\r\n
    //Last-Modified: Tue, 30 Oct 2007 17:00:02
    //    GMT\r\n
    //ETag: "17dc6-a5c-bf716880"\r\n
    //Accept-Ranges: bytes\r\n
    //Content-Length: 2652\r\n
    //Keep-Alive: timeout=10, max=100\r\n
    //Connection: Keep-Alive\r\n
    //Content-Type: text/html;
    //    charset=ISO-8859-1\r\n
    //\r\n
    //data of file you are returning
    char* version;
    char* response;
//    char* data;
};

char* find_str_pointer(char* big, char* small) { //NULL is returned if not found
    char* x = strstr(big,small);
    return x;
}

int find_str_index(char* big, char* small) { //problems
    char* x = strstr(big,small);
    return x-big;
}

char* get_root_filename_path(char* root_path, char* filename){
    int rootlen = strlen(root_path);
    int filelen = strlen(filename);
    char *output = malloc( (rootlen+filelen+1) * sizeof(char));
    strcpy(output, root_path);
    strcat(output,filename);
    return output;
}


//char* get_filename(char* get_request){
//    char output[1000];
//    int x=4;
//    while (x<strlen(get_request)) {
//    }
//}


//    get_header(&request, buffer);
void get_header(struct Request *req, char* header) {
    printf("INSIDER GET_HEADER\n");

    char *main_strtok_pointer = NULL;
    char *extract_token = malloc(sizeof(char)*3000);

    char *token = strtok_r(header, END_OF_LINE, &main_strtok_pointer); //DO NOT USE strtok


    while(token != NULL) {
        strcpy(extract_token,token);

        //PULL FILENAME
        if(find_str_pointer(extract_token, GET)!=NULL)
        {
            req->filename = malloc(sizeof(char)*(strlen(extract_token)));
            char *get_strtok_pointer = NULL;
            char *get_token = strtok_r(extract_token, " ", &get_strtok_pointer);
            get_token = strtok_r(NULL, WHITE_SPACE, &get_strtok_pointer);
            strcpy(req->filename, get_token);
        }

        if(find_str_pointer(extract_token, ACCEPT)!=NULL)
        {
            //check if accept field is not empty
            if(find_str_pointer(extract_token, ACCEPT_EMPTY)!=NULL) { //TODO NEED TO FIX AND TEST
                //req->filename = malloc(sizeof(char)*(strlen(extract_token)));
                char *accept_strtok_pointer = NULL;
                char *accept_token = strtok_r(extract_token, " ", &accept_strtok_pointer);
                //accept_token = strtok_r(NULL, ACCEPT, &accept_strtok_pointer);
                strcpy(req->accept, accept_token);
            }
        }

        printf(" %s \n", token);
        token = strtok_r(NULL, END_OF_LINE, &main_strtok_pointer);
    }
    free(extract_token);
}

void generate_response(struct Response *r) {
    //Status Line (1)

    char * status_line = (char *) malloc(1 + 1 + 2 + strlen(r->version)+ strlen(r->response) ); //space, /r/n, and string terminator??

    sprintf(status_line, "%s %s", r->version, r->response);
    strcat(status_line, END_OF_LINE);
    printf("Contents of structure are %s\n", r->version);
    printf("Contents of structure are %s\n", r->response);

    printf("Contents of structure are %s\n", status_line);

    free(status_line);

}










/*
    if ((new_socket = accept(server, NULL, NULL))<0) //int accept(int socket, struct sockaddr *restrict address, socklen_t*restrict address_len);
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    //char *end_str = "\r\n\r\n";
    char buffer[30000] = {0};
    read( new_socket , buffer, 30000);
    printf("%s\n",buffer );
    printf("%lu\n", strlen(buffer));
    close(new_socket);
    printf("find point %p\n", find_str_pointer(buffer,END_OF_HEADER));
    printf("test %p\n",strstr(buffer,END_OF_HEADER));
    char* x = strstr(buffer,END_OF_HEADER)-(sizeof(char)*2);
    printf("testing %d\n", find_str_index(buffer,END_OF_HEADER));



 */