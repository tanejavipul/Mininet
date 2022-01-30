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
char *IMAGE_ANY = "image/*";
char *TEXT_ANY = "text/*";


char *DAYS_OF_WEEK[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
char *MONTH[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul","Aug", "Sep", "Oct", "Nov", "Dec" };


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
int contains(char* string, char* word) {
    int string_len = strlen(string);
    int word_len = strlen(word);
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
    //char *last_modify = last_modified(full_path, req);

    int total = strlen(status_r) + strlen(date) + strlen(MIME)  + strlen(content_t) + strlen(content_len);
    //total += strlen(last_modify)
    char *output = malloc(sizeof(char)*total + 1000);

    // do the copy and concat
    strcpy(output, status_r);
    strcat(output,date); // or strncat
    //do we need server?
    strcat(output,MIME);
    //strcat(output,last_modify);
    strcat(output,content_t);
    strcat(output,content_len);
    strcat(output,END_OF_LINE); //not using END_OF_HEADER because last header may already have /r/n at the end

    if(req->http_version == 1)
    {
        //add keep alive
    }

    free(status_r);
    free(date);
    free(content_t);
    free(content_len);
    //Can't free this for some reason
    //free(last_modify);
    printf("COMPILED RESPONSE:\n%s", output);
    return output;
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




