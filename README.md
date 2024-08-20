# HTTP-Web-Server

## Description
- This a an HTTP web server capable of serving static web content, including html/text, image/gif, image/png.
- This web server is written with raw UNIX sockets and thus must be ran on a UNIX system.
- Additionally this web server only supports GET requests.
- This web server was written with intention of learning more about the HTTP protocol, it is designed to be a simple web server, but it still posses the capability of handling many concurrent HTTTP requests.
- The server logs all hits to a file titled "web.log" which will be creating upon running in the root directory of the server. Here all requests, headers and errors, both sent and received will be logged.

## Features

- **Concurrent Handling:** The server can handle many concurrent HTTP requests by utilizing its underlying threadpool, making it efficient and scalable.
- **Static Content:** Capable of serving static content, including HTML, CSS, images and gifs.
- **Logging:** All requests and responses, along with headers and errors are recorded in a file titled 'web.log' for analysis.
- **Error Handling:** Basic error handling for invalid requests, missing files, unsupported methods, and internal server errors are present. The resptive HTTP response is sent to the client when these erros are met.

## Installation

- To compile the server, ensure you have gcc installed, then run the following commands:

'''bash
gcc -pthread -o web-server main.c server.c http_handler.c threadpool.c util.c
- Or to view additional logging and updates to stdout compile via:

'''bash 
gcc -pthread -DVERBOSE -o web-server main.c server.c http_handler.c thtreadpool.c util.c

## Usage

- To run the server, specify either the 'Web-Server' directory I have provided, including my example HTML, CSS and gifs, or with a directory of your choice.
- NOTE: Some directories are prevented for security measures.
- Using the provided folder and port 80 for HTTP, run the following command:

'''bash
./web-server /Path/To/Web-Server 80

- Optionally you can run the server as a daemon with the -d option.
'''bash
./web-server /Path/To/Web-Server 80 -d

- To stop the server, kernel signals SIGTERM(14) and SIGINT(2) may be sent to the process close the seserver. These can be sent via control C for SIGINT or the following command for either:
'''bash 
pkill -15 web-server

## Future Improvements 

- Support for other HTTP methods such as POST, PUT, DELETE, HEAD etc.
- HTTPS Support: Implements TLS/SSL for secure connections.
- Enhanced Logging: Add more detail to logging, such as client IP address and request times.
- Configurable Thread Pool: Allow dynamic adjustment of the thread pool size based on server load.

## Troubleshooting

- Ensure your firewall has been configured to allow TCP connections on your desired port. Some firewalls may block TCP connections on lower numbered ports, try running with ports > 1025.
- Additionally the server may be required to run with 'sudo', this is likely due the 'chroot' function call which will change the root directory of the server to the one specified in the command line argument.

## Contributing
- Contributions are welcome! If you have any suggestions or improvements, feel free to open an issure or submit a pull request.
