//Faisal Khan
//Student Number: S1828698
// Cwk2: server.c - multi-threaded server using readn() and writen()

#include <sys/socket.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <pthread.h>
#include <sys/stat.h>
#include <dirent.h>
#include <signal.h>
#include <sys/time.h>
#include <time.h>
#include "rdwrn.h"

//Thread Function
void *client_handler(void *);

typedef struct utsname systemInfo;


struct timeval t1, t2;
int connfd = 0;
int listenfd = 0;
pthread_t sniffer_thread;

//Declare Functions
void send_IP(int);
void systeminfo(int, systemInfo);
void send_msg(int);
void get_menu_choice(int, char*);
void send_file_list(int);
void random_num(int);
static void handler(int, siginfo_t*, void*);

//////////////


// you shouldn't need to change main() in the server except the port number
int main(void)
{
	//For Signal Handler
	gettimeofday(&t1, NULL);

    	struct sigaction act;

    	memset(&act, '\0', sizeof(act));
	
	//Points to the handler function
    	act.sa_sigaction = &handler;

    	act.sa_flags = SA_SIGINFO;

    	if (sigaction(SIGINT, &act, NULL) == -1) {
		perror("sigaction");
		exit(EXIT_FAILURE);
    	}
	/////

    	int listenfd = 0, connfd = 0;

    	struct sockaddr_in serv_addr;
    	struct sockaddr_in client_addr;
    	socklen_t socksize = sizeof(struct sockaddr_in);
    	listenfd = socket(AF_INET, SOCK_STREAM, 0);
    	memset(&serv_addr, '0', sizeof(serv_addr));

    	serv_addr.sin_family = AF_INET;
    	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    	serv_addr.sin_port = htons(50001);

    	bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));

    	if (listen(listenfd, 10) == -1) {
		perror("Failed to listen");
		exit(EXIT_FAILURE);
    	}
    	//End socket setup
	
	//Accept and incoming connection
	puts("Waiting for incoming connections...");
    	while (1) {
		printf("Waiting for a client to connect...\n");
		connfd =
	    		accept(listenfd, (struct sockaddr *) &client_addr, &socksize);
		printf("Connection accepted...\n");

		pthread_t sniffer_thread;
        // third parameter is a pointer to the thread function, fourth is its actual parameter
	if (pthread_create
		(&sniffer_thread, NULL, client_handler,
	     	(void *) &connfd) < 0) {
	    	perror("could not create thread");
	    	exit(EXIT_FAILURE);
	}
	//Now join the thread , so that we dont terminate before the thread
	//pthread_join( sniffer_thread , NULL);
	printf("Handler assigned\n");
    }


    // never reached...
    // ** should include a signal handler to clean up
    exit(EXIT_SUCCESS);
}//End Main()



///////////////////


//Thread Function with Menu
void *client_handler(void *socket_desc)
{
	//Recieve socket descriptor
	int connfd = *(int *) socket_desc;

	systemInfo *uts;
	uts = (systemInfo *) malloc(sizeof(systemInfo));

	char *menu_choice = (char*) malloc(8);

	do{
		printf("Waiting for client to select option...\n\n");
		send_msg(connfd);

		get_menu_choice(connfd, menu_choice);
		printf("Client %d choice was: %s\n", connfd, menu_choice);

		//Case statement shows choices with associated function
		switch (*menu_choice) {
		case '1':
	    		send_IP(connfd);
	    		break;
		case '2':
			random_num(connfd);
		    	break;
		case '3':
			systeminfo(connfd, *uts);
		    	break;
		case '4':
		    	send_file_list(connfd);
		    	break;
		case '5':
		    	printf("File Transfer not implemented\n");
		    	break;
		case '6':
		    	break;
		default:
		    	printf("Invalid choice\n");
		}

	} while (*menu_choice != '6');

	if(menu_choice != NULL)
	{
		free(menu_choice);
	}

	
	shutdown(connfd, SHUT_RDWR);
    	close(connfd);

	printf("Thread %lu exiting\n", (unsigned long) pthread_self());

    	// always clean up sockets gracefully
    	shutdown(connfd, SHUT_RDWR);
    	close(connfd);

    	return 0;
}  // end client_handler()



////////////////////


//Function to send message to the client
void send_msg(int socket)
{
	//Message to send
	char msg_string[] = "\nPlease enter an option:";

    	size_t n = strlen(msg_string) + 1;

	//Send original struct
    	writen(socket, (unsigned char *) &n, sizeof(size_t));	
    	writen(socket, (unsigned char *) msg_string, n);
 
} // end send_msg()


///////////////////


//Recieves the menu choice from the client
void get_menu_choice(int socketNum, char *choice)
{
    	size_t n;

	//Recieves the altered struct
    	readn(socketNum, (unsigned char *) &n, sizeof(size_t));	
    	readn(socketNum, (unsigned char *) choice, n);

    	printf("Received: %zu bytes\n\n", n);

}// end get_menu_choice()


/////////////

