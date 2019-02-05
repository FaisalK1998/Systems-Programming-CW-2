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

void *client_handler(void *);
typedef struct utsname systemInfo;


struct timeval t1, t2;
int connfd = 0;
int listenfd = 0;
pthread_t sniffer_thread;


void send_IP(int);
void systeminfo(int, systemInfo);
void send_msg(int);
void get_menu_choice(int, char*);
void send_file_list(int);
void random_num(int);
static void handler(int, siginfo_t*, void*);



int main(void)
{

	gettimeofday(&t1, NULL);

    	struct sigaction act;

    	memset(&act, '\0', sizeof(act));

    	act.sa_sigaction = &handler;

    	act.sa_flags = SA_SIGINFO;

    	if (sigaction(SIGINT, &act, NULL) == -1) {
		perror("sigaction");
		exit(EXIT_FAILURE);
    	}


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

	puts("Waiting for incoming connections...");
    	while (1) {
		printf("Waiting for a client to connect...\n");
		connfd =
	    	accept(listenfd, (struct sockaddr *) &client_addr, &socksize);
		printf("Connection accepted...\n");
		pthread_t sniffer_thread;

	if (pthread_create
		(&sniffer_thread, NULL, client_handler,
	    	(void *) &connfd) < 0) {
	    	perror("Could Not Create Thread");
	    	exit(EXIT_FAILURE);
	}
	printf("Handler assigned\n");
    }

    	exit(EXIT_SUCCESS);
}




void *client_handler(void *socket_desc)
{
	int connfd = *(int *) socket_desc;

	systemInfo *uts;
	uts = (systemInfo *) malloc(sizeof(systemInfo));

	char *menu_choice = (char*) malloc(8);

	do{
		printf("Waiting for client to select option...\n\n");
		send_msg(connfd);

		get_menu_choice(connfd, menu_choice);
		printf("Client %d choice was: %s\n", connfd, menu_choice);

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
		    	printf("Invalid choice.\nPlease Choose A Valid Option.\n");
		}

	} while (*menu_choice != '6');

	if(menu_choice != NULL)
	{
		free(menu_choice);
	}

	
	shutdown(connfd, SHUT_RDWR);
    	close(connfd);

	printf("Thread %lu exiting\n", (unsigned long) pthread_self());

    	shutdown(connfd, SHUT_RDWR);
    	close(connfd);

    	return 0;
} 



void send_msg(int socket)
{
    char msg_string[] = "\nPlease enter an option:";

    size_t n = strlen(msg_string) + 1;
    writen(socket, (unsigned char *) &n, sizeof(size_t));	
    writen(socket, (unsigned char *) msg_string, n);
 
}



void get_menu_choice(int socketNum, char *choice)
{
    size_t n;

    readn(socketNum, (unsigned char *) &n, sizeof(size_t));	
    readn(socketNum, (unsigned char *) choice, n);

    printf("Received: %zu bytes\n\n", n);

}



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

    	strcpy(full_string, inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
    	strcat(full_string, student_id);

	size_t n = strlen(full_string) + 1;

	writen(socketNum, (unsigned char*) &n, sizeof(size_t));
	writen(socketNum, (unsigned char*) full_string, n);

	printf("Sent string: %s\n\n", full_string);

}



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

    	writen(socketNum, (unsigned char *) &payload_length, sizeof(size_t));
    	writen(socketNum, (unsigned char *) &uts, payload_length);

    	printf("Sent server info\n");

}



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

    	mydir = opendir("upload");
    	while ((entry = readdir(mydir)) != NULL)
    	{
		len = len + strlen(entry->d_name);
    	}
    	closedir(mydir);
    	char filelist[len];

    	mydir = opendir("upload");
    	while ((entry = readdir(mydir)) != NULL)
    	{
		strcat(strcat(filelist, entry->d_name),"\n");
    	}
    	closedir(mydir);

    	strcat(filelist, "\0");

    	size_t n = strlen(filelist);

    	writen(socketNum, (unsigned char *) &n, sizeof(size_t));
    	writen(socketNum, (unsigned char *) filelist, n);

    	printf("Sent file list of size %zu bytes\n",n);

}


void random_num(int socketNum)
{
	static int num[5];

	int i, l;
	time_t t;

	l = 5;

	srand((unsigned) time(&t));
	
	for( i = 0 ; i < l ; i++ ) {
		num[i] = rand() % 1000 + 1;
	}

	char strNum[100] = {0};
	char *p = strNum;
	size_t o = 0;

	for(o = 0; o < sizeof num / sizeof *num; ++o) {
		sprintf(p += strlen(p), "%d ", num[o]);
	}

	size_t n = strlen(strNum) + 1;

	writen(socketNum, (unsigned char *) &n, sizeof(size_t));
	writen(socketNum, (unsigned char *) strNum, n);

}


static void handler(int sig, siginfo_t *siginfo, void *context)
{
    	printf("\nReceived a Ctrl+C (SIGINT)\n");
    	printf("Shutting down...\n");

    	pthread_join(sniffer_thread, NULL);

    	shutdown(connfd, SHUT_RDWR);
    	shutdown(listenfd, SHUT_RDWR);

    	gettimeofday(&t2, NULL);

    	double totalSeconds = (double) (t2.tv_usec - t1.tv_usec) / 1000000 + (double) (t2.tv_sec - t1.tv_sec);

    	int seconds = ((int)totalSeconds % 60);
    	int minutes = ((int)totalSeconds % 3600) / 60;
    	int hours = ((int)totalSeconds % 86400) / 3600;
    	int days = ((int)totalSeconds % (86400 * 30)) / 86400;

   	printf ("\nServer up-time = %d days %d hours %d minutes and %d seconds\n", days, hours , minutes , 	seconds);

    	close(connfd);
    	close(listenfd);

    	exit(EXIT_SUCCESS);
}

