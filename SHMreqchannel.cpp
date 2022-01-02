#include "common.h"
#include "SHMreqchannel.h"

#include "semaphore.h"

using namespace std;

SHMQueue::SHMQueue(char* _name, int _len): name(_name), len(_len) {

    int fd = shm_open(name, O_RDWR | O_CREAT, 0600);

    if (fd < 0) {
        EXITONERROR("No memory segment!");
    }

    ftruncate(fd, len);

    shmbuffer = (char*) mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (!shmbuffer) {
        EXITONERROR("No access to memory segment!");
    }

    
    readerdone = sem_open((name + name1).c_str(), O_CREAT, 0600, 1);
    writerdone = sem_open((name + name2).c_str(), O_CREAT, 0600, 0);
    
}

SHMQueue::~SHMQueue() {

    sem_close(writerdone);
    sem_close(readerdone);

    sem_unlink((name + name1).c_str());
    sem_unlink((name + name2).c_str());

    munmap(shmbuffer, len);

    shm_unlink(name);

}

int SHMQueue::cwrite(void* msg, int len) {

    sem_wait(readerdone);
    memcpy(shmbuffer, msg, len);
    sem_post(writerdone);

    return len;

}

int SHMQueue::cread(void* msg, int len) {

    sem_wait(writerdone);
    memcpy(msg, shmbuffer, len);
    sem_post(readerdone);

    return len;

}

/*--------------------------------------------------------------------------*/
/* CONSTRUCTOR/DESTRUCTOR FOR CLASS   R e q u e s t C h a n n e l  */
/*--------------------------------------------------------------------------*/

SHMRequestChannel::SHMRequestChannel(const string _name, const Side _side, int _bufcap) : RequestChannel(_name, _side) {

	sh1 = "/shm_" + my_name + "1";
	sh2 = "/shm_" + my_name + "2";

    bufcap = _bufcap;

	if (my_side == SERVER_SIDE){
        shm1 = new SHMQueue(const_cast<char*>(sh1.c_str()), bufcap);
        shm2 = new SHMQueue(const_cast<char*>(sh2.c_str()), bufcap);
	} else{
        shm1 = new SHMQueue(const_cast<char*>(sh2.c_str()), bufcap);
        shm2 = new SHMQueue(const_cast<char*>(sh1.c_str()), bufcap);       
	}

}

SHMRequestChannel::~SHMRequestChannel(){ 

    delete shm1;
    delete shm2;

}

int SHMRequestChannel::cread(void* msgbuf, int bufcapacity){
 
    return shm1->cread((char *) msgbuf, bufcapacity);
}

int SHMRequestChannel::cwrite(void* msgbuf, int len){

    return shm2->cwrite(msgbuf, len);

}

