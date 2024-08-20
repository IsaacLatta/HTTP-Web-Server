#include "server.h"

volatile int isRunning = 1;

void sig_handler(int sig)
{
    if(sig == SIGINT || sig == SIGTERM)
    {
        isRunning = 0;
    }
}

static void _handle_client(server_t* server, int clientfd, int hit)
{
    int* hit_num = (int*)ec_malloc(sizeof(int));
    int* client_sock = (int*)ec_malloc(sizeof(int));
    *hit_num = hit;
    *client_sock = clientfd;
    if(threadpool_push(server->pool, server_worker,(void*)client_sock, (void*)hit_num, NULL) != 0)
    {
        logger(ERROR, "threadpool_push", "", clientfd);
        free(hit_num);
        free(client_sock);
    }
}

void server_worker(void* arg)
{
    threadpool_args_t* args = (threadpool_args_t*)arg;
    int* clientfd = (int*)args->arg1;
    int* hit = (int*)args->arg2;
    static char buffer[BUFFER_SIZE + 1];
    int res;
    
    VERBOSE_LOG("waiting for client request...\n");
    res = read(*clientfd, buffer, BUFFER_SIZE);
    if (res < 0)
    {
        logger(INTERNAL_SERVER_ERROR, "sys call", "read", *clientfd);
        pexit("INTERNAL SERVER ERROR\n"); 
    }
    buffer[res] = '\0';
    logger(LOG, "request", buffer, *hit);
    
    handle_request(buffer, *clientfd, *hit);
    close(*clientfd);
}

static void _accept_connections(server_t *server)
{
    int clientfd;
    struct sockaddr_in client_addr;
    socklen_t len = sizeof(client_addr);

    for (int hit = 1; isRunning > 0; hit++)
    {
            if ((clientfd = accept(server->listenfd, (struct sockaddr *)&client_addr, &len)) < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
            {
                logger(ERROR, "sys call", "accept", 0);
            }
            if(clientfd > 0)
            {
                 VERBOSE_LOG("client connected\n");
                _handle_client(server, clientfd, hit);
            }
            sleep_ms(100);
    }
    close(server->listenfd);
}

static void _server_init(server_t* server, int port)
{
    VERBOSE_LOG("init http-server\n");
    signal(SIGTERM, sig_handler);
    signal(SIGINT, sig_handler);

    server->addr.sin_family = AF_INET;
    server->addr.sin_addr.s_addr = INADDR_ANY;
    server->addr.sin_port = htons(port);
    if ((server->listenfd = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
    {
        logger(INTERNAL_SERVER_ERROR, "sys call", "socket", 0);
        pexit("socket failed");
    }
    int flags = fcntl(server->listenfd, F_GETFL, 0);
    fcntl(server->listenfd, F_SETFL, flags | O_NONBLOCK);

    if (bind(server->listenfd, (const struct sockaddr *)&server->addr, sizeof(server->addr)) < 0)
    {
        logger(INTERNAL_SERVER_ERROR, "sys call", "bind", 0);
        close(server->listenfd);
        pexit("bind failed");
    }
    if (listen(server->listenfd, 64) < 0)
    {
        logger(INTERNAL_SERVER_ERROR, "sys call", "listen", 0);
        close(server->listenfd);
        pexit("listen failed");
    }
    server->pool = threadpool_create(10);
}

void server_run(server_t *server, int port)
{
    _server_init(server, port);
    VERBOSE_LOG("server up, listening for connections on port: %d\n", port);
    _accept_connections(server);
    threadpool_destroy(server->pool);
    VERBOSE_LOG("server closed on port %d, exiting gracefully\n", port);
}