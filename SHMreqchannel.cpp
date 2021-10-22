#include "common.h"
#include "SHMreqchannel.h"

#include "semaphore.h"

using namespace std;


SHMQueue::SHMQueue(char* _name, int _len): name(_name), len(_len) {

    int fd = shm_open(name, O_RDWR | O_CREAT, 0600);

    ftruncate(fd, len);

    shmbuffer = (char*) mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  
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

SHMRequestChannel::SHMRequestChannel(const string _name, const Side _side, int _len) : RequestChannel(_name, _side) {

	sh1 = "/shm_" + my_name + "1";
	sh2 = "/shm_" + my_name + "2";

    len = _len;

    shm1 = new SHMQueue(const_cast<char*>(sh1.c_str()), len);
    shm2 = new SHMQueue(const_cast<char*>(sh2.c_str()), len);

    SHMQueue* tempSHM;

    if (my_side == CLIENT_SIDE) {
        tempSHM = shm1;
        shm1 = shm2;
        shm2 = tempSHM;
    }

}

SHMRequestChannel::~SHMRequestChannel(){ 

    delete shm1;
    delete shm2;

}


int SHMRequestChannel::cread(void* msgbuf, int bufcapacity){
 
    // cout << "reading from " << my_side << endl;

    return shm1->cread((char *) msgbuf, bufcapacity);
}

int SHMRequestChannel::cwrite(void* msgbuf, int len){

    // cout << "writing " << ((char*)msgbuf) << " from " << my_side << endl;

    return shm2->cwrite(msgbuf, len);

}

