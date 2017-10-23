/*****************************************
 * Caleb Hayward
 * Justin Novack
 * 2/6/2017
 * CIS457 - Project #1
 * TCP File Transfer
 * tcpserver.c
 ******************************************/

#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char **argv)
{
  int sockfd;
  char portStr[100];
  int portNumber;
  struct sockaddr_in serveraddr, clientaddr;
  int len;
  int clientsocket;
  int pid;
  FILE *fp;
  int numFiles, i;
  char numFilesStr[100], fileSend[512];
  char file[512];
  int fileSize;
  char strSize[100];
  char *buffer;
  int clientNum = 0;

  // Create an end point for communication
  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if(sockfd < 0){
    printf("There was an error creating the socket\n");
    return -1;
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
  serveraddr.sin_addr.s_addr = INADDR_ANY; // Connect to any address

  // Bind name to the socket
  bind(sockfd, (struct sockaddr*)&serveraddr, sizeof(serveraddr));

  // Listen for connections, limit queue to 10
  listen(sockfd, 10);

  // Run in loop to keep server open
  while(1){
    
    len = sizeof(clientaddr);
    
    // Accept a new connection on the socket
    clientsocket = accept(sockfd, (struct sockaddr*)&clientaddr, &len);

    clientNum++;    

    printf("Connection from Client %d accepted\n", clientNum);
    
    // Fork to allow for multiple clients to connect
    pid = fork();

    if(pid == 0){ // Child process
      
      // Run in loop to allow for multiple file transfers
      // from a client until that client exits
      while(1){
	
	// Receive file name from client
	recv(clientsocket, file, sizeof(file), 0);
    
	printf("Received from Client %d: %s\n", clientNum, file);

	// Check for client exit
	if(!strcmp(file, "exit")){
	  break;
	}
	else if(!strcmp(file, "ls")){
	  
	  // Send number of files in server directory to client
	  fp = popen("ls | wc -l", "r");
	  fscanf(fp, "%s", numFilesStr);
	  numFiles = atoi(numFilesStr);
	  pclose(fp);
	  send(clientsocket, numFilesStr, strlen(numFilesStr) + 1, 0);

	  // Send name of all files to client
	  fp = popen("ls", "r");
	  for(i = 0; i < numFiles; i++){
	    usleep(100); // Delay to prevent sending faster than client can receive
	    fscanf(fp, "%s", fileSend);
	    send(clientsocket, fileSend, strlen(fileSend) + 1, 0);
	  }
	  pclose(fp);
	}
	else{
	  
	  // Open up binary file to read
	  fp = fopen(file, "rb");
	  if(fp == NULL){
	    send(clientsocket, "File Not Found", 15, 0);
	    continue;
	  }
	  
	  // Find how long the file is and send to client
	  fseek(fp, 0, SEEK_END);
	  fileSize = ftell(fp);
	  rewind(fp);
	  sprintf(strSize, "%d", fileSize);
	  send(clientsocket, strSize, strlen(strSize) + 1, 0);
	  usleep(100);

	  // Read the file and store into buffer
	  buffer = malloc(fileSize);
	  fread(buffer, fileSize, 1, fp);
	  fclose(fp);
	  
	  // Send file contents to client
	  send(clientsocket, buffer, fileSize, 0);
	  
	  // Free malloc space    
	  free(buffer);
	}
      }

      printf("Client %d exited\n", clientNum);

      // Close client file descriptor
      close(clientsocket);

      // Exit child process
      exit(0);
    }
    else{ // Parent Process
      
      // Close client file descriptor
      close(clientsocket);
    }
  }
  
  return 0;
}
