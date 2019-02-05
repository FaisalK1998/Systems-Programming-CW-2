#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/utsname.h>
#include <dirent.h>
#include "rdwrn.h"

void getIP(int socketNum)
{
	char full_string[32];
	size_t k;

	readn(socketNum, (unsigned char *) &k, sizeof(size_t));
	readn(socketNum, (unsigned char *) &full_string, k);

	printf("\n-----\n");
	printf("StudentID with prefixed IP:\n%s",full_string);
	printf("\n-----\n");

}


void server_time(int socketNum)
{
	char full_time[32];
	size_t k;

	readn(socketNum, (unsigned char *) &k, sizeof(size_t));
	readn(socketNum, (unsigned char *) full_time, k);

	printf("\n-----\n");
	printf("Server Time:\n%s", full_time);
	printf("\n-----\n");

}

	
void systeminfo(int socket)
{
	struct utsname uts;

	if(uname(&uts) == -1)
	{
		perror("uname error");
		exit(EXIT_FAILURE);
	}
	
	size_t payload_length = sizeof(uts);

	writen(socket, (unsigned char *) &payload_length, sizeof(size_t));
	writen(socket, (unsigned char *) &uts, payload_length);

	readn(socket, (unsigned char *) &payload_length, sizeof(size_t));
	readn(socket, (unsigned char *) &uts, payload_length);

	printf("\n-----\n");
	printf("Node Name: %s\n", uts.nodename);
	printf("System name:  %s\n", uts.sysname);
    	printf("Release: %s\n", uts.release);
    	printf("Version: %s\n", uts.version);
    	printf("Machine: %s\n", uts.machine);
    	printf("\n-----\n");

}


void list_files(int socket)
{
	size_t k;

	readn(socket, (unsigned char *) &k, sizeof(size_t));
	char fileslist[k];
	readn(socket, (unsigned char *) fileslist, k);

    	printf("\n---Files On Server---\n");
   	printf("%s", fileslist);
    	printf("\n-----\n"); 

}


void random_num(int socket)
{
	char full_string[32];
	size_t k;

	readn(socket, (unsigned char *) &k, sizeof(size_t));
	readn(socket, (unsigned char *) full_string, k);

	printf("\n-----\n");
	printf("Random Numbers:\n%s",full_string);
	printf("\n-----\n");
}


void get_msg(int socket)
{
    char msg_string[32];
    size_t k;

    readn(socket, (unsigned char *) &k, sizeof(size_t));	
    readn(socket, (unsigned char *) msg_string, k);

    printf("%s\n", msg_string);

}


void send_menu_choice(int socketNum, char *choice)
{
	printf("Sending menu choice...\n");
    	size_t n = strlen(choice) + 1;

    	writen(socketNum, (unsigned char *) &n, sizeof(size_t));
    	writen(socketNum, (unsigned char *) choice, n);

    	printf("Sent choice: %s\n\n", choice);
}


int main(void)
{
	    int sockfd = 0;
	    struct sockaddr_in serv_addr;

	    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Error - could not create socket");
		exit(EXIT_FAILURE);
	    }

	    serv_addr.sin_family = AF_INET;
	    serv_addr.sin_port = htons(50001);
	    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)  {
		perror("Error - connect failed");
		exit(1);
	    } else
	      	printf("Connected to server...\n");


	char *choice = (char*) malloc(8);

	do{
		get_msg(sockfd);

		printf("\nClient Server Application\n");
		printf("1. Show Server IP and Student ID\n");
		printf("2. 5 Random Numbers\n");
		printf("3. Display System Information\n");
		printf("4. List files on server\n");
		printf("5. File Transfer\n");
		printf("6. Exit\n");
		printf("Enter choice: ");
		scanf("%s", choice);

		send_menu_choice(sockfd, choice);

		switch(*choice){
		case '1':
			getIP(sockfd);
			break;
		case '2':
			random_num(sockfd);
			break;
		case '3':
			systeminfo(sockfd);
			break;
		case '4':
			list_files(sockfd);
			break;
		case '5':
			printf("File Transfer Not Implemented");
			break;
		case '6':
			printf("Closing..\n");
			break;
		default:
			printf("Invalid Choice.\nPlease Choose A Valid Option.\n");
		}

	} while (*choice != '6');

	if(choice != NULL)
	{
		free(choice);
	}
			
    	close(sockfd);
    	exit(EXIT_SUCCESS);
}
