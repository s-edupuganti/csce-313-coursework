/*
    Tanzir Ahmed
    Department of Computer Science & Engineering
    Texas A&M University
    Date  : 2/8/20
	Original author of the starter code
	
	Please include your name and UIN below
	Name: Sidharth Edupuganti
	UIN: 628004560
 */
#include "common.h"
#include "FIFOreqchannel.h"
#include "MQreqchannel.h"
#include <sys/wait.h>
#include <chrono>

using namespace std;
typedef std::chrono::high_resolution_clock Clock;

int main(int argc, char *argv[]){

	double t = 0.0;

	string setIPCMethod = "f";

	int opt;
	int p = 1;
	int e = 1;
	int d = 1;
	int m = MAX_MESSAGE;

	bool isFile = false;
	bool makeNewChan = false;
	bool patientGiven = false;
	bool timeGiven = false;
	bool ecgGiven = false;
	bool bufferGiven = false;

	auto startTime = Clock::now();
	auto endTime = Clock::now();
	
	string filename = "";
	while ((opt = getopt(argc, argv, "p:t:e:f:m:c:i:")) != -1) {
		switch (opt) {
			case 'p':
				p = atoi (optarg);
				patientGiven = true;
				break;
			case 't':
				t = atof (optarg);
				timeGiven = true;
				break;
			case 'e':
				e = atoi (optarg);
				ecgGiven = true;
				break;
			case 'f':
				filename = optarg;
				isFile = true;
				break;
			case 'd':
				d = atoi (optarg);
				break;
			case 'm':
				m = atoi (optarg);
				bufferGiven = true;
				break;
			case 'c':
				makeNewChan = true;
				break;
			case 'i':
				setIPCMethod = optarg;
				break;
		}	
	}

	pid_t process = fork();

	if (process == 0) {

			char *argv[] = {"./server", "-m", (char*)to_string(m).c_str(), NULL};
        	execvp(argv[0], argv);		

	} else {

		RequestChannel* chan;

		if (setIPCMethod == "f") {
			chan = new FIFORequestChannel ("control", RequestChannel::CLIENT_SIDE);
		} else if (setIPCMethod == "q") {
			chan = new MQRequestChannel ("control", RequestChannel::CLIENT_SIDE);
			cout << "USING MQREQCHANNEL!" << endl;
		} else if (setIPCMethod == "s") {


		}


		// FIFORequestChannel chan ("control", FIFORequestChannel::CLIENT_SIDE);



		if (t < 0) {


			std::ofstream myFile;

			myFile.open ("x1.csv");

			// cout << "Requesting 1000 datapoints...";

			startTime = Clock::now();


			for (double i = 0; i < 4; i = i + 0.004) {

				char buf [MAX_MESSAGE];
				datamsg x (p, i, 1);
				chan->cwrite (&x, sizeof (datamsg)); // question
				double reply;
				int nbytes = chan->cread (&reply, sizeof(double)); //answer

				datamsg y (p, i, 2);
				chan->cwrite (&y, sizeof (datamsg)); // question
				double replyTwo;
				int nbytesTwo = chan->cread (&replyTwo, sizeof(double)); //answer


				myFile << i << "," << reply << "," << replyTwo << "\n"; 

			}

			endTime = Clock::now();

			cout << "Amount of time to request 1000 datapoints: " << chrono::duration_cast<chrono::nanoseconds>(endTime - startTime).count() << " nanoseconds" << endl;

			myFile.close();

		} else if (patientGiven && timeGiven && ecgGiven && (t >= 0)) {

			// char buf [MAX_MESSAGE]; // 256
			datamsg x (p, t, e);

			chan->cwrite (&x, sizeof (datamsg)); // question
			double reply;
			chan->cread (&reply, sizeof(double)); //answer
			cout << "For person " << p <<", at time " << t << ", the value of ecg "<< e <<" is " << reply << endl;
		}

		if (isFile) {
			
			filemsg fm (0,0);
			string fname = filename;
			
			int len = sizeof (filemsg) + fname.size()+1;
			char buf2 [len];
			memcpy (buf2, &fm, sizeof (filemsg));
			strcpy (buf2 + sizeof (filemsg), fname.c_str());
			chan->cwrite (buf2, len);  // I want the file length;

			__int64_t fileLength;
			chan->cread(&fileLength, sizeof(__int64_t));
			cout << "File length is: " << fileLength << endl;



			int requests = ceil(fileLength / (1.0 * m));
			cout << "Number of requests is: " << requests << endl;

			filemsg msg(0, 0);

			int offset = 0;
			int length = m;

			ofstream requestFile;

			requestFile.open(("received/" + filename));

			startTime = Clock::now();


			for (int i = 0; i < requests; i++) {

					// cout << "(" << offset << "," << length << ")" << endl;

					msg.offset = offset;
					msg.length = length;

					int len = sizeof (filemsg) + fname.size()+1;
					char buf2 [len];
					memcpy (buf2, &msg, sizeof (filemsg));
					strcpy (buf2 + sizeof (filemsg), fname.c_str());
					chan->cwrite (buf2, len);  // I want the file length;

					char x[length];

					requestFile.write(x, chan->cread(&x, length));

					offset += length;

					if (offset + length > fileLength) {
						length = fileLength - offset;
					}
			
			}

			endTime = Clock::now();

			cout << "Amount of time to request file: " << chrono::duration_cast<chrono::nanoseconds>(endTime - startTime).count() << " nanoseconds" << endl;


			requestFile.close();



		}

		if (makeNewChan) {

			MESSAGE_TYPE nc = NEWCHANNEL_MSG;
			chan->cwrite(&nc, sizeof(MESSAGE_TYPE));
			char ncChar[m];
			chan->cread(&ncChar, sizeof(ncChar));

			string channelName = ncChar;

			cout << "The channel name is: " << channelName << endl;


			FIFORequestChannel newChan (channelName, FIFORequestChannel::CLIENT_SIDE);

			datamsg dm1 (5, 1, 2); // -0.1

			cout << "Testing data point for patient 5, at time of 1.00 and ecg 2..." << endl;

			newChan.cwrite (&dm1, sizeof (datamsg)); // question
			double testOutput1;
			int nbytes1 = newChan.cread (&testOutput1, sizeof(double)); //answer
			cout << testOutput1 << endl;

			datamsg dm2 (10, 0.36, 1); // -0.245

			cout << "Testing data point for patient 10, at time of 0.36 and ecg 1..." << endl;

			newChan.cwrite (&dm2, sizeof (datamsg)); // question
			double testOutput2;
			int nbytes2 = newChan.cread (&testOutput2, sizeof(double)); //answer
			cout << testOutput2 << endl;

			MESSAGE_TYPE closeNC = QUIT_MSG;
			newChan.cwrite(&closeNC, sizeof (MESSAGE_TYPE));
			// wait(0);

		}

		
		// closing the channel    
		MESSAGE_TYPE q = QUIT_MSG;
		chan->cwrite (&q, sizeof (MESSAGE_TYPE));
		
		wait(0);
	
	}

	// return 0;
}
