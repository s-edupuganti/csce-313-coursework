#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include "common.h"
#include "TCPreqchannel.h"

using namespace std;

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

    ofstream output;

    int nbytes;
    bool begin = false;
    // while (true) {
    //         nbytes = chan->cread(&msg, 255);

    //         if (nbytes == 0) {
    //             break;
    //         }

        //     string s;
        //     if (nbytes < 255) {
        //         s = "";
        //         for (int i = 0; i < nbytes; i++) {
        //             if (msg[i] == '<') {
        //                 begin = true;
        //             }
        //             s += msg[i];
        //         }
        //     } else {
        //         s = string(msg);
        //         if (s.find('<') != string::npos) {
        //             if (!begin) {
        //                 int index = s.find('<');
        //                 s = s.substr(index);
        //                 begin = true;
        //             }
        //         }
        //     }

        // if (begin) {
        //     output.open(filename, std::ios::app);
        //     output << s;
        //     output.close();
        // }
    // }

    bool foundStatusCode = false;



    while (true) {

        nbytes = chan->cread(&msg, 255);

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


                // delete chan;
                handleStatusCode(url, filename);
                break;
            } else if (statusCode == '4') {
                cout << "4XX STATUS CODE" << endl;
                exit(1);
            }
 
        }

        cout << "2XX STATUS CODE" << endl;


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