
// #ifndef _FIFOreqchannel_H_
// #define _FIFOreqchannel_H_

#ifndef _TCPreqchannel_H_
#define _TCPreqchannel_H

#include "common.h"

class TCPRequestChannel
{


	
private:
	/*  The current implementation uses named pipes. */

	int sockfd;
	

	
public:

	TCPRequestChannel (const string hostname, const string port_no);

	TCPRequestChannel (int sockfd);

	~TCPRequestChannel();

	int cread (void* msgbuf, int buflen);
	int cwrite(void* msgbuf, int msglen);

	int getfd();

};

#endif
