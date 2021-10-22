#include "common.h"
#include "MQreqchannel.h"
#include <mqueue.h>

using namespace std;

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR/DESTRUCTOR FOR CLASS   R e q u e s t C h a n n e l  */
/*--------------------------------------------------------------------------*/

MQRequestChannel::MQRequestChannel(const string _name, const Side _side) : RequestChannel(_name, _side) {

    // cout << "GOT TO MQREQUEST CONSTRUCTOR!" << endl;

	mq1 = "/mq_" + my_name + "1";
    // cout << "MY_NAME: " << my_name << endl;
	mq2 = "/mq_" + my_name + "2";


		
	if (my_side == SERVER_SIDE){
		wfd = open_messageQueue(mq1, O_RDWR | O_CREAT);
        // cout << "GOT TO HERE!" << endl;
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

	mq_unlink(mq1.c_str());
	mq_unlink(mq2.c_str());
}

int MQRequestChannel::open_messageQueue(string _mq_name, int mode){


    struct mq_attr members;

    members.mq_flags = 0;
    members.mq_maxmsg = 1;
    members.mq_msgsize = 256;
    members.mq_curmsgs = 0;


    int mqd = (int) mq_open (_mq_name.c_str(), mode, 0600, &members);

    // cout << "GOT TO OPEN_MESSAGE_QUEUE FUNCTION!" << endl;

    if (mqd < 0) {
        EXITONERROR(_mq_name);
    }

    return mqd;

}

int MQRequestChannel::cread(void* msgbuf, int bufcapacity){
	// cout << "reading from " << my_side << endl;
	return mq_receive (rfd, (char *)msgbuf, 8192, NULL); 
}

int MQRequestChannel::cwrite(void* msgbuf, int len){
	// cout << "writing " << ((char*)msgbuf) << " from " << my_side << endl;
	// return mq_open (wfd, msgbuf, len);
    return mq_send (wfd, (char*) msgbuf, len, 0);
}

