#include "http_handler.h"

struct
{
    char* ext;
    char* content_type;
} extensions[] = {
    {"html", "text/html"}, 
    {"ico", "image/ico"}, 
    {"png", "image/png"}, 
    {"gif", "image/gif"},
    {NULL, NULL}
    };

static void _send_resource(request_t* request)
{
    int filefd, res;
    if((filefd = open(request->resource, O_RDONLY)) == -1)
    {
        logger(NOT_FOUND, "failed to open file", request->resource, request->sockfd);
        return;
    }

    long file_len = (long)lseek(filefd, (off_t)0, SEEK_END);
    lseek(filefd, 0, SEEK_SET);
    char header[BUFFER_SIZE];
    snprintf(header, sizeof(header), "HTTP/1.1 200 OK\r\nContent-Length: %ld\r\nConnection: close\r\nContent-Type: %s\r\n\r\n", file_len, request->content_type);
    write(request->sockfd, header, strlen(header)); // send header

    logger(LOG, "header", header, request->hit);
    VERBOSE_LOG("sending header: %s\n", header);

    char* buffer = (char*)ec_malloc(file_len);
    while((res = read(filefd, buffer, file_len)) > 0)
    {
        write(request->sockfd, buffer, res); // send file contents
    }
    VERBOSE_LOG("resource: %s sent\n", request->resource);
    logger(LOG, "SEND", request->resource, request->hit);

    free(buffer);
    close(filefd);
}

static int _check_method(request_t* request, char* buffer)
{
    if(strncmp(buffer, "GET ", 4) && strncmp(buffer, "get ", 4))
    {
        logger(FORBIDDEN, "Only GET operation supported", buffer, request->sockfd);
        return ERROR;
    }
    return SUCCESS;
}

static int _sanitize_buffer(request_t* request, char* buffer)
{
    for(int i = 0; i < strlen(buffer); i++) // remove /r/n, check for directory traversal
    {
        if(buffer[i] == '\n' || buffer[i] == '\r') buffer[i] = '*';
        if(i < strlen(buffer) - 2 && buffer[i] == '.' && buffer[i+1] == '.') 
        {
            logger(FORBIDDEN, "parent directory access \"..\" forbidden", buffer, request->sockfd);
            return ERROR;
        }
    }
    return SUCCESS;
}

static int _check_resource(request_t* request)
{
    char *ext;
    if(!strncmp(request->resource, "/", 1) && strlen(request->resource) == 1)
    {
        request->resource = "/index.html";
    }

    char resource[strlen(request->resource) + 1];    
    strcpy(resource, request->resource);
    if(!(ext = strrchr(resource, '.')))
    {
        logger(FORBIDDEN, "no extension found", request->resource, request->sockfd);
        return ERROR;
    }
    ext++;
    
    request->content_type = NULL;
    for(int i = 0; extensions[i].ext != NULL; i++) 
    {
        if(!strcmp(extensions[i].ext, ext))
        {
            request->content_type = extensions[i].content_type;
        }
    }
    if(!request->content_type)
    {
        logger(FORBIDDEN, "file extension not supported", request->resource, request->sockfd);
        return ERROR;
    }
    return SUCCESS;
}

static int _parse_request_line(request_t* request, char* buffer)
{
    if(!(request->method = strtok(buffer, " ")) || !(request->resource = strtok(NULL, " ")))
    {
        logger(BAD_REQUEST, "malformed request", buffer, request->sockfd);
        return ERROR;
    }
    return SUCCESS;
}

static int _parse_request(request_t* request, char* buffer)
{
    if(_sanitize_buffer(request, buffer) == ERROR) return ERROR;
    if(_check_method(request, buffer) == ERROR) return ERROR;
    if(_parse_request_line(request, buffer) == ERROR) return ERROR;
    if(_check_resource(request) == ERROR) return ERROR;
    return SUCCESS;
}

void handle_request(char* buffer, int sockfd, int hit)
{
    VERBOSE_LOG("%s", buffer);
    request_t request;
    request.hit = hit;
    request.sockfd = sockfd;
    if(_parse_request(&request, buffer) == SUCCESS)
    {
        VERBOSE_LOG("Detected request\nMethod: %s\nResource: %s\nContent-Type: %s\nSocket: %d\n", request.method, request.resource, request.content_type, request.sockfd);
        _send_resource(&request);
    }
}

void logger(int type, char* s1, char* s2, int sockfd)
{
    int fd;
    char log_buffer[1024];
    char* response;
    char* log_msg;
    switch (type)
    {
        case BAD_REQUEST:
            response = "HTTP/1.1 400 Bad Request\r\n"; 
            log_msg = "BAD REQUEST";
            write(sockfd, response, strlen(response));
            break;
        case INTERNAL_SERVER_ERROR:
            response = "HTTP/1.1 500 Internal Server Error\r\n";
            sprintf(log_buffer, "INTERNAL SERVER ERROR: Errno=%d exiting pid=%d", errno, getpid()); 
            log_msg = log_buffer;
            write(sockfd, response, strlen(response));
            break;
        case FORBIDDEN:
            response = "HTTP/1.1 403 Forbidden\r\n";
            log_msg = "FORBIDDEN";
            write(sockfd, response, strlen(response));
            break;
        case NOT_FOUND:
            response = "HTTP/1.1 404 Not Found\r\n";
            log_msg = "NOT FOUND";
            write(sockfd, response, strlen(response));
            break;
        case LOG:
            log_msg = "INFO";
            break;
        case ERROR:
            log_msg = "ERROR";
            break;
    }

    if((fd = open("/web.log", O_CREAT | O_WRONLY | O_APPEND, 0644)) > 0) // rw---r--r
    {
        sprintf(log_buffer, "%s: %s %s\n",log_msg, s1, s2);
        write(fd, log_buffer, strlen(log_buffer));
        write(fd, "\n", 1); 
        close(fd);
    }
}
