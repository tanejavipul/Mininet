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
int get_header(struct Header *header, char *input) {
    printf("INSIDE GET_HEADER: %s\n", input);

    char *main_strtok_pointer = NULL;
    char *extract_token = malloc(sizeof(char) * 10000);

    char *token = strtok_r(input, END_OF_LINE, &main_strtok_pointer); //DO NOT USE STRTOK BAD
    // LOOP THROUGH INPUT AND BREAK IT INTO SPERATE LINES USING '\r\n'
    while (token != NULL) {
        strcpy(extract_token, token);

        //PULL FILENAME + FILE TYPE + HTTP VERSION
        if (contains(extract_token, GET) == 0) {
            header->filename = malloc(sizeof(char) * (strlen(extract_token)));
            header->filetype = malloc(sizeof(char) * (strlen(extract_token)));

            char *http_version = (char *) malloc(sizeof(char) * strlen(extract_token));
            char *file_name = (char *) malloc(sizeof(char) * strlen(extract_token));
            char *file_type;

            //EXTRACT FILENAME AND HTTP VERSION
            int x = sscanf(extract_token, "%*s %s %s", file_name, http_version); //two variables should be printed
            if (x != 2) {
                return -1;
            }
            strcpy(header->filename, file_name);

            //SAVE HTTP VERSION
            if (contains(http_version, HTTP10) == 0) {
                header->http_version = 0;
            } else if (contains(http_version, HTTP11) == 0) {
                header->http_version = 1;
            } else {
                // IN CASE NO OR IMPROPER HTTP VERSION
                return -1;
            }

            //EXTRACT THE FILE TYPE, SEARCH FILENAME FOR '.'
            file_type = strrchr(file_name, '.');

            // IN CASE NO FILETYPE (strrchr return null if it cant find char)
            if (file_type == NULL) {
                return -1;
            }
            strcpy(header->filetype, &file_type[1]); //to get rid of '.' char

            printf("Filename: |%s| Filetype: |%s|\n", header->filename, header->filetype);

            if (contains(file_type, JPG) == 0 || contains(file_type, JPEG) == 0) {
                header->type = malloc(sizeof(char) * (strlen(IMAGE)));
                strcpy(header->type, IMAGE);
            } else if (contains(file_type, CSS) == 0 || contains(file_type, HTML) || contains(file_type, JS) == 0 ||
                       contains(file_type, TXT) == 0) {

                header->type = malloc(sizeof(char) * (strlen(TEXT)));
                strcpy(header->type, TEXT);
            }
            free(file_name);
        }

        //PULL ACCEPT
        if (contains(extract_token, ACCEPT) == 0) {
            //check if accept field is not empty
            if (contains(extract_token, ACCEPT_EMPTY) != 0) {
                header->accept = malloc(sizeof(char) * (strlen(extract_token)));
                char *accept_extract = malloc(sizeof(char) * (strlen(extract_token)));
                int x = sscanf(extract_token, "%*s %s", accept_extract);
                if (x != 1) {
                    return -1;
                }
                strcpy(header->accept, accept_extract);
                printf("Accept: |%s|\n", header->accept);
                free(accept_extract);
            }
        }
        //PULL CONDITIONAL (Currently only if modified)
        if (contains(extract_token, IF_MODIFIED) == 0) {
            printf("extract token if modified: %s\n", extract_token);
            char *output = malloc(sizeof(char) * (strlen(extract_token)));
            int x = sscanf(extract_token, "If-Modified-Since: %[^\t\n]", output);
            if (x != 1) {
                return -1;
            }
            header->if_modified_since = malloc(sizeof(char) * (strlen(output)));
            printf("if_modified_since : %s\n", output);
            strcpy(header->if_modified_since, output);
            free(output);
        }
        if (contains(extract_token, IF_UNMODIFIED) == 0) {
            printf("extract token if unmodified: %s\n", extract_token);
            char *output = malloc(sizeof(char) * (strlen(extract_token)));
            int x = sscanf(extract_token, "If-Unmodified-Since: %[^\t\n]", output);
            if (x != 1) {
                return -1;
            }
            printf("%s\n", output);

            header->if_unmodified_since = malloc(sizeof(char) * (strlen(output)));
            printf("if_unmodified_since : %s\n", output);
            strcpy(header->if_unmodified_since, output);
            free(output);
        }

        if (contains(extract_token, CONNECTION) == 0) {
            printf("extract token CONNECTION: %s\n", extract_token);
            char *output = malloc(sizeof(char) * (strlen(extract_token)));
            sscanf(extract_token, "Connection: %[^\t\n]", output);

            header->connectiontype = malloc(sizeof(char) * (strlen(output)));
            printf("Connection : %s\n", output);
            strcpy(header->connectiontype, output);
            free(output);
        }

        printf(" %s \n", token);
        token = strtok_r(NULL, END_OF_LINE, &main_strtok_pointer);
    }
//if there was no connection type
    if (header->connectiontype == NULL) {
        header->connectiontype = malloc(sizeof(char) * (100));
        if (header->http_version == 0) {
            strcpy(header
                           ->connectiontype, TYPE_CLOSE);
        } else if (header->http_version == 1) {
            strcpy(header
                           ->connectiontype, TYPE_KEEPALIVE);
        } else {
//if there is no connectiontype header and no http_version but this should've failed already
            strcpy(header
                           ->connectiontype, TYPE_CLOSE);
        }
    }
    free(extract_token);
    return 200;
}


