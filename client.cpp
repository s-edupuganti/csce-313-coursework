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
#include "SHMreqchannel.h"

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
	int c = 1;
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
				c += atoi (optarg);
				break;
			case 'i':
				setIPCMethod = optarg;
				break;
		}	
	}

	pid_t process = fork();

	if (process == 0) {

			char *argv[] = {"./server", "-m", (char*)to_string(m).c_str(), "-i", (char *) setIPCMethod.c_str(), NULL};
        	execvp(argv[0], argv);		

	} else {

		RequestChannel* chan;

		if (setIPCMethod == "f") {
			chan = new FIFORequestChannel ("control", RequestChannel::CLIENT_SIDE);
			// cout << "USING FIFOREQCHANNEL!" << endl;
		} else if (setIPCMethod == "q") {
			chan = new MQRequestChannel ("control", RequestChannel::CLIENT_SIDE, m);
			// cout << "USING MQREQCHANNEL!" << endl;
		} else if (setIPCMethod == "s") {
			chan = new SHMRequestChannel ("control", RequestChannel::CLIENT_SIDE, m);
			// cout << "USING SHMREQCHANNEL!" << endl;
		}


		vector<RequestChannel*> channels;
		channels.reserve(c);
		channels.push_back(chan);

		for (int i = 1; i < c; i++) {

			RequestChannel* newChan;

			if (makeNewChan) {

				MESSAGE_TYPE nc = NEWCHANNEL_MSG;
				chan->cwrite(&nc, sizeof(MESSAGE_TYPE));
				char ncChar[m];
				chan->cread(&ncChar, sizeof(ncChar));

				string channelName = ncChar;

				cout << "Created channel: " << channelName << endl;

				// RequestChannel


				if (setIPCMethod == "f") {
					newChan = new FIFORequestChannel (channelName, RequestChannel::CLIENT_SIDE);
				} else if (setIPCMethod == "q") {
					newChan = new MQRequestChannel (channelName, RequestChannel::CLIENT_SIDE, m);
					// cout << "USING MQREQUEST" << endl;
				} else if (setIPCMethod == "s") {
					newChan = new SHMRequestChannel ("control", RequestChannel::CLIENT_SIDE, m);
				}

			}

			channels.push_back(newChan);

		}


		cout << "Channel vector size is: " << channels.size() << endl;
		
		for (int i = 0; i < channels.size(); i++) {

			if (t < 0 || (!(timeGiven) && patientGiven && ecgGiven)) {

				std::ofstream myFile;

				myFile.open ("received/x1.csv");

				cout << "Requesting 1000 datapoints...";

				startTime = Clock::now();

				for (double j = 0; j < 4; j+=0.004) {

					char buf [m];
					datamsg x (p, j, 1);
					channels[i]->cwrite (&x, sizeof (datamsg)); // question
					double reply;
					int nbytes = channels[i]->cread (&reply, sizeof(double)); //answer

					datamsg y (p, j, 2);
					channels[i]->cwrite (&y, sizeof (datamsg)); // question
					double replyTwo;
					int nbytesTwo = channels[i]->cread (&replyTwo, sizeof(double)); //answer


					myFile << j << "," << reply << "," << replyTwo << "\n"; 

				}

				endTime = Clock::now();

				cout << "Amount of time to request 1000 datapoints for Channel " << i << ": " << chrono::duration_cast<chrono::nanoseconds>(endTime - startTime).count() << " nanoseconds" << endl;

				myFile.close();

			} else if (patientGiven && timeGiven && ecgGiven && (t >= 0)) {

				// char buf [MAX_MESSAGE]; // 256
				datamsg x (p, t, e);

				channels[i]->cwrite (&x, sizeof (datamsg)); // question

				double reply;
				channels[i]->cread (&reply, sizeof(double)); //answer
				cout << "For person " << p <<", at time " << t << ", the value of ecg "<< e <<" is " << reply << endl;
			}

			
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

			__int64_t scBytes = ceil(fileLength / (1.0 * c));

			cout << "Each channel should get: " << scBytes << endl;


			filemsg msg(0, 0);

			int offset = 0;
			int length = m;

			ofstream requestFile;

			requestFile.open(("received/" + filename));



			for (int i = 0; i < channels.size(); i++) {

				startTime = Clock::now();


				if (i == (channels.size() - 1)) {
					__int64_t tempSCBytes = scBytes;
					scBytes = fileLength - (tempSCBytes * (channels.size() - 1));
					// cout << "FINAL SCBYTES: " << scBytes << endl; 
					
				}

				int requests = ceil(scBytes / (1.0 * m));
				cout << "Number of requests is: " << requests << endl;

				for (int j = 0; j < requests; j++) {

					msg.offset = offset;
					msg.length = length;


					int len = sizeof (filemsg) + fname.size()+1;
					char buf2 [len];
					memcpy (buf2, &msg, sizeof (filemsg));
					strcpy (buf2 + sizeof (filemsg), fname.c_str());
					channels[i]->cwrite (buf2, len);  // I want the file length;

					char x[length];

					requestFile.write(x, channels[i]->cread(&x, length));

					offset += length;

					if (offset + length > fileLength) {

						length = fileLength - offset;

					}
				}

				endTime = Clock::now();
				cout << "Amount of time to request file for Channel " << i << ": " << chrono::duration_cast<chrono::nanoseconds>(endTime - startTime).count() << " nanoseconds" << endl;

			}

			cout << "File Transfer Done!" << endl;

			requestFile.close();

		}

		// cout << "Channel vector size POST TRANSFER is: " << channels.size() << endl;

		// closing the channel besides control    
		MESSAGE_TYPE q = QUIT_MSG;

		for (int i = 1; i < channels.size(); i++) {
			channels[i]->cwrite (&q, sizeof (MESSAGE_TYPE));
			delete channels[i];
		}

		chan->cwrite (&q, sizeof (MESSAGE_TYPE)); // closing control
		delete chan;
		
		wait(0);

	// return 0;
	}

}
