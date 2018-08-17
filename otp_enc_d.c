#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

#define BUF_SIZE 50000
char encryptedMsg[BUF_SIZE];

//function declaration
char* encryptMsg(char* key, char* msg);
// void decryptMsg(char* key, char* cypherText);

// method to encrypt a message that is passed in with a key
char* encryptMsg(char* key, char* msg) {
	int i, j, msgValue, keyValue, encryptedVal, lenMsg, lenKey, lenPlainText;
	char* alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
	memset(encryptedMsg, 0, sizeof(encryptedMsg));
	lenMsg = strlen(msg); // remove null terminator
	for (i = 0; i < lenMsg; i++) {
		for (j = 0; j < 27; j++) {

			if (key[i] == alphabet[j]) { // check if key at index i matches a letter in the accepted string
				keyValue = j;
			}

			if (msg[i] == alphabet[j]) { // check if msg at index i matches a letter in the accepted string
				msgValue = j;
			}

			encryptedVal = (keyValue + msgValue) % 27; // combine values and use remainder 
			encryptedMsg[i] = alphabet[encryptedVal]; // get character equivalent of encrypted value
		}
	}
	encryptedMsg[i] = '\0'; // set null terminator
	// printf("Encrypted Msg:%s\n", encryptedMsg);
	return encryptedMsg;
}

// method to decrypt a cyphertext msg that is passed in with a key
// void decryptMsg(char* key, char* cypherText) {
// 	int i, j, decryptedValue, keyValue, cypherTextVal, lenCypherMsg, lenKey;
// 	char* alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";
// 	char unlockedMsg[1000];
// 	memset(unlockedMsg, 0, sizeof(unlockedMsg));
// 	lenCypherMsg = strlen(cypherText); // remove null terminator
// 	for (i = 0; i < lenCypherMsg; i++) {
// 		for (j = 0; j < 27; j++) {

// 			if (key[i] == alphabet[j]) { // check if key at index i matches a letter in the accepted string
// 				keyValue = j;
// 			}

// 			if (cypherText[i] == alphabet[j]) { // check if msg at index i matches a letter in the accepted string
// 				cypherTextVal = j;
// 			}

// 			if ((cypherTextVal - keyValue) < 0) {
// 				decryptedValue = (cypherTextVal - keyValue + 27) % 27;
// 			} else {
// 				decryptedValue = (cypherTextVal - keyValue) % 27; // combine values and use remainder 
// 			}
// 			unlockedMsg[i] = alphabet[decryptedValue]; // get character equivalent of encrypted value
// 		}
// 	}
// 	unlockedMsg[i] = '\0'; // set null terminator
// 	printf("Decrypted Msg:%s\n", unlockedMsg);
// 	return;
// }

void error(const char *msg) { 
	fprintf(stderr, "%s", msg); 
	exit(1); 
} // Error function used for reporting issues

int main(int argc, char *argv[]) {

	// encryptMsg("XMCKL", "HELLO");
	// decryptMsg("JNGFQKRXUOM BVGT ", "R FYXOQYYUUMOCTZF");

	int listenSocketFD, establishedConnectionFD, portNumber, charsRead;
	socklen_t sizeOfClientInfo;
	char buffer[BUF_SIZE];
	char keyBuffer[BUF_SIZE];
	struct sockaddr_in serverAddress, clientAddress;

	// check that the correct # of arguments are used (port number is included)
	if (argc < 2) { 
		fprintf(stderr, "Please use the correct # of arguments\n"); 
		exit(1); 
	}
	// printf("Arg Count Correct\n");

// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	// printf("Argv[1]: %d\nportNumber: %d\n", atoi(argv[1]), portNumber);
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) {
		error("ERROR opening socket");
	}

	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {// Connect socket to port
		error("ERROR on binding");
	}

	// Flip the socket on - it can now receive up to 5 connections, if error, print error & exit
	if (listen(listenSocketFD, 5) < 0) {
		error("Listener failed\n");
		exit(1);
	}

	// printf("Socket flipped on, starting While Loop\n");

	while(1) {
		// Accept a connection, blocking if one is not available until one connects
		sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
		// printf("About to establish a connection\n");
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
		// printf("FD Accepted\n");
		if (establishedConnectionFD < 0) {
			error("ERROR on accept");		
		}
 		//create fork
		int spawnPid = fork();

		switch(spawnPid) {
	    case -1: { //error case
	      error("Fork Failed!\n");
	      exit(1);
	      break;
	    } // end of error case
	    case 0: { // child case
	    	memset(buffer, 0, BUF_SIZE); // clear out buffer
	    	memset(keyBuffer, 0, BUF_SIZE); // clear out buffer
	    	// Get the message from the client and display it
	    	// printf("About to Recv\n");
	    	// while(1) {
		    	charsRead = recv(establishedConnectionFD, buffer, BUF_SIZE - 1, 0); // Read the client's message from the socket
					if (charsRead < 0) {
						error("ERROR reading from socket");
						exit(1);
					}
					// printf("SERVER: I received this from the client: %s", buffer);

		    	// send ackknowledgement back to client
					charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back
					if (charsRead < 0) {
						error("ERROR writing to socket");
					}

					charsRead = recv(establishedConnectionFD, keyBuffer, BUF_SIZE, 0); // Read the client's message from the socket
					if (charsRead < 0) {
						error("ERROR reading from socket");
						exit(1);
					}
					// printf("SERVER: I received this from the client: %s", keyBuffer);

		   //  	// send acknowledgement back to client
					// charsRead = send(establishedConnectionFD, "I am the server, and I got your message", 39, 0); // Send success back
					// if (charsRead < 0) {
					// 	error("ERROR writing to socket");
					// }

					char* encryptedMsgToSend;
					encryptedMsgToSend = encryptMsg(keyBuffer, buffer);

		    	// send encryptedMessage back to client
					charsRead = send(establishedConnectionFD, encryptedMsg, strlen(encryptedMsg), 0); // Send success back
					if (charsRead < 0) {
						error("ERROR writing to socket");
					}
				// }
				close(establishedConnectionFD); // Close the existing socket which is connected to the client
	    	break;
	    } // end of child case
	    default: { // parent case
	    	close(establishedConnectionFD);
	    	break;
	    } // end of parent case
		}
	} // end of while loop
	close(listenSocketFD); // Close the listening socket
} // end of main
