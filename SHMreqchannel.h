
#ifndef _SHMreqchannel_H_
#define _SHMreqchannel_H_

#include "common.h"
#include "requestChannel.h"
#include "semaphore.h"
#include "sys/mman.h"

class SHMQueue {

	private:

		int len;

		char* shmbuffer;
		char* name;

		sem_t* readerdone;
		sem_t* writerdone;

		string name1 = "_1";
    	string name2 = "_2";
	
	public:

		SHMQueue(char* _name, int _len);
		~SHMQueue();
		
		int cwrite(void* msg, int len);
		int cread(void* msg, int len);

};

class SHMRequestChannel: public RequestChannel {
	
private:
	
	SHMQueue* shm1;
	SHMQueue* shm2;
	
	string sh1, sh2;

	int bufcap;
	
public:
	SHMRequestChannel(const string _name, const Side _side, int _bufcap);

	~SHMRequestChannel();

	int cread (void* msg, int len);	
	int cwrite (void* msg, int len);
	 
};

#endif
