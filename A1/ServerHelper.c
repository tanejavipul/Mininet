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

int get_header(struct Header *header, char *input) {

    char *main_strtok_pointer = NULL;
    char *extract_token = malloc(sizeof(char) * 10000);

    char *token = strtok_r(input, END_OF_LINE, &main_strtok_pointer);
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
                free(http_version);
                free(file_name);
                free(extract_token);
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
                free(http_version);
                free(file_name);
                free(extract_token);

                return -1;
            }

            //EXTRACT THE FILE TYPE, SEARCH FILENAME FOR '.'
            file_type = strrchr(file_name, '.');

            // IN CASE NO FILETYPE (strrchr return null if it cant find char)
            if (file_type == NULL) {
                free(http_version);
                free(file_name);
                free(extract_token);

                return -1;
            }
            strcpy(header->filetype, &file_type[1]); //to get rid of '.' char

            if (contains(file_type, JPG) == 0 || contains(file_type, JPEG) == 0 || contains(file_type, PNG) == 0) {
                header->type = malloc(sizeof(char) * (strlen(IMAGE)));
                strcpy(header->type, IMAGE);
            }
            else if (contains(file_type, CSS) == 0 || contains(file_type, HTML) == 0 || contains(file_type, JS) == 0 ||
                       contains(file_type, TXT) == 0) {

                header->type = malloc(sizeof(char) * (strlen(TEXT)));
                strcpy(header->type, TEXT);
            }
            else {
                free(http_version);
                free(file_name);
                free(extract_token);
                return -1;
            }

            free(file_name);
            free(http_version);
        }

        //PULL ACCEPT
        if (contains(extract_token, ACCEPT) == 0) {
            //check if accept field is not empty
            if (contains(extract_token, ACCEPT_EMPTY) != 0) {
                header->accept = malloc(sizeof(char) * (strlen(extract_token)));
                char *accept_extract = malloc(sizeof(char) * (strlen(extract_token)));
                int x = sscanf(extract_token, "%*s %s", accept_extract);
                if (x != 1) {
                    free(accept_extract);
                    free(extract_token);
                    return -1;
                }
                strcpy(header->accept, accept_extract);
                free(accept_extract);
            }
        }
        //Pull Conditional: IF MODIFIED
        if (contains(extract_token, IF_MODIFIED) == 0) {
            char *output = malloc(sizeof(char) * (strlen(extract_token)));
            int x = sscanf(extract_token, "If-Modified-Since: %[^\t\n]", output);
            if (x != 1) {
                free(output);
                free(extract_token);
                return -1;
            }
            header->if_modified_since = malloc(sizeof(char) * (strlen(output)));
            strcpy(header->if_modified_since, output);
            free(output);
        }

        //Pull Conditional: IF UNMODIFIED
        if (contains(extract_token, IF_UNMODIFIED) == 0) {

            char *output = malloc(sizeof(char) * (strlen(extract_token)));
            int x = sscanf(extract_token, "If-Unmodified-Since: %[^\t\n]", output);
            if (x != 1) {
                free(output);
                free(extract_token);
                return -1;
            }

            header->if_unmodified_since = malloc(sizeof(char) * (strlen(output)));
            printf("if_unmodified_since : %s\n", output);
            strcpy(header->if_unmodified_since, output);
            free(output);
        }

        //PULL CONNECTION header
        if (contains(extract_token, CONNECTION) == 0) {

            char *output = malloc(sizeof(char) * (strlen(extract_token)));
            int x = sscanf(extract_token, "Connection: %[^\t\n]", output);
            if(x == 1) {
                header->connectiontype = malloc(sizeof(char) * (strlen(output)));
                strcpy(header->connectiontype, output);
            }
            free(output);
        }
        token = strtok_r(NULL, END_OF_LINE, &main_strtok_pointer);
    }

    //If there is no CONNECTION header
    if (header->connectiontype == NULL) {
        header->connectiontype = malloc(sizeof(char) * (100));
        if (header->http_version == 0) {
            strcpy(header->connectiontype, TYPE_CLOSE);
        } else if (header->http_version == 1) {
            strcpy(header->connectiontype, TYPE_KEEPALIVE);
        } else {
            //if there is no CONNECTION header or HTTP VERSION but this should've failed already
            free(extract_token);
            return -1;
        }
    }
    free(extract_token);
    return 200;
}


//process client request
void handler(int socket, struct Header *header, char *root_address) {
    char *file_name = header->filename;
    char *full_path = (char *) malloc((strlen(root_address) + strlen(file_name)) * sizeof(char));

    // Merge the file name and path of the root folder
    strcpy(full_path, root_address);
    strcat(full_path, file_name);

    int fp;
    if ((fp = open(full_path, O_RDONLY)) > 0) //FILE FOUND
    {
        int bytes;
        char buffer[BUFFER_SIZE];

        //getting file size
        struct stat st;
        stat(full_path, &st);
        long file_size = st.st_size;

        //handling if_modified_since conditional
        if (header->if_modified_since != NULL) {
            int resp = if_modified_since_time_diff(header, full_path);
            if (resp == -1) {
                //1.0 http request
                if (header->http_version == 0) {
                    write(socket, RESPONSE_304_0_CLOSE, strlen(RESPONSE_304_0_CLOSE));
                //1.1 http request
                } else {
                    //if connection type specified as close otherwise keep-alive
                    if ( contains(header->connectiontype, TYPE_CLOSE) == 0 ) {
                        write(socket, RESPONSE_304_0_CLOSE, strlen(RESPONSE_304_0_CLOSE));
                    } else {
                        write(socket, RESPONSE_304_KEEP, strlen(RESPONSE_304_KEEP));
                    }
                }
                free(full_path);
                return;
            }
        }

        //handling if_unmodified_since conditional
        if (header->if_unmodified_since != NULL) {
            int resp = if_unmodified_since_time_diff(header, full_path);
            if (resp == -1) {
                //1.0 http request
                if (header->http_version == 0) {
                    write(socket, RESPONSE_304_0_CLOSE, strlen(RESPONSE_304_0_CLOSE));
                //1.1 http request
                } else {
                    //if connection type specified as close otherwise keep-alive
                    if ( contains(header->connectiontype, TYPE_CLOSE) == 0 ) {
                        write(socket, RESPONSE_304_0_CLOSE, strlen(RESPONSE_304_0_CLOSE));
                    } else {
                        write(socket, RESPONSE_304_KEEP, strlen(RESPONSE_304_KEEP));
                    }
                }
                free(full_path);
                return;
            }
        }

        //generate response
        char *response = compile_response(header, OK, file_size, full_path);
        send(socket, response, strlen(response), 0);
        free(response);

        // Read the file to buffer. If not the end of the file, then continue reading the file
        while ((bytes = read(fp, buffer, BUFFER_SIZE)) > 0)
            write(socket, buffer, bytes);
        close(fp);
    } else {
        //file could not be found

        //1.0 http request
        if (header->http_version == 0) {
            write(socket, RESPONSE_404_0_CLOSE, strlen(RESPONSE_404_0_CLOSE));
        //1.1 http request
        } else {
            //if connection type specified as close otherwise keep-alive
            if ( contains(header->connectiontype, TYPE_CLOSE) == 0 ) {
                write(socket, RESPONSE_404_0_CLOSE, strlen(RESPONSE_404_0_CLOSE));
            } else {
                write(socket, RESPONSE_404_KEEP, strlen(RESPONSE_404_KEEP));
            }
        }
    }
    free(full_path);
}

// freeing malloced variables
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
