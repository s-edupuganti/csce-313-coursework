#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include "common.h"
#include "TCPreqchannel.h"

// #include <typeinfo>

using namespace std;

void remSubstr(string &str, string &substr) {

    size_t substrPos = str.find(substr);

    if (substrPos != string::npos) {
        str.erase(substrPos, substr.length());
    }
}

int main(int argc, char** argv) {

    // cout << "The type of the URL Input is: " << typeid(argv[1]).name() << endl;

    string URL = argv[1];
    string filename;
    string hostname;
    string path;

    if (argv[2] == NULL) {
        cout << "No second arguement." << endl;
    } else {
        filename = argv[2];
        cout << "The filename is: " << filename << endl;
    }

    cout << "The URL is: " << URL << endl;

    size_t httpFound = URL.find("://");
    size_t domExtFound = URL.find(".org");

    // if (URL.find("#")) {

    // }

    httpFound = httpFound + 3;
    domExtFound = domExtFound + 4;

    int hostnameChar = domExtFound - httpFound;

    cout << "Position of :// is: " << httpFound << endl;
    cout << "Position of .org is: " << (domExtFound) << endl;

    hostname = URL.substr(httpFound, hostnameChar);

    // cout << "Last Char in URL: " << URL[domExtFound-1] << endl;

    if (URL[domExtFound] == NULL || URL[domExtFound] == '/') {

        path = "/";

        // URL = URL + '/';
        // cout << "The NEW URL is: " << URL << endl;


        cout << "The hostname is: " << hostname << endl;
        cout << "The path is: " << path << endl;

    } else {
        cout << "Still working on this part!" << endl;
    }


    // struct addrinfo hints, *res;

    TCPRequestChannel* chan = new TCPRequestChannel(hostname, "80");

    string request = "GET " + path + " HTTP/1.1\r\nHost: " + hostname + "\r\nConnection: close\r\n\r\n";





    

    



    // cout << "Is Char Empty?: " << URL[domExtFound] << endl;

    // URL[domExtFound] = '/';

    //  cout << "Is Char Emptyv2.0?: " << URL[domExtFound] << endl;

    // if (URL[domExtFound] == "") {
    //     URL[domExtFound] = '/';
    //     cout << "The New URL is: " << URL << endl;
    // }





    // remSubstr(argv[1], ".org");




    

}