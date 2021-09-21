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

using namespace std;


int main(int argc, char *argv[]){


	
	int opt;
	int p = 1;
	double t = 0.0;
	int e = 1;
	int d = 1;
	// fix-me
	int m = MAX_MESSAGE;
	// string c = "";

	bool isFile = false;
	bool makeNewChan = false;

	// bool isFile = fale;
	
	string filename = "";
	while ((opt = getopt(argc, argv, "p:t:e:f:m:c:")) != -1) {
		switch (opt) {
			case 'p':
				p = atoi (optarg);
				break;
			case 't':
				t = atof (optarg);
				break;
			case 'e':
				e = atoi (optarg);
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
				break;
			case 'c':
				makeNewChan = true;
				break;
		}
	}

	// int pid = fork();

	// if (pid == 0) {

	// } else {


		FIFORequestChannel chan ("control", FIFORequestChannel::CLIENT_SIDE);


		if (t < 0) {

			std::ofstream myFile;

			myFile.open ("x1.csv");

			for (double i = 0; i < 4; i = i + 0.004) {

				char buf [MAX_MESSAGE];
				datamsg x (p, i, 1);
				chan.cwrite (&x, sizeof (datamsg)); // question
				double reply;
				int nbytes = chan.cread (&reply, sizeof(double)); //answer

				datamsg y (p, i, 2);
				chan.cwrite (&y, sizeof (datamsg)); // question
				double replyTwo;
				int nbytesTwo = chan.cread (&replyTwo, sizeof(double)); //answer

				myFile << i << "," << reply << "," << replyTwo << "\n"; 

			}

			myFile.close();

		} else {

			char buf [MAX_MESSAGE]; // 256
			datamsg x (p, t, e);

			chan.cwrite (&x, sizeof (datamsg)); // question
			double reply;
			int nbytes = chan.cread (&reply, sizeof(double)); //answer
			cout << "For person " << p <<", at time " << t << ", the value of ecg "<< e <<" is " << reply << endl;
		}

		if (isFile) {
			
			filemsg fm (0,0);
			string fname = filename;
			
			int len = sizeof (filemsg) + fname.size()+1;
			char buf2 [len];
			memcpy (buf2, &fm, sizeof (filemsg));
			strcpy (buf2 + sizeof (filemsg), fname.c_str());
			chan.cwrite (buf2, len);  // I want the file length;

			__int64_t fileLength;
			chan.cread(&fileLength, sizeof(__int64_t));
			cout << "File length is: " << fileLength << endl;



			int requests = ceil(fileLength / (1.0 * m));
			cout << "Number of requests is: " << requests << endl;

			filemsg msg(0, 0);

			int offset = 0;
			int length = m;

			ofstream requestFile;

			requestFile.open(("received/" + filename));


			for (int i = 0; i < requests; i++) {

					cout << "(" << offset << "," << length << ")" << endl;

					msg.offset = offset;
					msg.length = length;

					int len = sizeof (filemsg) + fname.size()+1;
					char buf2 [len];
					memcpy (buf2, &msg, sizeof (filemsg));
					strcpy (buf2 + sizeof (filemsg), fname.c_str());
					chan.cwrite (buf2, len);  // I want the file length;

					char x[length];

					requestFile.write(x, chan.cread(&x, length));

					offset += length;

					if (offset + length > fileLength) {
						length = fileLength - offset;
					}
			
			}

			requestFile.close();

		}

		if (makeNewChan) {

			MESSAGE_TYPE nc = NEWCHANNEL_MSG;
			chan.cwrite(&nc, sizeof(MESSAGE_TYPE));
			char ncChar[m];
			chan.cread(&ncChar, sizeof(ncChar));

			string channelName = ncChar;

			cout << "The channel name is: " << channelName << endl;

			FIFORequestChannel newChan (channelName, FIFORequestChannel::CLIENT_SIDE);

			datamsg dm1 (5, 1, 2);

			newChan.cwrite (&dm1, sizeof (datamsg)); // question
			double testOutput1;
			int nbytes1 = newChan.cread (&testOutput1, sizeof(double)); //answer
			cout << testOutput1 << endl;

			datamsg dm2 (10, 0.36, 1);

			newChan.cwrite (&dm2, sizeof (datamsg)); // question
			double testOutput2;
			int nbytes2 = newChan.cread (&testOutput2, sizeof(double)); //answer
			cout << testOutput2 << endl;


			
		}

		

		
		// closing the channel    
		MESSAGE_TYPE q = QUIT_MSG;
		chan.cwrite (&q, sizeof (MESSAGE_TYPE));
	// }
}
