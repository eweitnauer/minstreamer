/* A simple server in the internet domain using UDP
   The port number is passed as an argument */
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

const string commands[] = {"set client list", "add client", "remove client", "clear all", "exit"};
const int command_count = 5;

void error(char *msg)
{
    perror(msg);
    exit(1);
}

/**
 * Removes trailing \n's.
 */
string chomp(string str) {
	int pos = str.find_last_not_of("\n");
	if (pos > -1) return str.substr(0,pos+1);
	return str;
}

/**
 * Removes leading and trailing ' ' and \t 's.
 */
string strip(string str) {
	int pos = str.find_last_not_of(" \t");
	if (pos > -1) str = str.substr(0,pos+1);
	pos = str.find_first_not_of(" \t");
	if (pos > -1) str = str.substr(pos);
	return str;
}

/**
 * Returns false if the program should exit.
 */
bool parse_message(string s) {
  int pos;
  string params;
  for (int i=0; i<command_count; i++) {
	  pos = s.find(commands[i]);
	  if (pos >= 0) {
	  	if (commands[i] == "exit") {
	  	  cout << "Received exit command. Exiting..." << endl;
	  	  return false;
	  	}
	  	cout << "Found command: " << commands[i] << endl;
	  	cout << "Parameter: '" << strip(s.substr(pos+commands[i].length())) << "'" << endl;
	  	return true;
	  }
  }
  cout << "Did not recognize command." << endl;
  return true;
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno, clilen;
     char buffer[256];
     struct sockaddr_in serv_addr;
		 int n;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_DGRAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     while (1) {
     		bzero(buffer,256);
     		n = recv(sockfd, buffer, 255, 0);
     		if (n < 0) error("ERROR reading from socket");
     		string s = chomp(buffer);
     		cout << "Received '" << s << "', parsing..." << endl;	
     		if (!parse_message(s)) break;
     }
     return 0; 
}
