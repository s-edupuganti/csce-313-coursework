#ifndef _requestChannel_H_
#define _requestChannel_H_

#include "common.h"

class RequestChannel {
    public:
        typedef enum {SERVER_SIDE, CLIENT_SIDE} Side;
        typedef enum {READ_MODE, WRITE_MODE} Mode;

    protected:
    	
	    string my_name;
	    Side my_side;

    public:

        RequestChannel (const string _name, const Side _side) : my_name(_name), my_side(_side) {} // Constructor
        virtual ~RequestChannel() {} // Destructor

        virtual int cread (void* msgbuf, int bufcapacity) = 0;
        virtual int cwrite (void * msgbuf, int bufcapacity) = 0;




};

#endif