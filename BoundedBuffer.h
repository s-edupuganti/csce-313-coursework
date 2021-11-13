#ifndef BoundedBuffer_h
#define BoundedBuffer_h

#include <stdio.h>
#include <queue>
#include <string>
#include <thread>
#include <condition_variable>
#include <mutex>

using namespace std;

class BoundedBuffer
{
private:
	int cap; // max number of items in the buffer
	queue<vector<char>> q;	/* the queue of items in the buffer. Note
	that each item a sequence of characters that is best represented by a vector<char> for 2 reasons:
	1. An STL std::string cannot keep binary/non-printables
	2. The other alternative is keeping a char* for the sequence and an integer length (i.e., the items can be of variable length).
	While this would work, it is clearly more tedious */

	// add necessary synchronization variables and data structures 

	mutex mut;

	condition_variable data_cv;
	condition_variable slot_cv;


public:
	BoundedBuffer(int _cap){

		cap = _cap;

	}
	~BoundedBuffer(){

	}

	void push(char* data, int len){

		//1. Wait until there is room in the queue (i.e., queue lengh is less than cap)
		unique_lock<mutex> lock(mut);
		
		// while (q.size() == cap) {
		// 	cv1.wait(lock);
		// }

		slot_cv.wait(lock, [this]{return q.size() < cap;});
		

		//2. Convert the incoming byte sequence given by data and len into a vector<char>
		vector<char> incByteSeq (data, data + len);

		//3. Then push the vector at the end of the queue
		q.push(incByteSeq);

		lock.unlock();

		data_cv.notify_one();
	}

	int pop(char* buf, int bufcap){
		//1. Wait until the queue has at least 1 item
		unique_lock<mutex> lock(mut);

		// while (q.size() <= 0) {
		// 	cv2.wait(lock);
		// }

		data_cv.wait(lock, [this]{return q.size() > 0 ;});

		//2. pop the front item of the queue. The popped item is a vector<char>
		vector<char> poppedItem = q.front();
		q.pop();
		lock.unlock();



		//3. Convert the popped vector<char> into a char*, copy that into buf, make sure that vector<char>'s length is <= bufcap
		if (poppedItem.size() > bufcap) {
			// cout << "ERROR: Vector length is greater than bufcap" << endl;
			throw invalid_argument("ERROR: Vector length is greater than bufcap!");
		}

		// for (int i = 0; i < poppedItem.size(); i++) {
		// 	buf[i] = poppedItem[i];
		// }

		memcpy(buf, poppedItem.data(), poppedItem.size());

		// reinterpret_cast<char*>(poppedItem.data);


		//4. Return the vector's length to the caller so that he knows many bytes were popped

		slot_cv.notify_one();
		
		return (poppedItem.size());
	}
};

#endif /* BoundedBuffer_ */
