#include <bits/stdc++.h>
using namespace std;

#include <stdio.h> 
#include <unistd.h>
#include <stdlib.h>
#include <string.h> 
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <sys/stat.h> 
#include <fcntl.h>
#include <sys/time.h> 
#include <errno.h> 
#include <sys/select.h> 

#define MAX_BUFFER_SIZE 1024
# define BUFSIZE 1024

//The ip address used to check the code : 192.168.100.13

void error_msg(string str) {
   char * msg = new char[str.length() + 1];
   strcpy(msg, str.c_str());
   perror(msg);
   exit(1);
}

struct detail {
   char * ipaddress;
   char * portno;
   detail(string ip, string port) {
      ipaddress = new char[ip.length() + 1];
      strcpy(ipaddress, ip.c_str());

      portno = new char[port.length() + 1];
      strcpy(portno, port.c_str());
   }
};


pair <string, string> getPer_msg(char * buffer) {
   int length = strlen(buffer);
   bool flag = false;
   string person,message;
   for (int i = 0; i <length; i++) {
      if (flag == false) {
         if (buffer[i] == '/') {
            flag = true;
            continue;
         }
         person.push_back(buffer[i]);
      } else message.push_back(buffer[i]);
   }
   return make_pair(person, message);
}

char * getSendbuf(char* buf,string name){
    char output[BUFSIZE];
   int len = strlen(buf);
   bool done = false;
   string person,message;
   for (int i = 0; i <len; i++) {
      if (done == false) {
         if (buf[i] == '/') {
            done = true;
            message.push_back(buf[i]);
            continue;
         }
         person.push_back(buf[i]);
      } else message.push_back(buf[i]);
   }
   strcpy(buf,name.c_str());
   strcat(buf,message.c_str());
   buf[strlen(buf)]='\0';
   return buf;
}


