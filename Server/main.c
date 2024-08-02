#include "server.h"

int main(int argc, char** argv)
{
    if(argc < 3 || argc > 3 || 
    !strcmp(argv[1], "-h" || !strcmp(argv[1], "--help") || 
    !strcmp(argv[1], "--usage") || !strcmp(argv[1], "-?")))
    {
        printf("[*] Usage: ./web --directory-name --port");
        exit(EXIT_SUCCESS);
    }
    int port = atoi(argv[2]);
    if(port < 0 || port > 60000)
    {
        pexit("invalid port");
    }

    server_t* server;
    server_run(server, argv[1], port);
    
    return 0;
}