//Faisal Khan
//Student Number: S1828698
// Cwk2: client.c - message length headers with variable sized payloads
// also use of readn() and writen() implemented in separate code module

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

//Function to get IP Address
void getIP(int socketNum)
{
	char full_string[32];
	size_t k;

	//Reads the IP Address of the user 
	readn(socketNum, (unsigned char *) &k, sizeof(size_t));
	readn(socketNum, (unsigned char *) &full_string, k);
	
	//Display IP Address
	printf("\n-----\n");
	printf("StudentID with prefixed IP:\n%s",full_string);
	printf("\n-----\n");

}//End of getIP

//////////////////

	
//Function to get system information as a utsname
void systeminfo(int socket)
{
	struct utsname uts;

	if(uname(&uts) == -1)
	{
		perror("uname error");
		exit(EXIT_FAILURE);
	}
	
	size_t payload_length = sizeof(uts);
	
	//Sends the original struct
	writen(socket, (unsigned char *) &payload_length, sizeof(size_t));
	writen(socket, (unsigned char *) &uts, payload_length);

	//Recieves the altered struct 
	readn(socket, (unsigned char *) &payload_length, sizeof(size_t));
	readn(socket, (unsigned char *) &uts, payload_length);

	//Display system details 
	printf("\n-----\n");
	printf("Node Name: %s\n", uts.nodename);
	printf("System name:  %s\n", uts.sysname);
    	printf("Release: %s\n", uts.release);
    	printf("Version: %s\n", uts.version);
    	printf("Machine: %s\n", uts.machine);
    	printf("\n-----\n");

}//End of systeminfo

////////////


//Function to list all files from the server
void list_files(int socket)
{
	size_t k;
	
	//Recieves the files available
	readn(socket, (unsigned char *) &k, sizeof(size_t));
	char fileslist[k];
	readn(socket, (unsigned char *) fileslist, k);

	//Display the files available on the server
    	printf("\n---Files On Server---\n");
   	printf("%s", fileslist);
    	printf("\n-----\n"); 

}//End of list_files

/////////////


//Function to generate random number
void random_num(int socket)
{
	char full_string[32];
	size_t k;

	//Recieves the altered struct
	readn(socket, (unsigned char *) &k, sizeof(size_t));
	readn(socket, (unsigned char *) full_string, k);

	//Display Random numbers
	printf("\n-----\n");
	printf("Random Numbers:\n%s",full_string);
	printf("\n-----\n");
}//End of random_num

//////////


//Recieves message from the server
void get_msg(int socket)
{
    	char msg_string[32];
    	size_t k;

	//Recieves the altered struct
    	readn(socket, (unsigned char *) &k, sizeof(size_t));	
    	readn(socket, (unsigned char *) msg_string, k);

    	printf("%s\n", msg_string);

}//End of get_msg


///////////////

//writes string to the server
void send_menu_choice(int socketNum, char *choice)
{
	printf("Sending menu choice...\n");
    	size_t n = strlen(choice) + 1;

    	writen(socketNum, (unsigned char *) &n, sizeof(size_t));
    	writen(socketNum, (unsigned char *) choice, n);

    	printf("Sent choice: %s\n\n", choice);
}//End of send_menu_choice


//////////////////


int main(void)
{
	    // *** this code down to the next "// ***" does not need to be changed except the port number
	    int sockfd = 0;
	    struct sockaddr_in serv_addr;

	    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Error - could not create socket");
		exit(EXIT_FAILURE);
	    }

	    serv_addr.sin_family = AF_INET;

	    // IP address and port of server we want to connect to
	    serv_addr.sin_port = htons(50001);
	    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	    // try to connect...
	    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)  {
		perror("Error - connect failed");
		exit(1);
	    } else
	      	printf("Connected to server...\n");

		// ***

////////

	char *choice = (char*) malloc(8);

	do{
		get_msg(sockfd);		
		
		//Display Menu Options
		printf("\nClient Server Application\n");
		printf("1. Show Server IP and Student ID\n");
		printf("2. 5 Random Numbers\n");
		printf("3. Display System Information\n");
		printf("4. List files on server\n");
		printf("5. File Transfer\n");
		printf("6. Exit\n");
		printf("Enter choice: ");
		scanf("%s", choice);

		//Send user input to server
		send_menu_choice(sockfd, choice);

		//Case statement states choices with associated function
		switch(*choice)
		{
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
			printf("File Transfer");
			//(sockfd);
			break;
		case '6':
			printf("Closing..\n");
			break;
		default:
			printf("Invalid Choice.\nPlease Choose A Valid Option.\n");
		}

	}while (*choice != '6');

	if(choice != NULL)
	{
		free(choice);
	}
			

    // *** make sure sockets are cleaned up
    	close(sockfd);

    	exit(EXIT_SUCCESS);
} // end main()