//Function sends the concatenated server IP and Student ID to the Client
void send_IP(int socketNum)
{
	int fd;
	struct ifreq ifr;
	fd = socket(AF_INET, SOCK_DGRAM, 0);

	ifr.ifr_addr.sa_family = AF_INET;

	strncpy(ifr.ifr_name, "eth0", IFNAMSIZ-1);

	ioctl(fd, SIOCGIFADDR, &ifr);
	close(fd);

	char full_string[32];
    	char student_id[] = "-----S1828698";

    	//Copy server IP to full string
    	strcpy(full_string, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

    	//Concatenate Student ID to full string (IP)
    	strcat(full_string, student_id);

	size_t n = strlen(full_string) + 1;

	//Send the original struct
	writen(socketNum, (unsigned char*) &n, sizeof(size_t));
	writen(socketNum, (unsigned char*) full_string, n);

	//Displays the Server IP and Student ID
	printf("Sent string: %s\n\n", full_string);

}//End send_IP


////////////////////


//Function to send the system information to the client
void systeminfo(int socketNum, systemInfo uts)
{

    	if (uname(&uts) == -1) {
		perror("uname error");
		exit(EXIT_FAILURE);
    	}  

    	size_t payload_length = sizeof(uts);

    	size_t n = readn(socketNum, (unsigned char *) &payload_length, sizeof(size_t));
    	printf("payload_length is: %zu (%zu bytes)\n", payload_length, n);
    	n = readn(socketNum, (unsigned char *) &uts, payload_length);

	//Sends the original struct
    	writen(socketNum, (unsigned char *) &payload_length, sizeof(size_t));
    	writen(socketNum, (unsigned char *) &uts, payload_length);

	//Displays the server information
    	printf("Sent server info\n");

}  // end get_and_send_uname()


////////////////////


//Function to send a list of files in the server
void send_file_list(int socketNum)
{

    	DIR *mydir;
    	if ((mydir = opendir("upload")) == NULL) {
		perror("error");
		exit(EXIT_FAILURE);
    	}
    	closedir(mydir);

    	struct dirent *entry = NULL;

    	size_t len = 0;

    	//Loop through entry to get size of all filenames as string.
    	mydir = opendir("upload");
    	while ((entry = readdir(mydir)) != NULL)
    	{
		len = len + strlen(entry->d_name);
    	}
    	closedir(mydir);

    	char filelist[len];

    	//Returns NULL when dir contents all processed
    	mydir = opendir("upload");
    	while ((entry = readdir(mydir)) != NULL)
    	{
		strcat(strcat(filelist, entry->d_name),"\n");
    	}
    	closedir(mydir);

    	strcat(filelist, "\0");

    	size_t n = strlen(filelist);

	//Sennds the original struct
    	writen(socketNum, (unsigned char *) &n, sizeof(size_t));
    	writen(socketNum, (unsigned char *) filelist, n);

	//Display the files
    	printf("Sent file list of size %zu bytes\n",n);

}//end send_file_list()


/////////////////////////


//Function to generate random numbers and send them to the client
void random_num(int socketNum)
{
	static int num[5];

	int i, l;
	time_t t;

	//Sets the limit of numbers to list
	l = 5;

	srand((unsigned) time(&t));
	
	
	for( i = 0 ; i < l ; i++ ) {
		num[i] = rand() % 1000 + 1;
	}

	char strNum[100] = {0};
	char *p = strNum;
	size_t o = 0;

	//Generates random number 
	for(o = 0; o < sizeof num / sizeof *num; ++o) {
		sprintf(p += strlen(p), "%d ", num[o]);
	}

	size_t n = strlen(strNum) + 1;

	//Sends the original struct
	writen(socketNum, (unsigned char *) &n, sizeof(size_t));
	writen(socketNum, (unsigned char *) strNum, n);

}//End random_num



///////////////


//Function to display server up-time when a signal is detected (ctrl + C)
void handler(int sig, siginfo_t *siginfo, void *context)
{
	//Display Message
    	printf("\nReceived a Ctrl+C (SIGINT)\n");
    	printf("Shutting down...\n");

    	pthread_join(sniffer_thread, NULL);

    	shutdown(connfd, SHUT_RDWR);
    	shutdown(listenfd, SHUT_RDWR);

    	gettimeofday(&t2, NULL);
	
	//Calulates the server up-time
    	double totalSeconds = (double) (t2.tv_usec - t1.tv_usec) / 1000000 + (double) (t2.tv_sec - t1.tv_sec) ;

    	int seconds = ((int)totalSeconds % 60);
    	int minutes = ((int)totalSeconds % 3600) / 60;
    	int hours = ((int)totalSeconds % 86400) / 3600;
    	int days = ((int)totalSeconds % (86400 * 30)) / 86400;

	//Displays the server up-time
   	printf ("\nServer up-time = %d days %d hours %d minutes and %d seconds\n", days, hours , minutes , 		seconds);

    	//Clean up sockets
    	close(connfd);
    	close(listenfd);

    	exit(EXIT_SUCCESS);
}//end handler