/*
 *
 */
void handler(int socket, struct Header *header, char *root_address) {
    char *file_name = header->filename;
    char *full_path = (char *) malloc((strlen(root_address) + strlen(file_name)) * sizeof(char));

    strcpy(full_path, root_address); // Merge the file name that requested and path of the root folder
    strcat(full_path, file_name);

    int fp;
    if ((fp = open(full_path, O_RDONLY)) > 0) //FILE FOUND
    {
        printf("%s Found\n", header->filetype);
        int bytes;
        char buffer[BUFFER_SIZE];

        //getting file size
        struct stat st;
        stat(full_path, &st);
        long file_size = st.st_size;

        if (header->if_modified_since != NULL) {
            int resp = if_modified_since_time_diff(header, full_path);
            if (resp == -1) {
                //Write is not working sending bad response TODO
                write(socket,
                      "HTTP/1.0 304 NOT MODIFIED\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>304 Not Modified</body></html>",
                      strlen("HTTP/1.0 304 NOT MODIFIED\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>304 Not Modified</body></html>"));
                printf("wrote 304\n");
                free(full_path);
                return;
            }
        }
        if (header->if_unmodified_since != NULL) {
            int resp = if_unmodified_since_time_diff(header, full_path);
            if (resp == -1) {
                //Write is not working sending bad response TODO
                write(socket,
                      "HTTP/1.0 404 NOT MODIFIED\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>304 Not Modified</body></html>",
                      strlen("HTTP/1.0 404 NOT MODIFIED\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>304 Not Modified</body></html>"));
                printf("wrote 304\n");
                free(full_path);
                return;
            }
        }

        char *response = compile_response(header, OK, file_size, full_path); //generate response
        send(socket, response, strlen(response), 0);

        while ((bytes = read(fp, buffer, BUFFER_SIZE)) >
               0) // Read the file to buffer. If not the end of the file, then continue reading the file
            write(socket, buffer, bytes); // Send the part of the jpeg to client.
        close(fp);
    } else {
        write(socket,
              "HTTP/1.0 404 NOT FOUND\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>404 File or File Extension not found</body></html>",
              strlen("HTTP/1.0 404 Not Found\r\nConnection: close\r\nContent-Type: text/html\r\n\r\n<!doctype html><html><body>404 File Not Found</body></html>"));
    }
    free(full_path);
}

// TODO add more
void free_memory(struct Header *header) {
    if (header->accept != NULL) {
        free(header->accept);
    }

    if (header->filename != NULL) {
        free(header->filename);
    }

    if (header->filetype != NULL) {
        free(header->filetype);
    }

    if (header->type != NULL) {
        free(header->type);
    }

    if (header->if_modified_since != NULL) {
        free(header->if_modified_since);
    }

    if (header->if_unmodified_since != NULL) {
        free(header->if_unmodified_since);
    }

    if (header->connectiontype != NULL) {
        free(header->connectiontype);
    }

    header->accept = NULL;
    header->filename = NULL;
    header->filetype = NULL;
    header->type = NULL;
    header->if_modified_since = NULL;
    header->if_unmodified_since = NULL;
    header->connectiontype = NULL;
}
