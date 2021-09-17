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
    FIFORequestChannel chan ("control", FIFORequestChannel::CLIENT_SIDE);
	
	int opt;
	int p = 1;
	double t = 0.0;
	int e = 1;
	int d = 1;
	// fix-me
	int m = MAX_MESSAGE;
	
	string filename = "";
	while ((opt = getopt(argc, argv, "p:t:e:f:m:")) != -1) {
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
				break;
			case 'd':
				d = atoi (optarg);
			case 'm':
				m = atoi (optarg);
		}
	}
	
    // sending a non-sense message, you need to change this
    // char buf [MAX_MESSAGE]; // 256
    // datamsg x (p, t, e);
	// // datamsg y (1, 0, 1);

	// // cout << "For person " << p <<", at time " << t << ", the value of ecg "<< e << " is " << reply << endl;
	
	// chan.cwrite (&x, sizeof (datamsg)); // question
	// double reply;
	// int nbytes = chan.cread (&reply, sizeof(double)); //answer
	// cout << "For person " << p <<", at time " << t << ", the value of ecg "<< e <<" is " << reply << endl;
	// cout << "# of Datapoints: " << d << endl;

	if (t < 0) {

		std::ofstream myFile;

		// const char* path = "received"


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

	if (filename == "hello.csv") {
		
		filemsg fm (0,0);
		string fname = "hello.csv";
		
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

		requestFile.open("received/hello.csv");


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

	

	
	// closing the channel    
    MESSAGE_TYPE q = QUIT_MSG;
    chan.cwrite (&q, sizeof (MESSAGE_TYPE));
}