int main(int argc, char * * argv) {

       map <string, detail> mp;
           ifstream myfile;
    myfile.open("users.txt");
   
   
    string name, iip, iport;
    while (myfile >> name) {
        myfile >> iip >> iport;
        detail det(iip, iport);
        mp.insert(make_pair(name, det));
    }
    myfile.close();
        printf("++++++++++++++++++Connected+++++++++++++++++++++++\n" );
    for (pair < string, detail> d : mp) {
        cout << d.first;
        cout << "(" << d.second.ipaddress << " : " << d.second.portno << " )" << endl; 
    }

	fd_set readset, tempset;
	int portno;
   	int maxfd, flags;
	socklen_t len;
	char iname[50];
	struct timeval tv;
	char buffer[MAX_BUFFER_SIZE + 1];
	struct sockaddr_in serveraddr;
	int serversock, peersock, j, result, result1, sent;
	char buf[BUFSIZE];

	if (argc != 2) {
	  fprintf(stderr, "usage: %s <port>\n", argv[0]);
	  exit(1);
	}
	portno = atoi(argv[1]);

	 
	// socket: creating the parent socket 
	serversock = socket (AF_INET, SOCK_STREAM, 0);
	if (serversock <0) error_msg("ERROR opening socket");
 
	// Eliminates "ERROR on binding: Address already in use" error. 
	
	int optval = 1;
	setsockopt (serversock, SOL_SOCKET, SO_REUSEADDR, (const void *) &optval, sizeof (int));
	
	// build the server's Internet address
	
	bzero((char *) &serveraddr, sizeof (serveraddr));
	//Internet address 
	serveraddr.sin_family = AF_INET;

	//IP address figured out
	serveraddr.sin_addr.s_addr = htonl (INADDR_ANY);

	//the port we will listen 
	serveraddr.sin_port = htons ((unsigned short) portno);

	 
	// bind: associate the parent socket with a port 
	
	if (bind (serversock, (struct sockaddr *) &serveraddr, sizeof (serveraddr)) <0)
	error_msg ("ERROR on binding");

	 
	// listen: make this socket ready to accept connection requests 
	
	// allow 5 requests
	if (listen(serversock, 5) <0) error_msg ("ERROR on listen");
	printf ("\nServer Running, start conversation.....\n");
	string myName;
	int checker=0;
	for (pair <string, detail> d: mp) {
		if (!strcmp (d.second.ipaddress ,"192.168.100.13") && atoi (d.second.portno) == atoi (argv[1])) {
			cout <<"You are " << d.first << "." << endl << "Start chating..." << endl;
			myName = d.first;
			checker = 1;
			break;
		}
	}
	if (checker == 0) {
		printf ("Your name is not in the list. Name need to be registered at 'ip=192.168.100.13' with port you are using.\n");
		exit(1);
	}

	
	FD_ZERO (&readset);
	FD_SET (serversock, &readset);
	FD_SET (0, &readset);
	maxfd = serversock;
	tv.tv_sec =3;
	tv.tv_usec = 0;
	bool exit1=false;
	do {
	  memcpy (&tempset, &readset, sizeof (tempset));
	  
	  result = select (maxfd + 1, &tempset, NULL, NULL, &tv);

	  if (result == 0) {
		 if (exit1 == true) {
			cout << "Do you want to quit chat?[y/n]:\t";
			char e;
			cin >> e;
			if(e == 'y') {
				exit(1);
			}
		 }
		 
	  } else if (result < 0 && errno != EINTR) {
	  		cout << "Error in select (): " << strerror (errno) << endl;
	  } else if (result > 0) {
		 if (FD_ISSET(serversock, &tempset)) {
			len = sizeof(serveraddr);
			peersock = accept (serversock, (struct sockaddr * ) &serveraddr, &len);
			if (peersock < 0) {
				cout << "Error in accept (): " << strerror (errno) << endl;
			} else {
				FD_SET (peersock, &readset);
				maxfd = (maxfd <peersock) ? peersock : maxfd;
			}
			FD_CLR (serversock, &tempset); // changed from tempsock -> tempset
		 }
		 
		 if (FD_ISSET (0, &tempset)) {
		 	if (read (0, buf, MAX_BUFFER_SIZE) < 0) {
				cout << "Error reading from terminal" << endl;
			}
			if(!strcmp(buf,"exit")||!strcmp(buf,"Exit")||!strcmp(buf,"EXIT")||!strcmp(buf,"quit")||!strcmp(buf,"Quit")||!strcmp(buf,"QUIT")){
				exit1 = true;
				continue;
			}
			int sockfd, portno2, n;
			struct sockaddr_in serveraddr2;
			struct hostent * server;
			char hostname[20];
			pair <string, string> personMessage2 = getPer_msg (buf);
			
			bool isPersonAvailable = false;

			for (pair <string, detail> d: mp) {
				if (d.first == personMessage2.first) {
					strcpy (hostname, d.second.ipaddress);
					portno2 = atoi(d.second.portno);
					cout << "Sending to : ( " << hostname << ":" << portno2 << " )" << endl;
					isPersonAvailable = true;
					break;
				}
			}
			
			if (!isPersonAvailable) {
				continue;
			}

			sockfd = socket (AF_INET, SOCK_STREAM, 0);
			if (sockfd < 0) {
				error_msg("ERROR opening socket");
			}

			// gethostbyname: get the server's DNS entry 
			server = gethostbyname (hostname);
			if (server == NULL) {
				fprintf(stderr, "ERROR, no such host as %s\n", hostname);
				exit(0);
			}

			// build the server's Internet address
			bzero ((char * ) &serveraddr2, sizeof (serveraddr2));
			serveraddr2.sin_family = AF_INET;
			bcopy ((char *) server -> h_addr, (char *) &serveraddr2.sin_addr.s_addr, server -> h_length);
			serveraddr2.sin_port = htons (portno2);

			// connect: create a connection with the server 
			if (connect (sockfd, (struct sockaddr *) &serveraddr2, sizeof (serveraddr2)) <0) {
				error_msg("ERROR connecting");
			}

			// send the message  to the server 
			strcpy (buf, getSendbuf (buf, myName));
			n = write (sockfd, buf, strlen (buf));
			if (n < 0) {
				error_msg("ERROR writing to socket");
			}
			close (sockfd);
			continue;
		 }

		 for (j = 1; j <= maxfd; j++) {
			if (FD_ISSET(j, &tempset)) {
				do {
					result = recv (j, buffer, MAX_BUFFER_SIZE, 0);
				} while (result == -1 && errno == EINTR);

				if (result> 0) {
					buffer[result] = '\0';
					pair <string, string> personMessage1 = getPer_msg(buffer);
					
					cout << personMessage1.first.c_str () << " : " << personMessage1.second.c_str () << endl;

				} else if (result == 0) {
					close (j);
					FD_CLR (j, &readset);
				} else {
					cout << "Error in recv () : " << strerror (errno) << endl;
				}
			} // end--- if (FD_ISSET(j, &tempset))
		 } // end--- for (j=0;...)
	  } // end--- else if (result> 0)
	} while (1);

	
	return 0;
}
