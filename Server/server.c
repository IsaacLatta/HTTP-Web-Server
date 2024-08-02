#include "server.h"

void _log(int type, char* s1, char* s2, int sockfd)
{
    int fd;
    char log_buffer[1024];
    switch (type)
    {
        case ERROR:
            sprintf(log_buffer, "ERROR: %s:%s Errno=%d exiting pid=%d", s1, s2, errno, getpid()); 
            break;
    }

    if(fd = open("web.log", O_CREAT | O_WRONLY | O_APPEND, 0644)) // rw---r--r
    {
        write(fd, log_buffer, strlen(log_buffer));
        write(fd, "\n", 1); 
        close(fd);
    }
    if(type == ERROR)
    {
        pexit(log_buffer);
    }
}

void server_run(server_t* server, char* root_dir, int port)
{
    if(chroot(root_dir) == -1) // Add valid directories later
    {
        pexit("ERROR: Bad root directory");
    }
    
    if(server->listenfd = socket(AF_INET, SOCK_STREAM, 0) < 0)
    {
        log(ERROR, "sys call", "socket", server->listenfd);
    }

    server->addr.sin_family = AF_INET;
    server->addr.sin_addr.s_addr = INADDR_ANY;
    server->addr.sin_port = htons(port);
   
}