#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>


#define BUFFER_SIZE 1024

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
char *HTTP11 = "HTTP/1.1";
char *HTTP10 = "HTTP/1.0";
char *OK = "200 OK";
char *NOT_FOUND = "404 NOT FOUND";
char *BAD_REQUEST = "400 BAD_REQUEST";
const char *VERSION_NOT_SUPPORTED = "505 HTTP VERSION NOT SUPPORTED";
char *SERVER = "SERVER: UTM_358_SERVER (BROKEN-UNFIXABLE)\r\n";
char *CONTENT_TYPE = "Content-Type:";
char *CONTENT_LENGTH = "Content-Length:";
char *DATE = "Data:";
char *MIME = "MIME-version: 1.0\r\n";

//FileType
char *JPG = "jpg";
char *HTML = "html";
char *CSS = "css";
char *TEXT = "text/";
char *IMAGE = "image/";

char *DAYS_OF_WEEK[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
char *MONTH[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul","Aug", "Sep", "Oct", "Nov", "Dec" };

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
struct Request {
    char* filename;
    char* accept;
    char* filetype;
    int http_version; // 0 = HTTP/1.0 or 1 = HTTP/1.1
};


/*
 * Returns 0 if word found in string else -1
 */
int contains(char* string, int string_len, char* word, int word_len) {
    int x = 0;
    int count = 0;
    while (x < string_len){
        if(string[x] == word[count]){
            count++;
        }
        else{
            count=0;
        }
        if(count==word_len){
            return 0;
        }
        x++;
    }
    if(count==word_len){
        return 0;
    }
    return -1;
}



//    get_header(&request, buffer);
void get_header(struct Request *req, char* header) {
    printf("INSIDE GET_HEADER: %s\n", header);

    char *main_strtok_pointer = NULL;
    char *extract_token = malloc(sizeof(char)*10000);

    char *token = strtok_r(header, END_OF_LINE, &main_strtok_pointer); //DO NOT USE strtok

    while(token != NULL) {
        strcpy(extract_token,token);

        //PULL FILENAME + FILE TYPE
        if(contains(extract_token, strlen(extract_token), GET, strlen(GET))==0)
        {
            //get filename
            req->filename = malloc(sizeof(char)*(strlen(extract_token)));
            char *get_strtok_pointer = NULL;
            char *get_token = strtok_r(extract_token, WHITE_SPACE, &get_strtok_pointer);
            get_token = strtok_r(NULL, WHITE_SPACE, &get_strtok_pointer);
            strcpy(req->filename, get_token);
            
            //get filetype
            req->filetype = malloc(sizeof(char)*(strlen(extract_token)));
            char *get_strtok_pointer2 = NULL;
            char *get_file_token = strtok_r(get_token, ".", &get_strtok_pointer2);
            get_file_token = strtok_r(NULL, ".", &get_strtok_pointer2);

            printf("get_token file name : %s\n", get_file_token);
            strcpy(req->filetype, get_file_token);
        }

        //PULL ACCEPT
        if(contains(extract_token, strlen(extract_token), ACCEPT, strlen(ACCEPT))==0)
        {
            //check if accept field is not empty
            //TODO NEED TO FIX AND TEST
            if(contains(extract_token, strlen(extract_token), ACCEPT_EMPTY, strlen(ACCEPT_EMPTY))!=0) {
                req->accept = malloc(sizeof(char)*(strlen(extract_token)));
                char *accept_strtok_pointer = NULL;
                char *accept_token = strtok_r(extract_token, " ", &accept_strtok_pointer);
                //accept_token = strtok_r(NULL, ACCEPT, &accept_strtok_pointer);
                strcpy(req->accept, accept_token);
                int x = 0;
                while (x< strlen(extract_token)){
                    printf("char:__%c__",extract_token[x+4]);
                    x++;
                }
            }
        }

        printf(" %s \n", token);
        token = strtok_r(NULL, END_OF_LINE, &main_strtok_pointer);
    }
    free(extract_token);
}



void css_jpg_write(int socket, char* full_path) {
    int fp;
    if ((fp=open(full_path, O_RDONLY)) > 0) //FILE FOUND
    {
        puts("CSS or JPG Found.");
        int bytes;
        char buffer[BUFFER_SIZE];

        send(socket, "HTTP/1.0 200 OK\r\nContent-Type: image/jpg\r\n\r\n", 45, 0);
        while ( (bytes=read(fp, buffer, BUFFER_SIZE))>0 ) // Read the file to buffer. If not the end of the file, then continue reading the file
            write (socket, buffer, bytes); // Send the part of the jpeg to client.
        close(fp);
    }
}


/*
 *
 */
void handler(int socket, struct Request *req, char* root_address) {
    char* file_name = req->filename;
    char *full_path = (char *)malloc((strlen(root_address) + strlen(file_name)) * sizeof(char));

    strcpy(full_path, root_address); // Merge the file name that requested and path of the root folder
    strcat(full_path, file_name);


    if(strcmp(req->filetype, HTML) == 0) {
        FILE *fp;
        fp = fopen(full_path, "r");
        if (fp != NULL) //FILE FOUND
        {
            char *buffer;
            puts("File Found.");

            fseek(fp, 0, SEEK_END); // Find the file size.
            long bytes_read = ftell(fp);
            fseek(fp, 0, SEEK_SET);

            send(socket, "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n", 44, 0); // Send the header for succesful respond.
            buffer = (char *)malloc(bytes_read * sizeof(char));

            fread(buffer, bytes_read, 1, fp); // Read the html file to buffer.
            write (socket, buffer, bytes_read); // Send the content of the html file to client.

            free(buffer);
            fclose(fp);
        }
    }
    else if(strcmp(req->filetype, JPG) == 0) {
        int fp;
        if ((fp=open(full_path, O_RDONLY)) > 0) //FILE FOUND
        {
            puts("Image Found.");
            int bytes;
            char buffer[BUFFER_SIZE];

            send(socket, "HTTP/1.0 200 OK\r\nContent-Type: image/jpeg\r\n\r\n", 45, 0);
            while ( (bytes=read(fp, buffer, BUFFER_SIZE))>0 ) // Read the file to buffer. If not the end of the file, then continue reading the file
                write (socket, buffer, bytes); // Send the part of the jpeg to client.
            close(fp);
        }

    }
    else if(strcmp(req->filetype, CSS) == 0)
    {
        int fp;
        if ((fp=open(full_path, O_RDONLY)) > 0) //FILE FOUND
        {
            puts("CSS Found.");
            int bytes;
            char buffer[BUFFER_SIZE];

            send(socket, "HTTP/1.0 200 OK\r\nContent-Type: text/css\r\n\r\n", 45, 0);
            while ( (bytes=read(fp, buffer, BUFFER_SIZE))>0 ) // Read the file to buffer. If not the end of the file, then continue reading the file
                write (socket, buffer, bytes); // Send the part of the jpeg to client.
            close(fp);
        }

    }
    else // If there is not such a file.
    {
        write(socket, "HTTP/1.0 404 Not Found\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>404 File Not Found</body></html>", strlen("HTTP/1.0 404 Not Found\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>404 File Not Found</body></html>"));
    }

    free(full_path);
}




char *status_response( struct Request *req, char *status){
    // HTTP/1.0 200 OK
    int x = strlen(HTTP10) + strlen(req->filetype) + strlen(TEXT);
    x += strlen(IMAGE) + strlen(status) + strlen(END_OF_LINE) + 100;

    char *output = (char *)malloc(x * sizeof(char));
    if(req->http_version == 0){
        snprintf(output, x, "%s %s %s", HTTP10,status,END_OF_LINE); //HTTP 1.0
    }
    else{
        snprintf(output, x, "%s %s %s", HTTP11,status,END_OF_LINE); //HTTP 1.1

    }

    printf("STATUS_RESPONSE: %s",output);
    return output;
}

char *date_response() {
    //Date: Fri, 08 Aug 2003 08:12:31 GMT
    char *output = (char *)malloc(100 * sizeof(char));
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);

    //returns a int for response code
    snprintf(output, 100, "%s %s, %d %s %d %d:%d:%d %s\r\n", DATE, DAYS_OF_WEEK[tm->tm_wday],tm->tm_mday,MONTH[tm->tm_mon], tm->tm_year+1900,tm->tm_hour,tm->tm_min,tm->tm_sec,tm->tm_zone);

    printf("DATE_RESPONSE: %s",output);
    return output;
}

// TODO need to fix
char *content_type(struct Request *req) {
    char *output = (char *)malloc(100 * sizeof(char));

    snprintf(output, 100, "%s %s%s%s", CONTENT_TYPE,TEXT,req->filetype,END_OF_LINE);

    printf("STATUS_RESPONSE: %s",output);
    return output;
}

char *content_length(int length) {
    char *output = (char *)malloc(100 * sizeof(char));

    snprintf(output, 100, "%s %d\r\n", CONTENT_LENGTH,length);

    printf("STATUS_RESPONSE: %s",output);
    return output;

}

char *compile_response(struct Request *req, char *status, int length) {
    char *status_r = status_response( req, status);
    char *date = date_response();
    char *content_t = content_type(req);
    char *content_len = content_length(length);

    int total = strlen(status_r) + strlen(date) + strlen(content_t) + strlen(content_len);
    char *output = malloc(sizeof(char)*total + 1000);

    free(status_r);
    free(date);
    free(content_t);
    free(content_len);

    return output;
}












//Most likely delete later
//char* generate_response(struct Response *r) {
//    //Status Line (1)
//
//    char * status_line = (char *) malloc(1 + 1 + 2 + strlen(r->version)+ strlen(r->response) ); //space, /r/n, and string terminator??
//
//    sprintf(status_line, "%s %s", r->version, r->response);
//    strcat(status_line, END_OF_LINE);
//
//    printf("Contents of structure are %s\n", r->version);
//    printf("Contents of structure are %s\n", r->response);
//    printf("Contents of structure are %s\n", status_line);
//
//    //free(status_line);
//    return status_line;
//}


void free_memory(struct Request *req) {
    if (req->accept != NULL) {
        free(req->accept);
    }
    if (req->filename != NULL) {
        free(req->filename);
    }
}






    // DEPRECATED CODE
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












/*
    // SOME EXAMPLE USE OF CODE - TEMP
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
