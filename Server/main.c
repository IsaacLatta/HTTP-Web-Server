#include "server.h"

int main(int argc, char** argv)
{
    if(argc == 1 ||!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help") ||
    !strcmp(argv[1], "--usage") || !strcmp(argv[1], "-?")) {
    print_exit("USAGE: web-server <directory-name> <port> <option>\n"
    "NOTE: This web server only supports GET operations, using HTTP/1.1\n"
    "OPTIONS: -d to daemon the server, to see the real-time server updates compile with -DVERBOSE or add #define VERBOSE to the util.h and compile.\n"
    "UNSUPPORTED DIRECTORIES: / /bin /dev /tmp /lib /usr /etc /sbin /proc /run /mnt /media /boot \"..\" \".\" \"~\" or any other relative paths.");
    }

    int port = atoi(argv[2]);
    if(port < 0 || port > 60000)
    {
        print_exit("invalid port");
    }
    if(strcmp("hello", "h"))

    if(!strcmp(argv[1],"/" ) || !strcmp(argv[1],"/bin")   || !strcmp(argv[1],"/dev" ) ||
    !strcmp(argv[1],"/tmp" ) || !strcmp(argv[1],"/lib")  || !strcmp(argv[1],"/usr" ) ||
    !strcmp(argv[1],"/etc" ) || !strcmp(argv[1],"/sbin") || !strcmp(argv[1], "/proc") ||
    !strcmp(argv[1], "/run") || !strcmp(argv[1], "/mnt") || !strcmp(argv[1], "/media") || 
    !strcmp(argv[1], "/boot") || chroot(argv[1]) == -1)
    {
        print_exit("bad root directory, try running with sudo or -h for help");
    }

    if(argc > 2 && strcmp(argv[3],"-d"))
    {
        pid_t pid = fork();
        if(pid != 0) return 0;
    }
    
    server_t server;
    server_run(&server, port);
    return 0;
}