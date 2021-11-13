
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

void worker_thread_function(BoundedBuffer* reqBuf, BoundedBuffer* respBuf, FIFORequestChannel* newWChan, int bufferCap, string filename){
    
    char request [1024];


    while (true) {


        reqBuf->pop(request, 1024);

        MESSAGE_TYPE* msg = (MESSAGE_TYPE*) request;

        if (*msg == DATA_MSG) {

            datamsg* dm = (datamsg*) request;
            double ecgNum;


            newWChan->cwrite(dm, sizeof(datamsg));
            newWChan->cread(&ecgNum, sizeof(double));

            // cout << "Ecgnum: " << ecgNum << endl;

            Reply resp;

            resp.p = dm->person;
            resp.ecg = ecgNum;

            respBuf->push ((char*) &resp, sizeof (resp));

        } else if (*msg == QUIT_MSG) {

            newWChan->cwrite(msg, sizeof(MESSAGE_TYPE));
            delete newWChan;
            break;

        } else if (*msg == FILE_MSG) {

            char buffRec[bufferCap];

            filemsg *fm = (filemsg*) request;

            string filename = (char*) (fm + 1);

            int msgSize = sizeof(filemsg) + filename.size() + 1;

            newWChan->cwrite(request, msgSize);
            newWChan->cread(buffRec, bufferCap);

            string output = "received/" + filename;

            int_least64_t msgOffset = fm->offset;
            int_least64_t msgLen = fm->length;

            
            
            

            // filemsg *fm = (filemsg*) request;
            // // string fname = filename;
            // int len = sizeof (filemsg) + filename.size() + 1;

            // newWChan->cwrite(request, len);
            // newWChan->cread(buf2, bufferCap);

            FILE* pfile = fopen(output.c_str(), "r+");
            fseek(pfile, msgOffset, SEEK_SET);
            fwrite(buffRec, 1, msgLen, pfile);
            fclose(pfile);

     

        }
    }


}

void file_thread_function (BoundedBuffer* reqBuf, FIFORequestChannel* chan, int bufCap, string filename){

    char buf[1024];
    __int64_t fileLength;

    filemsg fm (0, 0);
    string fname = "Received/" + filename;

    memcpy(buf, &fm, sizeof(fm));
    strcpy(buf + sizeof(fm), filename.c_str());

    chan->cwrite(buf, sizeof(fm) + filename.size() + 1);
    chan->cread(&fileLength, sizeof(__int64_t));

    FILE* pfile = fopen(fname.c_str(), "w");
    fseek(pfile, fileLength, SEEK_SET);
    fclose(pfile);

    filemsg* fm2 = (filemsg*) buf;

    __int64_t fileLengthTemp = fileLength;

    while (fileLengthTemp > 0) {

        if (bufCap > fileLength) {

            bufCap = fileLength;

        }

        reqBuf->push(buf, sizeof(filemsg) + filename.size() + 1);

        fm2->offset += fm2->length;
        fileLength -= fm2->length;
;
    }

}



