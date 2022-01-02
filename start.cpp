#include <stdio.h>
#include<iostream>
#include <vector>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctime>
#include <algorithm>

using namespace std;

void displayVect(vector<string>& vect) { // Display vector help function

    cout << endl;

    for (int i = 0; i < vect.size(); i++) {
        cout << vect[i] << endl;
    }

    cout << endl;

}

string trimSpace(string &str) { // Trim extra spaces

    string resultStr;

    for (int i = 0; i < str.length();) {
        if (str[i] == ' ') {
            if (i == str.length() || i == 0) {
                i++;
                continue;
            }

            while (str[i + 1] == ' ') {
                i++;
            }
        }
        resultStr += str[i++];
    }

    return resultStr;
}

bool isCharBetweenChars (string line, char charToFind, char begChar, char endChar) { // Function to find if character is between 2 other characters

    int starPos = 0;
    int endPos = 0;

    for (int i = 0; i < line.size(); i++) { // Find position of first character from beginning of string
        if (line[i] == begChar) {
            starPos = i;
            break;
        }
    }

    for (int i = line.size(); i >= 0; i--) { // Find position of character starting from end of string
        if (line[i] == endChar) {
            endPos = i;
            break;
        }
    }

    for (int i = starPos; i < endPos; i++) { // Return true if character is found between the two positions
        if (line[i] == charToFind) {
            return true;
        }
    }

    return false;

}

vector<string> partSplit (string line, char separator){ // Parts split function to seperate after pipeSplit is checked
	vector<string> result;

    int startQuo = 0;
    int endQuo = 0;
    int dollarPos;
    int endParaPos;
    int quotePos;

    string cutQuo = "";

    if (line.find('<') != string::npos) { // If < redirect is found, add space before and after the character

        if (line[line.find('<') + 1] != ' ') {
            line.insert(line.find('<') + 1, " ");
        }

        if (line[line.find('<') - 1] != ' ') {
            line.insert(line.find('<') - 1, " ");
        }
        
    }

    if (line.find('>') != string::npos) { // If > redirect is found, add space before and after the character

        if (line[line.find('>') + 1] != ' ') {
            line.insert(line.find('>') + 1, " ");
        }

        if (line[line.find('>') - 1] != ' ') {
            line.insert(line.find('>') - 1, " ");
        }        
    }

    if (line.find('\'') != string::npos || (line.find('\"') != string::npos)) { // Quotation Parser

        for (int i  = 0; i < line.size(); i++) { // Find starting position of first quote
            if (line[i] == '\'' || line[i] == '\"') {
                startQuo = i;
                break;
            }
        }

        for (int i = line.size(); i >= 0; i--) { // Find ending position of last quote
            if (line[i] == '\'' || line[i] == '\"') {
                endQuo = i;
                break;
            }
        }

        for (int i = startQuo; i <= endQuo; i++) { // Append everything between quotes (including quotes) into empty string
            cutQuo+=line[i];
        }

        quotePos = line.find(cutQuo);

        line.erase(quotePos, cutQuo.size()); // Erase substring of previous string

    }


	while (line.size()){ // Until the size of the line becomes 0

        
		size_t found = line.find_first_of (separator); // Find first instant of the char
        

		if (found!=std::string::npos){ // If char hasn't been found run if statement until found and push to results vector

			string part = line.substr(0, found);
			result.push_back(part);
			line = line.substr (found+1);
		}
		else{
			result.push_back (line); // If last char seperator in input line then break after one last push
			break;
		}
    }

    int counter = 0;

    if (cutQuo != "") { // If there was no quotations involved with the input line, then if statement does not happen


        for (int i = 0; i < result.size(); i++) {
            for (int j = 0; j < result[i].size(); j++) { // Iterate through vector to get to characters within strings
                counter++;

                
                if (counter == (quotePos - 1)) {

                    result.insert(result.begin() + (1+i), cutQuo); // Insert the substring that was cut from the inputline into the vector at the position it was cut from
                    break;
       
                }
            }
        }
    }



    const char *pathDirectory = getenv("PATH"); // Check if $PATH is typed


    for (int i = 0; i < result.size(); i++) {
        if (result[i] == "-e" && ((cutQuo[0] == '\'')|| (cutQuo[0] == '\"'))) {
            result.insert(result.begin() + (1 + i), cutQuo); // Checks if -e flag is inputted for echo function
        }

        if (result[i] == "$PATH" || result[i] == "\"PATH\"" || result[i] == "\'PATH\'") {
            result[i] = pathDirectory; // return $PATH directory
        }
    }

    
	return result;
}


vector<string> pipeSplit (string line, char separator){  // Split function for pipes
	vector<string> result;


    if (isCharBetweenChars(line, '|','\'','\'')) { // Check first if pipe is within quotations

        result = partSplit(trimSpace(line), ' ');  // Send function to partsplit

        vector<string> pipeResult;
        string tempStr = "";

        for (int i = 0; i < result.size(); i++) { // Create empty string to append to

            tempStr+=result[i] + ' ';

        }

        result.clear();
        result.push_back(tempStr); // Push back empty string

        return result;
    }

 
    if (isCharBetweenChars(line, '|','\"','\"')) {

        result = partSplit(trimSpace(line), ' '); 

        vector<string> pipeResult;
        string tempStr = "";

        for (int i = 0; i < result.size(); i++) {

            tempStr+=result[i] + ' ';

        }

        result.clear();
        result.push_back(tempStr);

        return result;
    }

	while (line.size()){

		size_t found = line.find_first_of (separator);
        

		if (found!=std::string::npos){

			string part = line.substr(0, found);
			result.push_back(part);
			line = line.substr (found+1);
		}
		else{
			result.push_back (line);
			break;
		}
    }
    
	return result;
}


