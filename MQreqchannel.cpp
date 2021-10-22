#include "common.h"
#include "MQreqchannel.h"
#include <mqueue.h>

using namespace std;

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR/DESTRUCTOR FOR CLASS   R e q u e s t C h a n n e l  */
/*--------------------------------------------------------------------------*/

MQRequestChannel::MQRequestChannel(const string _name, const Side _side) : RequestChannel(_name, _side) {
	mq1 = "/mq_" + my_name + "1";
    cout << "MY_NAME: " << my_name << endl;
	mq2 = "/mq_" + my_name + "2";
		
	if (_side == SERVER_SIDE){
		wfd = open_messageQueue(mq1, O_RDWR | O_CREAT);
        cout << "GOT TO HERE!" << endl;
		rfd = open_messageQueue(mq2, O_RDWR | O_CREAT);
	}
	else{
		rfd = open_messageQueue(mq1, O_RDWR | O_CREAT);
		wfd = open_messageQueue(mq2, O_RDWR | O_CREAT);
		
	}
	
}

MQRequestChannel::~MQRequestChannel(){ 
	mq_close(wfd);
	mq_close(rfd);

	remove(mq1.c_str());
	remove(mq2.c_str());
}

int MQRequestChannel::open_messageQueue(string _mq_name, int mode){

    mq_attr members;

    members.mq_flags = 0;
    members.mq_maxmsg = 1;
    members.mq_msgsize = 256;
    members.mq_curmsgs = 0;

    int mqd = (int) mq_open (_mq_name.c_str(), mode, 0600, &members);

    if (mqd < 0) {
        EXITONERROR(_mq_name);
    }

    return mqd;

}

int MQRequestChannel::cread(void* msgbuf, int bufcapacity){
	return read (rfd, msgbuf, bufcapacity); 
}

int MQRequestChannel::cwrite(void* msgbuf, int len){
	return write (wfd, msgbuf, len);
}

