#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <unistd.h>

#include <string.h>

//Request
char *END_OF_HEADER = "\r\n\r\n";
char *END_OF_LINE = "\r\n";
char *GET = "GET";
char *HOST = "Host:";
char *USER_AGENT = "User-Agent:";
char *ACCEPT = "Accept:";
char *WHITE_SPACE = " ";

//Response
char *OK = "200";
char *NOT_FOUND = "404";


struct Request {
    char* get;
    char* accept;
};

char* find_str_pointer(char* big, char* small) {
    char* x = strstr(big,small);
    return x;
}

int find_str_index(char* big, char* small) {
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
//
//
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

            printf("inside full filename: %s     %lu\n", extract_token, strlen(extract_token));
            printf("GETTING GET %p\n", find_str_pointer(extract_token, GET));
        }

//        //More Headers
//        if(){
//
//        }


        printf(" %s \n", token);
        token = strtok_r(NULL, END_OF_LINE, &main_strtok_pointer);
    }
    free(extract_token);
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