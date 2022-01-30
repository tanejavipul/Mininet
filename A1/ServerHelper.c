#include "Parser.c"

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


//    get_header(&request, buffer);
void get_header(struct Request *req, char* header) {
    printf("INSIDE GET_HEADER: %s\n", header);

    char *main_strtok_pointer = NULL;
    char *extract_token = malloc(sizeof(char)*10000);

    char *token = strtok_r(header, END_OF_LINE, &main_strtok_pointer); //DO NOT USE strtok

    while(token != NULL) {
        strcpy(extract_token,token);

        //PULL FILENAME + FILE TYPE
        if(contains(extract_token, GET)==0)
        {
            //get filename
            req->filename = malloc(sizeof(char)*(strlen(extract_token)));
            req->filetype = malloc(sizeof(char)*(strlen(extract_token)));

            char *file_name = (char *) malloc(sizeof(char) * strlen(extract_token));
            char *file_type;
            sscanf(extract_token,"%*s %s %*s",file_name);
            strcpy(req->filename, file_name);

            // TODO FIX IN CASE NO FILETYPE DONT THINK WE CAN STRTOK IT
            // TODO strrchr return null if it cant find char
            file_type = strrchr(file_name, '.');
            strcpy(req->filetype, &file_type[1]); //to get rid of '.' char

            printf("Filename: |%s| Filetype: |%s|\n",req->filename,req->filetype);

            // TODO need to add support for JPEG and JPG not just JPG
            if(strcmp(file_type, JPG) == 0) {
                req->type = malloc(sizeof(char)*(strlen(IMAGE)));
                strcpy(req->type, IMAGE);
            } else { // TODO filetype is html, css, txt or js, NOTE: might be an issue if we want to handle additional extensions, IDEA: send BAD REQUEST if not html css txt or js
                req->type = malloc(sizeof(char)*(strlen(TEXT)));
                strcpy(req->type, TEXT);
            }

            free(file_name);
        }

        //PULL ACCEPT
        if(contains(extract_token, ACCEPT)==0)
        {
            //TODO NEED TO FIX AND TEST
            //check if accept field is not empty
            if(contains(extract_token, ACCEPT_EMPTY)!=0) {
                req->accept = malloc(sizeof(char)*(strlen(extract_token)));
                char *accept_extract = malloc(sizeof(char)*(strlen(extract_token)));
                sscanf(extract_token,"%*s %s", accept_extract);
                strcpy(req->accept, accept_extract);
                printf("Accept: |%s|\n", req->accept);
            }
        }
        //PULL CONDITIONAL (Currently only if modified)
        if(contains(extract_token, IF_MODIFIED)==0)
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


void update_tm_struct(struct Request *req, struct tm *timestamp){
    char *month_char = (char *) malloc(3 * sizeof(char));
    char *weekday_char = (char *) malloc(3 * sizeof(char));
    char *timezone = (char *) malloc(3 * sizeof(char));

    int year, day, hour, min, sec;
    sscanf(req->if_modified_timestamp, "%s %d %s %d %d:%d:%d %s", weekday_char, &day, month_char, &year, &hour, &min, &sec, timezone );
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
}


char *last_modified(struct Request *req, char *full_path) {
    //Fri, 08 Aug 2003 08:12:31 GMT
    char *output = (char *) malloc(100 * sizeof(char));
    char *last_modified_time = (char *) malloc(100 * sizeof(char));
    struct stat attr;
    stat(full_path, &attr);

    strftime(last_modified_time, 100, "%a, %d %b %Y %X %Z\r\n", gmtime(&attr.st_mtime));

    struct tm timestamp; // malloc(sizeof(struct tm)); may need to malloc
    update_tm_struct(req, &timestamp);

//    printf("timestamp: %s\n", asctime (&timestamp));
//    printf("timestamp2: %s\n", asctime (gmtime(&attr.st_mtime)));


    double diff = difftime(mktime(&timestamp), mktime(gmtime(&attr.st_mtime)));

    if ( diff > 0 ) {
        printf("%s is newer than %s\n", req->if_modified_timestamp, last_modified_time);
        return NULL;
    } else {
        printf("%s is older than %s\n", req->if_modified_timestamp, last_modified_time);
        strcpy(output, LAST_MODIFIED);
        strcat(output, last_modified_time);
        return output;
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

        //if modified since
        char *last_modify = last_modified(req, full_path);
        if (last_modify == NULL) {
            //Write is not working sending bad response TODO
            write (socket, "HTTP/1.0 304 NOT MODIFIED\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>304 Not Modified</body></html>", strlen("HTTP/1.0 304 NOT MODIFIED\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>304 Not Modified</body></html>"));
            printf("wrote 304\n");
            free(full_path);
            return;
        }

        char* response = compile_response(req, OK, file_size, full_path, last_modify); //generate response
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

// TODO add more
void free_memory(struct Request *req) {
    if (req->accept != NULL) {
        free(req->accept);
    }

    if (req->filename != NULL) {
        free(req->filename);
    }

    if (req->filetype != NULL) {
        free(req->filetype);
    }

    if (req->type != NULL) {
        free(req->type);
    }

    if (req->accept != NULL) {
        free(req->accept);
    }

    if (req->if_modified_timestamp != NULL) {
        free(req->if_modified_timestamp);
    }

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
