
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

// public:
// 	enum Side {SERVER_SIDE, CLIENT_SIDE};
// 	enum Mode {READ_MODE, WRITE_MODE};
	
private:
	/*  The current implementation uses named pipes. */
	
	
	// string my_name;
	// Side my_side;
	
	SHMQueue* shm1;
	SHMQueue* shm2;

	// const char* sh1;
	// const char* sh2;
	
	string sh1, sh2;

	int len;
	
public:
	SHMRequestChannel(const string _name, const Side _side, int _len);
	/* Creates a "local copy" of the channel specified by the given name. 
	 If the channel does not exist, the associated IPC mechanisms are 
	 created. If the channel exists already, this object is associated with the channel.
	 The channel has two ends, which are conveniently called "SERVER_SIDE" and "CLIENT_SIDE".
	 If two processes connect through a channel, one has to connect on the server side 
	 and the other on the client side. Otherwise the results are unpredictable.

	 NOTE: If the creation of the request channel fails (typically happens when too many
	 request channels are being created) and error message is displayed, and the program
	 unceremoniously exits.

	 NOTE: It is easy to open too many request channels in parallel. Most systems
	 limit the number of open files per process.
	*/

	~SHMRequestChannel();
	/* Destructor of the local copy of the bus. By default, the Server Side deletes any IPC 
	 mechanisms associated with the channel. */


	int cread (void* msg, int len);
	/* Blocking read of data from the channel. You must provide the address to properly allocated
	memory buffer and its capacity as arguments. The 2nd argument is needed because the recepient 
	side may not have as much capacity as the sender wants to send.
	
	In reply, the function puts the read data in the buffer and  
	returns an integer that tells how much data is read. If the read fails, it returns -1. */
	
	int cwrite (void* msg, int len);
	/* Writes msglen bytes from the msgbuf to the channel. The function returns the actual number of 
	bytes written and that can be less than msglen (even 0) probably due to buffer limitation (e.g., the recepient
	cannot accept msglen bytes due to its own buffer capacity. */
	 
	string name(); 
};

#endif
