#include "common.h"
#include "TCPreqchannel.h"
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

using namespace std;

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR/DESTRUCTOR FOR CLASS   R e q u e s t C h a n n e l  */
/*--------------------------------------------------------------------------*/

TCPRequestChannel::TCPRequestChannel(const string hostname, const string port_no) {



		struct addrinfo hints, *res;
		// int sockfd;

		// first, load up address structs with getaddrinfo():
			// PLEASE READ ME: https://en.wikipedia.org/wiki/Getaddrinfo
		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;
		int status;

		if ((status = getaddrinfo (hostname.c_str(), port_no.c_str(), &hints, &res)) != 0) {
				cerr << "getaddrinfo: " << gai_strerror(status) << endl;
				exit -1;
			}

		// make a socket:
		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (sockfd < 0){
			perror ("Cannot create socket");	
			exit -1;
		}

		// connect!
		if (connect(sockfd, res->ai_addr, res->ai_addrlen)<0){
			perror ("Cannot Connect");
			exit -1;
		}

		freeaddrinfo(res);
		//
		cout << "Connected " << endl;
		// talk_to_server(sockfd);
		// // return 0;



	// }



}

TCPRequestChannel::TCPRequestChannel(int sockfd_) {

	sockfd = sockfd_;

}

TCPRequestChannel::~TCPRequestChannel() {

	close(sockfd);
}

int TCPRequestChannel::cread(void* msgbuf, int buflen) {
	return recv(sockfd, msgbuf, buflen, 0);
}

int TCPRequestChannel::cwrite(void* msgbuf, int msglen) {
	return send (sockfd, msgbuf, msglen, 0);
}

int TCPRequestChannel::getfd() {

	return sockfd;
}
