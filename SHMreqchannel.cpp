#include "common.h"
#include "SHMreqchannel.h"
#include <mqueue.h>

using namespace std;

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR/DESTRUCTOR FOR CLASS   R e q u e s t C h a n n e l  */
/*--------------------------------------------------------------------------*/

SHMRequestChannel::SHMRequestChannel(const string _name, const Side _side) : RequestChannel(_name, _side) {

    cout << "GOT TO MQREQUEST CONSTRUCTOR!" << endl;

	sh1 = "/mq_" + my_name + "1";
    // cout << "MY_NAME: " << my_name << endl;
	sh2 = "/mq_" + my_name + "2";


		
	if (_side == SERVER_SIDE){
		wfd = open_sh(sh1, O_RDWR | O_CREAT);
        cout << "GOT TO HERE!" << endl;
		rfd = open_sh(sh2, O_RDWR | O_CREAT);
	}
	else{
		rfd = open_sh(sh1, O_RDWR | O_CREAT);
		wfd = open_sh(sh2, O_RDWR | O_CREAT);
		
	}
	
}

SHMRequestChannel::~SHMRequestChannel(){ 
	mq_close(wfd);
	mq_close(rfd);

	mq_unlink(sh1.c_str());
	mq_unlink(sh2.c_str());
}

int SHMRequestChannel::open_sh(string _mq_name, int mode){


    mq_attr members;

    members.mq_flags = 0;
    members.mq_maxmsg = 1;
    members.mq_msgsize = 256;
    members.mq_curmsgs = 0;


    int mqd = (int) mq_open (_mq_name.c_str(), mode, 0600, &members);

    cout << "GOT TO OPEN_MESSAGE_QUEUE FUNCTION!" << endl;

    if (mqd < 0) {
        EXITONERROR(_mq_name);
    }

    return mqd;

}

int SHMRequestChannel::cread(void* msgbuf, int bufcapacity){
	return mq_receive (rfd, (char *)msgbuf, 8192, NULL); 
}

int SHMRequestChannel::cwrite(void* msgbuf, int len){
	// return mq_open (wfd, msgbuf, len);
    return mq_send (wfd, (char*) msgbuf, len, 0);
}

