#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netdb.h> 

#define BUF_SIZE 50000

//variables
int portNum;
int keyFD;
int keyLen;
char buffer[BUF_SIZE];
char keyBuffer[BUF_SIZE];
int plainTextFD;
int plainTextLen;
int i;
int socketFD;
int charsWritten;
int charsRead;

struct sockaddr_in serverAddress;
struct hostent* serverHostInfo;

// void error(const char *msg) { 
// 	// printf("%s\n", msg);
// 	fprintf(stderr, "%s", msg); 
// 	exit(1); 
// } // Error function used for reporting issues

int main(int argc, char *argv[]) {

// running in terminal == "otp_enc plaintext key port"
	
	if (argc < 4) {
		fprintf(stderr, "Too Few Arguments\n");
		fflush(stdout);
		exit(1);
	} else if (argc > 4) {
		fprintf(stderr, "Too Many Arguments\n");
		fflush(stdout);
		exit(1);
	}

	plainTextFD = open(argv[1], O_RDONLY); // second argument is plaintext
	plainTextLen =  read(plainTextFD, buffer, BUF_SIZE); //get length of plainTextFD
	// printf("plainTextLen: %d\n", plainTextLen); //test statement
	// fflush(stdout);

	// check for invalid chars
  for (i = 0; i < plainTextLen - 1; i++) {
    if ((int) buffer[i] != 32 && ((int) buffer[i] < 65 || (int) buffer[i] > 90)) {
      fprintf(stderr, "otp_enc plainText File error: plainText File contains bad characters\n");
      // break;
      exit(1);
    }
	}

	// memset(buffer, 0, BUF_SIZE); //reset buffer

	keyFD = open(argv[2], O_RDONLY); //third argument is the key
	keyLen = read(keyFD, keyBuffer, BUF_SIZE); //get length of keyFD
	// printf("keyLen: %d\n", keyLen); //test statement
	// fflush(stdout);

	//check key file for bad characters
  for (i = 0; i < keyLen - 1; i++) {
    if ((int) keyBuffer[i] != 32 && ((int) keyBuffer[i] < 65 || (int) keyBuffer[i] > 90)) {
      fprintf(stderr, "otp_enc Key File error: key contains bad characters\n");
      // break;
      exit(1);
    }
	}	

	// memset(keyBuffer, 0, BUF_SIZE); //reset buffer

	// validate key length is long enough
	if (keyLen < plainTextLen) {
		fprintf(stderr, "Key Too Short\n");
		// fflush(stdout);
		exit(1);
	}

	//store port number
	portNum = atoi(argv[3]); // final argument is the port entered by user
	// printf("portNum: %d\n", portNum);

	memset((char*)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct

	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNum); // Store the port number	
  serverHostInfo = gethostbyname("localhost"); 	// use localhost as target IP address/host
	if (serverHostInfo == NULL) {
    fprintf(stderr, "otp_enc: Couldn't connect port # %d\n", portNum);
    exit(1);
	} 

	memcpy((char*)&serverAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

	// Set up the socket
	socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (socketFD < 0) {
		fprintf(stderr, "otp_enc: Error opening socket\n");
	}

	// Connect to server
	// printf("Connecting to Server\n");
	if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {// Connect socket to address
		fprintf(stderr, "otp_enc: Error connecting\n");
	}
	// printf("Connected\n");

	// Send message to server
	charsWritten = send(socketFD, buffer, strlen(buffer), 0); // Write to the server
	if (charsWritten < 0) {
		fprintf(stderr, "otp_enc: Error writing to socket\n");
	}

	if (charsWritten < strlen(buffer)) {
		fprintf(stderr, "otp_enc: Warning - Not all data written to socket!\n");
	}

	// printf("Getting Return Msg from Server\n");
		// Get return message from server
	memset(buffer, '\0', sizeof(buffer)); // Clear out the buffer again
	charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0); // Read data from the socket, leaving \0 at end
	
	// printf("Received Data from Socket\n");
	if (charsRead < 0) {
		fprintf(stderr, "CLIENT: ERROR reading from socket");
	}
	// printf("CLIENT: I received this from the server: \"%s\"\n", buffer);

	// ************************************************************** // 

		// Send message to server
	charsWritten = send(socketFD, keyBuffer, strlen(keyBuffer), 0); // Write to the server
	if (charsWritten < 0) {
		fprintf(stderr, "otp_enc: Error writing to socket\n");
	}

	if (charsWritten < strlen(keyBuffer)) {
		fprintf(stderr, "otp_enc: Warning - Not all data written to socket!\n");
	}

	// printf("Getting Return Msg from Server\n");
		// Get return message from server
	memset(keyBuffer, '\0', sizeof(keyBuffer)); // Clear out the buffer again for reuse
	charsRead = recv(socketFD, keyBuffer, sizeof(keyBuffer) - 1, 0); // Read data from the socket, leaving \0 at end
	
	// printf("Received Data from Socket\n");
	if (charsRead < 0) {
		fprintf(stderr, "CLIENT: ERROR reading from socket");
	}
	printf("%s\n", keyBuffer);

	return 0;
}