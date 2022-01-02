#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include "common.h"
#include "TCPreqchannel.h"

using namespace std;

int convert(char num[]) {
   int len = strlen(num);
   int base = 1;
   int temp = 0;
   for (int i=len-1; i>=0; i--) {
      if (num[i]>='0' && num[i]<='9') {
         temp += (num[i] - 48)*base;
         base = base * 16;
      }
      else if (num[i]>='A' && num[i]<='F') {
         temp += (num[i] - 55)*base;
         base = base*16;
      }
   }
   return temp;
}

void handleStatusCode(string newURL, string keepFilename) {

    string URL = newURL;
    string filename = keepFilename;
    string hostname;
    string path;

    cout << "The URL is: " << URL << endl;

    size_t httpFound = URL.find("://");
    size_t domExtFound = URL.find(".org");

    httpFound = httpFound + 3;
    domExtFound = domExtFound + 4;

    int hostnameChar = domExtFound - httpFound;

    cout << "Position of :// is: " << httpFound << endl;
    cout << "Position of .org is: " << (domExtFound) << endl;

    hostname = URL.substr(httpFound, hostnameChar);

    if (URL[domExtFound+1] == NULL) {

        path = "/";

        cout << "The hostname is: " << hostname << endl;
        cout << "The path is: " << path << endl;

    } else {

        path = URL.substr(domExtFound);

        cout << "The hostname is: " << hostname << endl;
        cout << "The path is: " << path << endl;
    }

    TCPRequestChannel* chan = new TCPRequestChannel(hostname, "80");

    string request = "GET " + path + " HTTP/1.1\r\nHost: " + hostname + "\r\nConnection: close\r\n\r\n";

    chan->cwrite((char*)request.c_str(), request.size());
    char msg[256];
    msg[255] = '\0';

    ofstream output;

    int nbytes;
    bool begin = false;
    bool foundStatusCode = false;

    while (true) {

        nbytes = chan->cread(&msg, 255);

        if (!foundStatusCode) {
            string firstLine(msg);
            char statusCode = firstLine[firstLine.find("HTTP/1.1") + 9];
            foundStatusCode = true;
            // cout << endl;
            // cout << firstLine << endl;
            // cout << endl;

            if (statusCode == '3') {
                cout << "3XX STATUS CODE" << endl;
                stringstream ss(firstLine);
                string line;
                string url = "";
                while (getline(ss, line)) {
                    if (line.find("location") != string::npos) {
                        url = line.substr((line.find("location") + 10));
                        cout << "url: " << url << endl;
                    }
                }
                break;
            } else if (statusCode == '4') {
                cout << "4XX STATUS CODE" << endl;
                exit(1);
            } else {
                cout << "2XX STATUS CODE" << endl;
            }
        }

        if (nbytes == 0) {
            break;
        }

            string s;
            if (nbytes < 255) {
                s = "";
                for (int i = 0; i < nbytes; i++) {
                    if (msg[i] == '<') {
                        begin = true;
                    }
                    s += msg[i];
                }
            } else {
                s = string(msg);
                if (s.find('<') != string::npos) {
                    if (!begin) {
                        int index = s.find('<');
                        s = s.substr(index);
                        begin = true;
                    }
                }
            }

        if (begin) {
            output.open(filename, std::ios::app);
            output << s;
            output.close();
        }

    }
}


