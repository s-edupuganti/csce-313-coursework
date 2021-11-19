#include "common.h"
#include "TCPreqchannel.h"
#include <sys/socket.h>
using namespace std;

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR/DESTRUCTOR FOR CLASS   R e q u e s t C h a n n e l  */
/*--------------------------------------------------------------------------*/

TCPRequestChannel::TCPRequestChannel(const string hostname, const string port_no) {

	if (hostname == "") {

		int sockfd, new_fd;                 // listen on sock_fd, new connection on new_fd
		struct addrinfo hints, *serv;
		struct sockaddr_storage their_addr; //connector's address information
		socklen_t sin_size;
		char s[INET6_ADDRSTRLEN];
		int rv;

		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC;       //any address family IPv4, IPv6
		hints.ai_socktype = SOCK_STREAM;   //stream oriented socket
		hints.ai_flags = AI_PASSIVE;       //use all available IP in this device

		if ((rv = getaddrinfo(NULL, port, &hints, &serv)) != 0) {
			cerr  << "getaddrinfo: " << gai_strerror(rv) << endl;
			return -1;
		}
		if ((sockfd = socket(serv->ai_family, serv->ai_socktype, serv->ai_protocol)) == -1) {
			perror("server: socket");
			return -1;
		}
		if (bind(sockfd, serv->ai_addr, serv->ai_addrlen) == -1) {
			close(sockfd);
			perror("server: bind");
			return -1;
		}
		freeaddrinfo(serv); // all done with this structure

		if (listen(sockfd, 20) == -1) {
			perror("listen");
			exit(1);
		}
		
		// cout << "server: waiting for connections..." << endl;
		// char buf [1024];
		// while(1) {  // main accept() loop
		// 	sin_size = sizeof their_addr;
		// 	int client_socket = accept (sockfd, (struct sockaddr *)&their_addr, &sin_size);
		// 	if (client_socket == -1) {
		// 		perror("accept");
		// 		continue;
		// 	}
		// 	thread t (connection_handler, client_socket);
		// 	t.detach (); 
		// }

		// return 0;

	} else {

		struct addrinfo hints, *res;
		int sockfd;

		// first, load up address structs with getaddrinfo():
			// PLEASE READ ME: https://en.wikipedia.org/wiki/Getaddrinfo
		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		int status;
		if ((status = getaddrinfo (server_name, port, &hints, &res)) != 0) {
				cerr << "getaddrinfo: " << gai_strerror(status) << endl;
				return -1;
			}

		// make a socket:
		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (sockfd < 0){
			perror ("Cannot create socket");	
			return -1;
		}

		// connect!
		if (connect(sockfd, res->ai_addr, res->ai_addrlen)<0){
			perror ("Cannot Connect");
			return -1;
		}
		//
		cout << "Connected " << endl;
		talk_to_server(sockfd);
		return 0;



	}



}

TCPRequestChannel::TCPRequestChannel(int sockfd) {

	sockfd = sockfd;

}

TCPRequestChannel::~TCPRequestChannel() {

	close(sockfd);
}

int TCPRequestChannel::cread(void* msgbuf, int buflen) {
	return recv(sockfd, msgbuf, buflen, 0);
}

int TCPRequestChannel::Cwrite(void* msgbuf, int msglen) {
	return send (sockfd, msgbuf, msglen, 0);
}

int TCPRequestChannel::getfd() {

	return sockfd;
}

// FIFORequestChannel::FIFORequestChannel(const string _name, const Side _side) : my_name( _name), my_side(_side){
// 	pipe1 = "fifo_" + my_name + "1";
// 	pipe2 = "fifo_" + my_name + "2";
		
// 	if (_side == SERVER_SIDE){
// 		wfd = open_pipe(pipe1, O_WRONLY);
// 		rfd = open_pipe(pipe2, O_RDONLY);
// 	}
// 	else{
// 		rfd = open_pipe(pipe1, O_RDONLY);
// 		wfd = open_pipe(pipe2, O_WRONLY);
		
// 	}
	
// }

// FIFORequestChannel::~FIFORequestChannel(){ 
// 	close(wfd);
// 	close(rfd);

// 	remove(pipe1.c_str());
// 	remove(pipe2.c_str());
// }

// int FIFORequestChannel::open_pipe(string _pipe_name, int mode){
// 	mkfifo (_pipe_name.c_str (), 0600);
// 	int fd = open(_pipe_name.c_str(), mode);
// 	if (fd < 0){
// 		EXITONERROR(_pipe_name);
// 	}
// 	return fd;
// }

// int FIFORequestChannel::cread(void* msgbuf, int bufcapacity){
// 	return read(rfd, msgbuf, bufcapacity); 
// }

// int FIFORequestChannel::cwrite(void* msgbuf, int len){
// 	return write(wfd, msgbuf, len);
// }

