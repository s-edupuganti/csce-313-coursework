
#ifndef _MQreqchannel_H_
#define _MQreqchannel_H_

#include "common.h"
#include "requestChannel.h"

class MQRequestChannel: public RequestChannel {

private:
	/*  The current implementation uses named pipes. */
	
	int wfd;
	int rfd;
	int bufcap;
	
	string mq1, mq2;
	int open_messageQueue(string _mq_name, int mode);
	
public:

	MQRequestChannel(const string _name, const Side _side, int _bufcap);

	~MQRequestChannel();

	int cread (void* msgbuf, int bufcapacity);
	int cwrite (void *msgbuf , int msglen);
	 
	// string name(); 
};

#endif
