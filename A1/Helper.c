#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

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
//Conditionals
char *IF_MATCH = "If-Match:";
char *IF_NONE_MATCH = "If-None-Match:";
char *IF_MODIFIED = "If-Modified-Since:";
char *IF_UNMODIFIED = "If-Unmodified-Since:";
char *IF_RANGE = "If-Range:";

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
char *DATE = "Date:";
char *MIME = "MIME-version: 1.0\r\n";
char *LAST_MODIFIED = "Last-Modified: ";

//FileType
char *JPG = "jpg";
char *HTML = "html";
char *CSS = "css";
char *PLAIN = "plain";
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
    char* type;
    int http_version; // 0 = HTTP/1.0 or 1 = HTTP/1.1
    char* if_modified_timestamp;
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
            //FIX IN CASE NO FILETYPE DONT THINK WE CAN STRTOK IT
            req->filetype = malloc(sizeof(char)*(strlen(extract_token)));
            char *get_strtok_pointer2 = NULL;
            char *get_file_token = strtok_r(get_token, ".", &get_strtok_pointer2);
            get_file_token = strtok_r(NULL, ".", &get_strtok_pointer2);

            printf("get_token file name : %s\n", get_file_token);
            strcpy(req->filetype, get_file_token);


            if(strcmp(get_file_token, JPG) == 0) {
                req->type = malloc(sizeof(char)*(strlen(IMAGE)));
                strcpy(req->type, IMAGE);
            } else { //filetype is html, css, txt or js, NOTE: might be an issue if we want to handle additional extensions, IDEA: send BAD REQUEST if not html css txt or js
                req->type = malloc(sizeof(char)*(strlen(TEXT)));
                strcpy(req->type, TEXT);
            }

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
        //PULL CONDITIONAL (Currently only if modified)
        if(contains(extract_token, strlen(extract_token), IF_MODIFIED, strlen(IF_MODIFIED))==0)
        {
            printf("extract token if modifeid: %s\n", extract_token);
            char *output = malloc(sizeof(char)*(strlen(extract_token)));
            sscanf(extract_token, "If-Modified-Since: %[^\\t\\n]", output);

            req->if_modified_timestamp = malloc(sizeof(char)*(strlen(output)));
            printf("if_modified_timestamp : %s\n", output);
            strcpy(req->if_modified_timestamp, output);

            struct tm *time;
            strftime(output, 100, "%a, %d %b %Y %X %Z", time);
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

char *last_modified( char *full_path, struct Request *req ) {
    //Fri, 08 Aug 2003 08:12:31 GMT
    char *output = (char *) malloc(100 * sizeof(char));
    char *last_modified_time = (char *) malloc(100 * sizeof(char));
    struct stat attr;
    stat(full_path, &attr);

    strftime(last_modified_time, 100, "%a, %d %b %Y %X %Z\r\n", gmtime(&attr.st_mtime));

    char *month_char = (char *) malloc(3 * sizeof(char));
    char *weekday = (char *) malloc(3 * sizeof(char));
    char *timezone = (char *) malloc(3 * sizeof(char));

    int year, day, hour, min, sec;
    sscanf(req->if_modified_timestamp, "%s %d %s %d %d:%d:%d %s", weekday, &day, month_char, &year, &hour, &min, &sec, timezone );
//    printf("month_char: %s, day: %d , year: %d, hour: %d , min: %d, sec: %d\n", month_char, day, year, hour, min, sec);

    int month = 0;
    for ( int i = 0; MONTH[i] != NULL; i++) {
        if (strcmp(MONTH[i], month_char) == 0) {
            month += 1;
            break;
        }
        month += 1;
    }

    struct tm timestamp;
    timestamp.tm_year = year - 1900;
    timestamp.tm_mon = month - 1;
    timestamp.tm_mday = day;
    timestamp.tm_hour = hour;
    timestamp.tm_min = min;
    timestamp.tm_sec = sec;

//    char buffer[26];
//    char buffer2[26];
//    strftime(buffer, 26, "%Y-%m-%d %H:%M:%S", &timestamp);
//    strftime(buffer2, 26, "%Y-%m-%d %H:%M:%S", gmtime(&attr.st_mtime));
//    printf("buffer: %s \nbuffer2: %s\n", buffer, buffer2);

    strcpy(output, LAST_MODIFIED);
    strcat(output, last_modified_time);

    //problem with reading timestamps TODO
    if ( &timestamp < gmtime(&attr.st_mtime) ) {
        printf("%s is older than %s\n", req->if_modified_timestamp, last_modified_time);
        return output;
    } else if ( &timestamp > gmtime(&attr.st_mtime) ) {
        printf("%s is newer than %s\n", req->if_modified_timestamp, last_modified_time);
        return NULL;
    }
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

    snprintf(output, 100, "%s %s%s%s", CONTENT_TYPE,req->type,req->filetype,END_OF_LINE);

    printf("CONTENT_TYPE_RESPONSE: %s",output);
    return output;
}

char *content_length(int length) {
    char *output = (char *)malloc(100 * sizeof(char));

    snprintf(output, 100, "%s %d\r\n", CONTENT_LENGTH,length);

    printf("CONTENT_LENGTH_RESPONSE: %s",output);
    return output;
}

char *compile_response(struct Request *req, char *status, int length, char *full_path) {
    char *status_r = status_response(req, status);
    char *date = date_response();
    char *content_t = content_type(req);
    char *content_len = content_length(length);
    char *last_modify = last_modified(full_path, req);

    int total = strlen(status_r) + strlen(date) + strlen(MIME) + strlen(last_modify) + strlen(content_t) + strlen(content_len);
    char *output = malloc(sizeof(char)*total + 1000);

    // do the copy and concat
    strcpy(output, status_r);
    strcat(output,date); // or strncat
    //do we need server?
    strcat(output,MIME);
    strcat(output,last_modify);
    strcat(output,content_t);
    strcat(output,content_len);
    strcat(output,END_OF_LINE); //not using END_OF_HEADER because last header may already have /r/n at the end

    free(status_r);
    free(date);
    free(content_t);
    free(content_len);
    //Can't free this for some reason
    //free(last_modify);
    printf("COMPILED RESPONSE:\n%s", output);
    return output;
}

/*
 *
 */
void handler(int socket, struct Request *req, char* root_address) {
    char* file_name = req->filename;
    char *full_path = (char *)malloc((strlen(root_address) + strlen(file_name)) * sizeof(char));

    strcpy(full_path, root_address); // Merge the file name that requested and path of the root folder
    strcat(full_path, file_name);

    //if modified since
    if (last_modified(full_path, req) == NULL) {
        //Write is not working sending bad response TODO
        write(socket,"HTTP/1.0 304\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>304</body></html>",
              strlen("HTTP/1.0 304\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>304</body></html>"));
        printf("wrote 304\n");
        free(full_path);
        return;
    }

    int fp;
    if ((fp=open(full_path, O_RDONLY)) > 0) //FILE FOUND
    {
        printf("%s Found\n", req->filetype);
        int bytes;
        char buffer[BUFFER_SIZE];

        //getting file size
        struct stat st;
        stat(full_path, &st);
        long file_size = st.st_size;

        char* response = compile_response(req, OK, file_size, full_path); //generate response
        send(socket, response, strlen(response), 0);

        while ( (bytes=read(fp, buffer, BUFFER_SIZE))>0 ) // Read the file to buffer. If not the end of the file, then continue reading the file
            write (socket, buffer, bytes); // Send the part of the jpeg to client.
        close(fp);
    }
    else {
        write(socket, "HTTP/1.0 404 NOT FOUND\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>404 File or File Extension not found</body></html>", strlen("HTTP/1.0 404 Not Found\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>404 File Not Found</body></html>"));
    }
    free(full_path);
}

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
