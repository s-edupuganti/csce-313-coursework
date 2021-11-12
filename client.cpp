
#include "common.h"
#include "BoundedBuffer.h"
#include "Histogram.h"
#include "common.h"
#include "HistogramCollection.h"
#include "FIFOreqchannel.h"

using namespace std;

class Reply {

    public:

        int p;
        double ecg;

};


void patient_thread_function(BoundedBuffer* reqBuf, int patient, int num){
    /* What will the patient threads do? */

        /* What will the patient threads do? */

    // datamsg args = (which patient?, which secondes?, which ecg - 1 or 2?)

    // double diff = 0.004;
    // double numDble = (double) num;

    datamsg dm (patient, 0.00, 1);

    for (int i = 0; i < num; i++) {

        reqBuf->push ((char*) &dm, sizeof (datamsg));
        dm.seconds += .004;

    }
}

void worker_thread_function(/*add necessary arguments*/){
    /*
		Functionality of the worker threads	
    */
}
void histogram_thread_function (/*add necessary arguments*/){
    /*
		Functionality of the histogram threads	
    */
}



int main(int argc, char *argv[])
{

    int opt;
    int n = 100;    		//default number of requests per "patient"
    int p = 10;     		// number of patients [1,15]
    int w = 100;    		//default number of worker threads
    int b = 20; 		// default capacity of the request buffer, you should change this default
    int h;
	int m = MAX_MESSAGE; 	// default capacity of the message buffer

    string filename = "";

    srand(time_t(NULL));

    while ((opt = getopt(argc, argv, "n:p:w:b:m:h:")) != -1) {

        switch(opt) {
            case 'n':
                n = atoi(optarg);
                break;
            case 'p':
                p = atoi(optarg);
                break;
            case 'w':
                w = atoi(optarg);
                break;
            case 'b':
                b = atoi(optarg);
                break;
            case 'm':
                m = atoi(optarg);
                break;
            case 'h':
                h = atoi(optarg);
                break;
        }
    }
    
    
    int pid = fork();
    if (pid == 0){
		// modify this to pass along m
        execl ("server", "server", (char *)NULL);
    }
    
	FIFORequestChannel* chan = new FIFORequestChannel("control", FIFORequestChannel::CLIENT_SIDE);
    BoundedBuffer request_buffer(b);
	HistogramCollection hc;
	
	
	
    struct timeval start, end;
    gettimeofday (&start, 0);

    /* Start all threads here */
	

	/* Join all threads here */
    gettimeofday (&end, 0);
    // print the results
	hc.print ();
    int secs = (end.tv_sec * 1e6 + end.tv_usec - start.tv_sec * 1e6 - start.tv_usec)/(int) 1e6;
    int usecs = (int)(end.tv_sec * 1e6 + end.tv_usec - start.tv_sec * 1e6 - start.tv_usec)%((int) 1e6);
    cout << "Took " << secs << " seconds and " << usecs << " micro seconds" << endl;

    MESSAGE_TYPE q = QUIT_MSG;
    chan->cwrite ((char *) &q, sizeof (MESSAGE_TYPE));
    cout << "All Done!!!" << endl;
    delete chan;
    
}