int main (){

    time_t now = time(0);
    char* dt = ctime(&now);

    string dateTime = dt;

    dup2(0, 10);


    dateTime.erase(remove(dateTime.begin(), dateTime.end(), '\n'), dateTime.end());

    vector<string> parts;
    vector<int> process;

    const char *homeDirectory = getenv("USER");

    char* directory[1024];

    while (true){


        char directory[1024];

        dup2(10, 0);

        cout << homeDirectory << ":" << dateTime << "$ "; // Shell Input Prompt

        string inputline;
        getline (cin, inputline);   // get a line from standard input

        string combinedStr = "";

        char doubleQuo = '\"';
        char singleQuo = '\'';


        for (int i = 0; i < process.size(); i++) { // Background process check
            if(waitpid(process[i], 0, WNOHANG) == process[i]) {
                process.erase(process.begin() + i);
                i--;
            }
        }

        bool isBackgroundExist = false;

        if (inputline == string("exit")){ // Exit input
            cout << "Bye!! End of shell" << endl;
            break;
        } else {

            int dollarPos;
            int endParaPos;

            string emptyStr = "";

            // for (int i = 0; i < line.size(); i++) {
            //     cout << line[i] << " ";
            // }

            // cout << endl;

            if (inputline.find('$(') != string::npos) { // Attempt for dollar sign expansion

                dollarPos = (inputline.find('$(')) + 1;

                for (int i = dollarPos; inputline.size(); i++) {
                    if (inputline[i] == ')') {
                        endParaPos = i;
                        break;
                    }
                }

                for (int i = dollarPos; i < endParaPos; i++) {
                    emptyStr+=inputline[i];
                    
                }


                inputline = emptyStr;


            }
            
        
            vector<string> pipeVect = pipeSplit(trimSpace(inputline), '|'); // First pipeSplit function call within main


            for (int i = 0; i < pipeVect.size(); i++) { // For loop for the amount of pipes found


                int fd[2];
                pipe(fd);

                inputline = trimSpace(pipeVect[i]); // Set input line to first part of the pipeVector

                parts = partSplit(trimSpace(inputline), ' '); // Run parts within each part of pipesVector
          
                if (parts[parts.size() - 1] == "&") { // Check if & is used in sleep command

                    isBackgroundExist = true;
                    parts.erase(parts.begin() + (parts.size() - 1));
                }

                int pid = fork();

                if (!pid) { // Child process

                    if (parts[0] == "echo") {    // Quotes handler for echo                 

                        for (int j = 0; j < parts.size(); j++) {
                            if (parts[j][0] == '\"' || parts[j][0] == '\'') {

                                parts[j] = parts[j].substr(1, parts[j].size() - 2);
                                break;
                            }
                        }
                    }

                    if (parts[0] == "awk") { // Quotes handler for awk

                        for (int j = 0; j < parts.size(); j++) {
                            if (parts[j][0] == '\"' || parts[j][0] == '\'') {

                                parts[j] = parts[j].substr(1, parts[j].size() - 2);
                                break;
                            }
                        }
                    }

                    if (parts[0] == "cd") { // Change directory function

                        char tempDir[1024];
                    
                        if (parts.size() == 1) {
                            getcwd(directory, sizeof(directory));
                            chdir("/home");
                        } else if (parts[1] == "-") {

                            getcwd(tempDir, sizeof(tempDir));

                            chdir(directory);

                            for (int i = 0; i < (sizeof(tempDir)/sizeof(tempDir[0])); i++) {
                                directory[i] = tempDir[i];
                            }

                        } else {
                            getcwd(directory, sizeof(directory));
                            chdir(parts[1].c_str());

                        }
                    }

                    if (find(parts.begin(), parts.end(), ">") != parts.end()) { // > Redirection

                        int index = (find(parts.begin(), parts.end(), ">")) - parts.begin();

                        string fileName = parts[index + 1];

                        parts.erase(parts.begin() + index);
                        parts.erase(parts.begin() + index);

                        int fdDup = open(fileName.c_str(), O_WRONLY | O_CREAT | O_RDONLY, S_IWUSR | S_IRUSR);

                        dup2(fdDup, 1);
                        close(fdDup);

                    }

                    if (find(parts.begin(), parts.end(), "<") != parts.end()) { // < Redirection


                        int index = (find(parts.begin(), parts.end(), "<")) - parts.begin();

                        string fileName = parts[index + 1];

                        parts.erase(parts.begin() + index);
                        parts.erase(parts.begin() + index);

                        int fdDup = open(fileName.c_str(), O_RDONLY | O_CREAT, S_IWUSR | S_IRUSR);
                        dup2(fdDup, 0);
                        close(fdDup);
                    }

                    if (i < pipeVect.size() - 1) {
                        dup2(fd[1], 1);
                        close(fd[1]);
                    }
                    

                    char* args [parts.size() + 2] = {NULL}; // Constructor
            

                    for (int i = 0; i < parts.size(); i++) { // Populate args char array from parts vector
                        args[i] = (char *)parts.at(i).c_str();
                    }

                    execvp (args [0], args);

                } else {

                    if (isBackgroundExist == false) { // Check if BG process or not running
                        if (i == pipeVect.size() - 1) {
                            waitpid(pid, 0, 0); // suspends process until program finds child
                        } else {
                            process.push_back(pid);
                        }

                    } else {
                        process.push_back(pid);
                    }

                    dup2(fd[0], 0);
                    close(fd[1]); // Close file descriptor
                    
                }
            }  
        }
    }
}