int main(int argc, char** argv) {

    string URL = argv[1];
    string filename;
    string hostname;
    string path;

    if (argv[2] == NULL) {
        filename = "index";
    } else {
        filename = argv[2];
        cout << "The filename is: " << filename << endl;
    }

    cout << "The URL is: " << URL << endl;

    size_t httpFound = URL.find("://");
    size_t domExtFound = URL.find(".org");

    httpFound = httpFound + 3;
    domExtFound = domExtFound + 4;

    int hostnameChar = domExtFound - httpFound;

    cout << "Position of :// is: " << httpFound << endl;
    cout << "Position of .org is: " << (domExtFound) << endl;

    hostname = URL.substr(httpFound, hostnameChar);

    if (URL[domExtFound+1] == NULL) {

        path = "/";

        cout << "The hostname is: " << hostname << endl;
        cout << "The path is: " << path << endl;

    } else {
        // cout << "Still working on this part!" << endl;

        path = URL.substr(domExtFound);

        cout << "The hostname is: " << hostname << endl;
        cout << "The path is: " << path << endl;
    }


    // struct addrinfo hints, *res;

    TCPRequestChannel* chan = new TCPRequestChannel(hostname, "80");

    string request = "GET " + path + " HTTP/1.1\r\nHost: " + hostname + "\r\nConnection: close\r\n\r\n";

    chan->cwrite((char*)request.c_str(), request.size());
    // char msg[MAX_LEN];
    char msg[256];
    msg[255] = '\0';

    // char msgTest[15000];
    // msgTest[14999] = '\0';
    // int nbytes2 = chan->cread(&msgTest, 14999);

    // for (int i = 0; i < nbytes2; i++) {
    //     cout << msgTest[i];
    // }

    // cout << endl;

    

    ofstream output;

    int nbytes;
    bool begin = false;
    bool foundStatusCode = false;
    bool isChunked = false;

    int i = 0;

    int totalNum = 0;
    int value = 0;

    string testString;

    stringstream chunkOutput;


    while (true) {

        nbytes = chan->cread(&msg, 255);


        if (nbytes == 0) {
            break;
        }

        if (!foundStatusCode) {
            string firstLine(msg);
            char statusCode = firstLine[firstLine.find("HTTP/1.1") + 9];
            foundStatusCode = true;

            if (statusCode == '3') {
                cout << "3XX STATUS CODE" << endl;
                stringstream ss(firstLine);
                string line;
                string url = "";
                while (getline(ss, line)) {
                    if (line.find("location") != string::npos) {
                        url = line.substr((line.find("location") + 10));
                        if (url[url.size() - 1] == '\r' || url[url.size() - 1] == '\n') {
                            url.erase(url.size() - 1);
                        }
                        cout << "url: " << url << endl;
                    }
                }

                handleStatusCode(url, filename);
                // break;
            } else if (statusCode == '4') {
                cout << "4XX STATUS CODE" << endl;
                exit(1);
            } else {
                cout << "2XX STATUS CODE" << endl;
            }
 
        }

        int hexadecimal;
        int checkNBytes = 0;
        int chunkSize;
        // int total;



        for (int i = 0; i < nbytes; i++) {
            // if (msg[i] == '\r' && msg[i+1] == '\n' && msg[i+6] == '\r' && msg[i+7] == '\n') {
            //         cout << endl;
            //         cout << msg[i + 2];
            //         cout << msg[i + 3];
            //         cout << msg[i + 4];
            //         cout << msg[i + 5];

            //         cout << endl;

            //         string charStr;

            //         charStr.push_back(msg[i + 2]);
            //         charStr.push_back(msg[i + 3]);
            //         charStr.push_back(msg[i + 4]);
            //         charStr.push_back(msg[i + 5]);

            //         int stringLen = charStr.length();

            //         char hexa2Int[stringLen + 1];

            //         strcpy(hexa2Int, charStr.c_str());

            //         hexadecimal = convert(hexa2Int);

            //         cout << "Conversion to Int: " << hexadecimal << endl;
            //         total+=hexadecimal;
            // }

            if (msg[i] == '\r' && msg[i + 1] == '\n' && msg[i+2] == '\r' && msg[i+3] == '\n' && msg[i+8] == '\r') {
                string charStr;
               
                charStr.push_back(msg[i + 4]);
                charStr.push_back(msg[i + 5]);
                charStr.push_back(msg[i + 6]);
                charStr.push_back(msg[i + 7]);

                cout << "First hexadecimal: " << charStr << endl;

                int stringLen = charStr.length();

                char str2Hexa[stringLen + 1];

                strcpy(str2Hexa, charStr.c_str());

                hexadecimal = convert(str2Hexa);
                value = hexadecimal;
                chunkSize = hexadecimal;
                // value-=nbytes;
                cout << "Hexadecimal Conversion: " << chunkSize << endl;


                // cout << "hexadecimal form of first size: " << hexadecimal << endl;
                break;
                // break;
            }



            



            // if (msg[i] == '\n') {
            //     cout << endl;
            //     cout << "backslashN Yes" << endl;
            //     cout << "Char - 1: " << msg[i - 1] << endl;
            //     cout << "Char + 1: " << msg[i + 1] << endl;
            //     cout << endl;
            // }
            
        }

        // checkNBytes+=nbytes;

        if (!isChunked) {
            // chan->cread(&msg, 255);

            string firstLine(msg);
            
            // cout << endl;
            // cout << firstLine << endl;
            // cout << endl;
            // break;

            stringstream ss(firstLine);
            string line;
            while (getline(ss, line)) {
                if (line.find("transfer-encoding: chunked") != string::npos) {
                    isChunked = true;
                    // cout << line.find("transfer-encoding: chunked") << endl;
                }
            }

        }


        if (isChunked) {

            if (nbytes < 255) {
                for (int i = 0; i < nbytes; i++) {
                    testString.push_back(msg[i]);
                }
            } else {
                testString.append(msg);
            }
    
        } else {

            string s;
            if (nbytes < 255) {
                s = "";
                for (int i = 0; i < nbytes; i++) {
                    if (msg[i] == '<') {
                        begin = true;
                    }
                    s += msg[i];
                }
            } else {


                s = string(msg);
                if (s.find('<') != string::npos) {
                    if (!begin) {
                        int index = s.find('<');
                        s = s.substr(index);
                        begin = true;
                    }
                }
            }


            if (begin) {
                output.open(filename, std::ios::app | std::ios::binary);
                output << s;
                output.close();
            }
        }
        i++;
    }

    // cout << "total characters: " << totalNum << "ok" << endl;

    if (isChunked) {
        // cout << (chunkOutput.str())[44] << endl;

        // cout << testString.size() << endl;

        int firstDecPos = testString.find("\r\n\r\n") + 4;

        // cout << testString[firstDecPos] << endl;

        cout << firstDecPos << endl;

        int endline;

        for (int i = firstDecPos; i < firstDecPos + 10; i++) {

            if (testString[i] == '\r' && testString[i + 1] == '\n') {

                cout << i << endl;

                endline = i - firstDecPos;
                break;

            }
            
        }

        // cout << testString[endline - 2] << endl;

        // cout << "endline: " << endline << endl;

        string firstChunk = testString.substr(firstDecPos, endline);

        char str2Hexa[firstChunk.size() + 1];
        strcpy(str2Hexa, firstChunk.c_str());
        int firstChunkSize = convert(str2Hexa);

        // for (int i = firstDecPos; i <)

        

        cout << firstChunkSize << endl;

        testString = testString.substr(firstDecPos + endline + 2);

        string finalString = testString.substr(0, firstChunkSize);

        output.open(filename, ios::app| std::ios::binary);
        output << finalString;
        output.close();

        // testString = testString.substr(firstChunkSize + 2);

        cout << "HERE" << endl;

        // for (int i = 0; i < 10; i++) {
        //     cout << testString[i];
        // }
        // cout << endl;

        // exit(0);

        while (true) {

            // if (testString.empty()) {
            //     break;
            // }

            testString = testString.substr(firstChunkSize + 2);

            for (int i = 0; i < 5; i++) {
                if (testString[i] == '\r' && testString[i + 1] == '\n') {
                    // cout << i << endl;
                    endline = i;
                    break;

                }
                
            }

            firstChunk = testString.substr(0, endline);

            // cout << firstChunk << endl;

            char str2Hexa2[firstChunk.size() + 1];
            strcpy(str2Hexa2, firstChunk.c_str());
            firstChunkSize = convert(str2Hexa2);

            if (firstChunkSize == 0) {
                cout << "NO" << endl;
                break;
            }

            cout << "IN WHILE LOOP: " << firstChunkSize << endl;

            testString = testString.substr(endline + 2);
            finalString = testString.substr(0, firstChunkSize); 

            output.open(filename, ios::app| std::ios::binary);
            output << finalString;
            output.close();

        }
    }
}