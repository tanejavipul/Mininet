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
#include <pthread.h>

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

//Request and Response
char* CONNECTION = "Connection:";
char* KEEP_ALIVE = "Keep-Alive: timeout="; //header to specify timeout time
char* TYPE_KEEPALIVE = "keep-alive";
char* TYPE_CLOSE = "close";

//FileType
char *JPG = "jpg";
char *JPEG = "jpeg";
char *HTML = "html";
char *JS = "js";
char *TXT = "txt";
char *CSS = "css";
char *PLAIN = "plain";
char *TEXT = "text/";
char *IMAGE = "image/";
char *IMAGE_ANY = "image/*";
char *TEXT_ANY = "text/*";


char *DAYS_OF_WEEK[7] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
char *MONTH[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul","Aug", "Sep", "Oct", "Nov", "Dec" };




struct Header {
    char* filename;
    char* accept;
    char* filetype;
    char* type;
    int http_version; // 0 = HTTP/1.0 or 1 = HTTP/1.1
    char* if_modified_since;
    char* if_unmodified_since;
    char* connectiontype;
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





char *status_response( struct Header *header, char *status){
    // HTTP/1.0 200 OK
    int x = strlen(HTTP10) + strlen(header->filetype) + strlen(TEXT);
    x += strlen(IMAGE) + strlen(status) + strlen(END_OF_LINE) + 100;

    char *output = (char *)malloc(x * sizeof(char));
    if(header->http_version == 0){
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
char *content_type(struct Header *header) {
    char *output = (char *)malloc(100 * sizeof(char));

    snprintf(output, 100, "%s %s%s%s", CONTENT_TYPE,header->type,header->filetype,END_OF_LINE);

    printf("CONTENT_TYPE_RESPONSE: %s",output);
    return output;
}

char *content_length(int length) {
    char *output = (char *)malloc(100 * sizeof(char));

    snprintf(output, 100, "%s %d\r\n", CONTENT_LENGTH,length);

    printf("CONTENT_LENGTH_RESPONSE: %s",output);
    return output;
}

char *connection_type(struct Header *header) {
    char *output = (char *)malloc(100 * sizeof(char));

    snprintf(output, 100, "%s %s%s", CONNECTION,header->connectiontype,END_OF_LINE);

    printf("CONNECTION_RESPONSE: %s",output);
    return output;

}

char *keepalive_time() {
    char *output = (char *)malloc(100 * sizeof(char));

    snprintf(output, 100, "%s%d%s", KEEP_ALIVE,300,END_OF_LINE);

    printf("KEEPALIVE_RESPONSE: %s",output);
    return output;

}

// returns last modified of file
char *last_modified_response(char *full_path) {
    //Fri, 08 Aug 2003 08:12:31 GMT
    char *output = (char *) malloc(100 * sizeof(char));
    char *last_modified_time = (char *) malloc(100 * sizeof(char));
    struct stat attr;
    stat(full_path, &attr);

    strftime(last_modified_time, 100, "%a, %d %b %Y %X %Z\r\n", gmtime(&attr.st_mtime));

    strcpy(output, LAST_MODIFIED);
    strcat(output, last_modified_time);
    printf("completed last modified\n");
    return output;
}


char *compile_response(struct Header *header, char *status, int length, char *full_path) {
    char *status_r = status_response(header, status);
    char *date = date_response();
    char *content_t = content_type(header);
    char *content_len = content_length(length);
    char *last_modify = last_modified_response(full_path);
    char *connection_t = connection_type(header);
    char *keepalive = keepalive_time();

    int total = strlen(status_r) + strlen(date) + strlen(MIME)  + strlen(content_t) + strlen(content_len) + strlen(connection_t) + strlen(keepalive) + strlen(last_modify);
    total += strlen(last_modify);
    char *output = malloc(sizeof(char)*total + 1000);

    // do the copy and concat
    strcpy(output, status_r);
    strcat(output,date); // or strncat
    //do we need server?
    strcat(output,MIME);
    strcat(output,last_modify);
    strcat(output,content_t);
    strcat(output,content_len);
    strcat(output, connection_t);
    //Leaving this out for now
//    if (strcmp(header->connectiontype, TYPE_KEEPALIVE) == 0) {
//        strcat(output, keepalive); //only add this header if connection type is keep-alive, it is possible for 1.1 header to pass connection: close
//    }
    strcat(output,END_OF_LINE); //not using END_OF_HEADER because last header may already have /r/n at the end


    free(status_r);
    free(date);
    free(content_t);
    free(content_len);
    free(last_modify);
    free(connection_t);
    free(keepalive);
    printf("COMPILED RESPONSE:\n%s", output);
    return output;
}





void update_tm_struct(char *extract_time, struct tm *timestamp){
    char *month_char = (char *) malloc(3 * sizeof(char));
    char *weekday_char = (char *) malloc(3 * sizeof(char));
    char *timezone = (char *) malloc(3 * sizeof(char));

    int year, day, hour, min, sec;
    sscanf(extract_time, "%s %d %s %d %d:%d:%d %s", weekday_char, &day, month_char, &year, &hour, &min, &sec, timezone );
    //take out the , in the weekday
    weekday_char[strlen(weekday_char)-1] = '\0';
//    printf("weekday_char: %s, day: %d , year: %d, hour: %d , min: %d, sec: %d\n", weekday_char, day, year, hour, min, sec);

    int month = 0;
    for ( int i = 0; MONTH[i] != NULL; i++) {
        if (strcmp(MONTH[i], month_char) == 0) {
            month += 1;
            break;
        }
        month += 1;
    }
    int weekday = 0;
    for ( int i = 0; DAYS_OF_WEEK[i] != NULL; i++) {
        if (strcmp(DAYS_OF_WEEK[i], weekday_char) == 0) {
            break;
        }
        weekday += 1;
    }

    timestamp->tm_wday = weekday;
    timestamp->tm_year = year - 1900;
    timestamp->tm_mon = month - 1;
    timestamp->tm_mday = day;
    timestamp->tm_hour = hour;
    timestamp->tm_min = min;
    timestamp->tm_sec = sec;
    printf("completed update\n");
}


int if_modified_since_time_diff (struct Header *header, char *full_path) {
    //    printf("timestamp: %s\n", asctime (&timestamp));
    //    printf("timestamp2: %s\n", asctime (gmtime(&attr.st_mtime)));
    printf("starting time diff last modified\n");


    // file modified time
    struct stat file_modified_time;
    stat(full_path, &file_modified_time);

    // header modified time
    struct tm header_modified_time; // malloc(sizeof(struct tm)); may need to malloc
    update_tm_struct(header->if_modified_since, &header_modified_time);

    double diff = difftime(mktime(&header_modified_time), mktime(gmtime(&file_modified_time.st_mtime)));

    if ( diff > 0 ) {
        printf("%s is newer than %s\n", header->if_modified_since, asctime (gmtime(&file_modified_time.st_mtime)));
        //failed
        return -1;
    } else {
        printf("%s is older than %s\n", header->if_modified_since, asctime (gmtime(&file_modified_time.st_mtime)));
        return 0;
    }
}

int if_unmodified_since_time_diff (struct Header *header, char *full_path) {
    //    printf("timestamp: %s\n", asctime (&timestamp));
    //    printf("timestamp2: %s\n", asctime (gmtime(&attr.st_mtime)));
    printf("starting time diff last modified\n");


    // file modified time
    struct stat file_unmodified_time;
    stat(full_path, &file_unmodified_time);

    // header modified time
    struct tm header_unmodified_time; // malloc(sizeof(struct tm)); may need to malloc
    update_tm_struct(header->if_unmodified_since, &header_unmodified_time);

    double diff = difftime(mktime(&header_unmodified_time), mktime(gmtime(&file_unmodified_time.st_mtime)));

    if ( diff < 0 ) {
        printf("%s is newer than %s\n", header->if_unmodified_since, asctime (gmtime(&file_unmodified_time.st_mtime)));
        //failed
        return -1;
    } else {
        printf("%s is older than %s\n", header->if_unmodified_since, asctime (gmtime(&file_unmodified_time.st_mtime)));
        return 0;
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




