
#include "common.h"
#include "BoundedBuffer.h"
#include "Histogram.h"
#include "common.h"
#include "HistogramCollection.h"
#include "TCPreqchannel.h"
#include <string>

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

void worker_thread_function(BoundedBuffer* reqBuf, BoundedBuffer* respBuf, TCPRequestChannel* newWChan, int bufferCap){
    
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

            char buf[bufferCap];

            filemsg* fm = (filemsg*) request;

            string fname = (char*) (fm + 1);
            string filename = "received/" + fname;

            newWChan->cwrite(request, sizeof(filemsg) + fname.size() + 1);
            newWChan->cread(buf, bufferCap);

            __int_least64_t offset = fm->offset;
            __int_least64_t len = fm->length;

            FILE* pFile = fopen(filename.c_str(), "r+");

            fseek(pFile, offset, SEEK_SET);
            fwrite(buf, 1, len, pFile);
            fclose(pFile);

        }
    }


}

void file_thread_function (BoundedBuffer* reqBuf, TCPRequestChannel* chan, int bufCap, string filename){

    char request[1024];

    filemsg fm(0, 0);

    __int64_t fileLength;
    __int64_t remFileLength;

    string outputFile = "received/" + filename;

    memcpy(request, &fm, sizeof(fm));
    strcpy(request + sizeof(fm), filename.c_str());

    chan->cwrite(request, sizeof(fm) + filename.size() + 1);
    chan->cread(&fileLength, sizeof(fileLength));

    FILE* pFile = fopen(outputFile.c_str(), "w");
    fseek(pFile, fileLength, SEEK_SET);
    fclose(pFile);

    filemsg* fm_ = (filemsg*) request;

    remFileLength = fileLength;

    while (remFileLength > 0) {

        if (remFileLength < (__int64_t) bufCap) {
            fm_->length = remFileLength;
        } else {
            fm_->length = (__int64_t) bufCap;
        }

        reqBuf->push(request, sizeof(filemsg) + outputFile.size() + 1);

        fm_->offset += fm_->length;
        remFileLength -= fm_->length;

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
    int h = 5;          // default histogram
	int m = MAX_MESSAGE; 	// default capacity of the message buffer

    string filename = "";
    string hostName = "";
    string port = "";

    bool isFile = false;

    srand(time_t(NULL));

    while ((opt = getopt(argc, argv, "n:p:w:b:m:h:f:r:a:")) != -1) {

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
            case 'r':
                port = optarg;
                break;
            case 'a':
                hostName = optarg;
                break;

        }
    }
    
    
    // int pid = fork();
    // if (pid == 0){
	// 	// modify this to pass along m
    //     char *argv[] = {"./server", "-m", (char*)to_string(m).c_str(), NULL};
    //     execvp(argv[0], argv);	
    // }
    
	TCPRequestChannel* chan = new TCPRequestChannel(hostName, port);
    TCPRequestChannel* newWChans[w];

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

        Histogram* hist = new Histogram (12, -3.0, 3.0);
        hc.add(hist);

    }

    cout << "Histograms Created" << endl;

    cout << "Creating Worker Channels...";

    for (int i = 0; i < w; i++) { // Worker Channels

        // MESSAGE_TYPE nc = NEWCHANNEL_MSG;
        // chan->cwrite(&nc, sizeof(nc));
        // char ncChar[1024];
        // chan->cread(&ncChar, 1024);
        newWChans[i] = new TCPRequestChannel (hostName, port);

    }

    cout << "Worker Channels Created" << endl;


    thread fileThr;

    if (isFile) {

        cout << "File transfer starting!" << endl;

        fileThr = thread (file_thread_function, &request_buffer, chan, m, filename);

        cout << "File Thread Created" << endl;

        cout << "Creating Worker Threads...";

        for (int i = 0; i < w; i++) {

            workerThr[i] = thread (worker_thread_function, &request_buffer, &response_buffer, newWChans[i], m);

        }

        cout << "Worker Threads Created" << endl;

        fileThr.join();

        cout << "File Thread Joined" << endl;

        for (int i = 0; i < w; i++) {

            MESSAGE_TYPE quit = QUIT_MSG;

            request_buffer.push ((char*) &quit, sizeof (MESSAGE_TYPE));
        }

        for (int i = 0; i < w; i++) {

            workerThr[i].join();

        }

        cout << "Worker Thread Joined" << endl;

        gettimeofday (&end, 0);

    } else { // if not file transfer

        cout << "Creating Patient Threads...";

        for (int i = 0; i < p; i++) {
            patientThr[i] = thread (patient_thread_function, &request_buffer, i + 1, n);
        }

        cout << "Patient Threads Created" << endl;

        cout << "Creating Worker Threads...";

        for (int i = 0; i < w; i++) {

            workerThr[i] = thread (worker_thread_function, &request_buffer, &response_buffer, newWChans[i], m);

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

        cout << "Patient Threads Joined" << endl;

        for (int i = 0; i < w; i++) {

            MESSAGE_TYPE quit = QUIT_MSG;

            request_buffer.push ((char*) &quit, sizeof (MESSAGE_TYPE));
        }

        for (int i = 0; i < w; i++) {

            workerThr[i].join();

        }

        cout << "Worker Threads Joined" << endl;

        Reply response;

        response.ecg = 0;
        response.p = -1;

        for (int i = 0; i < h; i++) {

            response_buffer.push((char*) &response, sizeof(response));
        }

        for (int i = 0; i < h; i++) {

            histogramThr[i].join();
        }

        cout << "Histogram Threads Joined" << endl;

        gettimeofday (&end, 0);

        cout << "Final HistogramOutput: " << endl;
        cout << endl;

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