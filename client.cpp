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
	
	string filename = "";
	while ((opt = getopt(argc, argv, "p:t:e:f:")) != -1) {
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

	if (filename == "test.csv") {
		
		filemsg fm (0,0);
		string fname = "test.csv";
		
		int len = sizeof (filemsg) + fname.size()+1;
		char buf2 [len];
		memcpy (buf2, &fm, sizeof (filemsg));
		strcpy (buf2 + sizeof (filemsg), fname.c_str());
		chan.cwrite (buf2, len);  // I want the file length;

		__int64_t fileLength;

		chan.cread(&fileLength, sizeof(__int64_t));

		cout << "Fil length is: " << fileLength << endl;



		


	}

	
	

	
	// _int64_t fileLength = chan.cwrite (buf2, len);

	
	// closing the channel    
    MESSAGE_TYPE m = QUIT_MSG;
    chan.cwrite (&m, sizeof (MESSAGE_TYPE));
}