void histogram_thread_function (BoundedBuffer* respBuf, HistogramCollection* histColl, int bufferCap){
    /*
		Functionality of the histogram threads	
    */

char response[1024];


   while (true) {

       respBuf->pop(response, 1024);

       Reply* resp = (Reply*) response;

       if (resp->p < 0) {
           break;
       } else {

           ((histColl->get_hist_vect())[(resp->p) - 1])->update(resp->ecg);

        // cout << "Patient: " << resp->p << "|| Ecg: " << resp->ecg << endl; 
           
       }
       
   }


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

    bool isFile = false;

    srand(time_t(NULL));

    while ((opt = getopt(argc, argv, "n:p:w:b:m:h:f:")) != -1) {

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
            case 'f':
                filename = optarg;
                isFile = true;
                break;
        }
    }
    
    
    int pid = fork();
    if (pid == 0){
		// modify this to pass along m
        char *argv[] = {"./server", "-m", (char*)to_string(m).c_str(), NULL};
        execvp(argv[0], argv);	
    }
    
	FIFORequestChannel* chan = new FIFORequestChannel("control", FIFORequestChannel::CLIENT_SIDE);
    FIFORequestChannel* newWChans[w];

    BoundedBuffer request_buffer(b);
    BoundedBuffer response_buffer(b);

	HistogramCollection hc;
	
    struct timeval start, end;
    gettimeofday (&start, 0);

    /* Start all threads here */
    thread patientThr [p];
    thread workerThr [w];
    thread histogramThr [h];

    cout << "Creating Histograms...";


    for (int i = 0; i < p; i++){

        Histogram* hist = new Histogram (10, -2.0, 2.0);
        hc.add(hist);

    }

    cout << "Histograms Created" << endl;

    cout << "Creating Worker Channels...";

    for (int i = 0; i < w; i++) { // Worker Channels

        MESSAGE_TYPE nc = NEWCHANNEL_MSG;
        chan->cwrite(&nc, sizeof(nc));
        char ncChar[1024];
        chan->cread(&ncChar, 1024);
        newWChans[i] = new FIFORequestChannel (ncChar, FIFORequestChannel::CLIENT_SIDE);


    }

    cout << "Worker Channels Created" << endl;

    if (isFile) {

        thread fileThr;

        fileThr = thread (file_thread_function, &request_buffer, chan, m, filename);

        for (int i = 0; i < w; i++) {

            workerThr[i] = thread (worker_thread_function, &request_buffer, &response_buffer, newWChans[i], m, filename);

        }

        for (int i = 0; i < h; i++) {

            histogramThr[i] = thread (histogram_thread_function, &response_buffer, &hc, m);
        }

        fileThr.join();

        for (int i = 0; i < w; i++) {

            MESSAGE_TYPE quit = QUIT_MSG;
            request_buffer.push ((char*) &quit, sizeof (MESSAGE_TYPE));

        }

        for (int i = 0; i < w; i++) {

            workerThr[i].join();

        }

        Reply response;

        response.ecg = 0;
        response.p = -1;

        for (int i = 0; i < h; i++) {

            response_buffer.push((char*) &response, sizeof(response));
        }

        for (int i = 0; i < h; i++) {

            histogramThr[i].join();
        }

        gettimeofday (&end, 0);

    } else {

        cout << "Creating Patient Threads...";

        for (int i = 0; i < p; i++) {
            patientThr[i] = thread (patient_thread_function, &request_buffer, i + 1, n);
        }

        cout << "Patient Threads Created" << endl;

        cout << "Creating Worker Threads...";

        for (int i = 0; i < w; i++) {

            workerThr[i] = thread (worker_thread_function, &request_buffer, &response_buffer, newWChans[i], m, filename);

        }

        cout << "Worker Threads Created" << endl;

        cout << "Creating Histogram Threads...";

        for (int i = 0; i < h; i++) {

            histogramThr[i] = thread (histogram_thread_function, &response_buffer, &hc, m);
        }

        cout << "Histogram Threads Created" << endl;
        

        /* Join all threads here */

        for (int i = 0; i < p; i++) {

            patientThr[i].join();

        }

        for (int i = 0; i < w; i++) {

            MESSAGE_TYPE quit = QUIT_MSG;

            request_buffer.push ((char*) &quit, sizeof (MESSAGE_TYPE));
        }

        for (int i = 0; i < w; i++) {

            workerThr[i].join();

        }

        Reply response;

        response.ecg = 0;
        response.p = -1;

        for (int i = 0; i < h; i++) {

            response_buffer.push((char*) &response, sizeof(response));
        }

        for (int i = 0; i < h; i++) {

            histogramThr[i].join();
        }

        gettimeofday (&end, 0);

        hc.print ();

    }

    
    // print the results
    int secs = (end.tv_sec * 1e6 + end.tv_usec - start.tv_sec * 1e6 - start.tv_usec)/(int) 1e6;
    int usecs = (int)(end.tv_sec * 1e6 + end.tv_usec - start.tv_sec * 1e6 - start.tv_usec)%((int) 1e6);
    cout << "Took " << secs << " seconds and " << usecs << " micro seconds" << endl;


    MESSAGE_TYPE q = QUIT_MSG;
    chan->cwrite ((char *) &q, sizeof (MESSAGE_TYPE));
    cout << "All Done!!!" << endl;
    delete chan;
    
}