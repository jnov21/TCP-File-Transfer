/*****************************************
 * Caleb Hayward
 * Justin Novack
 * 2/6/2017
 * CIS457 - Project #1
 * TCP File Transfer
 * tcpclient.c
 ******************************************/

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char** argv)
{
  int sockfd;
  char ipAddress[100], portStr[100];
  int portNumber;
  struct sockaddr_in serveraddr;
  int e;
  char file[512];
  char *buffer;
  char numFilesStr[100];
  int numFiles, i;
  char fileRecv[512];
  char fileSizeStr[100];
  int fileSize;
  char *fileName, *fileExtension;
  char outputFile[512];
  FILE *fp;

  // Create an end point for communication
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if(sockfd < 0){
    printf("There was an error creating the socket\n");
    return -1;
  }

  // Run in loop until valid IP address is entered
  while(1){
    
    // Prompt the user to enter the IP address
    printf("Enter IP Address: ");
    scanf("%s", ipAddress);

    // Check that a valid IP address was entered
    if(!strcmp(ipAddress, "")){
      printf("Invalid IP Address\n");
      continue;
    }
    
    break;
  }

  // Run in loop until valid port number is entered
  while(1){
    
    // Prompt the user to enter a port number
    printf("Enter the Port Number: ");
    scanf("%s", portStr);
    portNumber = atoi(portStr);

    // Check that a valid port number was entered
    if(portNumber < 1024){
      printf("Invalid Port Number\n");
      continue;
    }

    break;
  }

  // Set variables of server address structure
  serveraddr.sin_family = AF_INET; // Internet socket
  serveraddr.sin_port = htons(portNumber); // Port Number
  serveraddr.sin_addr.s_addr = inet_addr(ipAddress); // IP Address

  // Connect to the server
  e = connect(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));

  if(e < 0){
    printf("Error connecting to the server\n");
    close(sockfd);
    return -2;
  }

  // Run in loop to allow for multiple file transfers
  while(1){
    
    // Prompt user to enter a file to receive from the server
    printf("Enter a file to receive from server: ");
    scanf("%s", file);

    // Send file to the server
    send(sockfd, file, strlen(file) + 1, 0);

    if(!strcmp(file, "exit")){ // Exit Client
      break;
    }
    else if(!strcmp(file, "ls")){ // List Available Server Files
      
      // Receive number of files from server
      recv(sockfd, numFilesStr, sizeof(numFilesStr), 0);

      numFiles = atoi(numFilesStr);

      // Receive and display all file names
      for(i = 0; i < numFiles; i++){
	memset(fileRecv, 0, strlen(fileRecv));
	recv(sockfd, fileRecv, sizeof(fileRecv), 0);
	printf("%s\n", fileRecv);
      }
    }
    else{

      // Receive size of file contents from server
      recv(sockfd, fileSizeStr, sizeof(fileSizeStr), 0);

      // Check if the file was received from server
      if(!strcmp(fileSizeStr, "File Not Found")){
	printf("Server could not find file\n");
	continue;
      }
    
      fileSize = atoi(fileSizeStr);
    
      printf("File Size: %d bytes\n", fileSize); 
    
      // Allocate space for buffer to store file contents received
      buffer = malloc(fileSize);

      // Receive file contents from server
      recv(sockfd, buffer, fileSize, 0);

      // Print the received messages
      printf("Received Buffer: %s\n", buffer);

      // Save copy of the file
      fileName = strtok(file, ".");
      fileExtension = strtok(NULL, ".");
      sprintf(outputFile, "%s_Copy.%s", fileName, fileExtension);
      printf("Saving copy of file with name %s\n", outputFile);
      fp = fopen(outputFile, "wb");
      fwrite(buffer, fileSize, 1, fp);
      fclose(fp);

      // Free malloc space
      free(buffer);
    }
  }

  // Close file descriptor
  close(sockfd);

  return 0;
}
