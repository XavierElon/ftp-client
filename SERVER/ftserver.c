/* Name: Xavier Hollingsworth
** Date: 08/10/2019
** Description:
** Sources: Beej's Guide
	 for sending files: http://stackoverflow.com/questions/2014033/send-and-receive-a-file-in-socket-programming-in-linux-with-c-c-gcc-g
*/


#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

// Function to create address information from Beej's guide
struct addrinfo *getAddress( char *port)
{
  struct addrinfo hints;
  struct addrinfo *server;
  int status;

  // Make sure the struct is empty
  memset(&hints, 0, sizeof(hints));

  // IPv4
  hints.ai_family = AF_INET;
  // TCP stream sockets
  hints.ai_socktype = SOCK_STREAM;
  //Fill in my IP for me
  hints.ai_flags = AI_PASSIVE;

  status = getaddrinfo(NULL, port, &hints, &server);

  // Pulled straight from Beej's Guide for handling an error
  if(status != 0)
  {
    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
    exit(1);
  }
  return server;
}


// Function to create address information with IP address as well
struct addrinfo *createIPAddress(char *ipAddress, char *port)
{
  int status;
  struct addrinfo hints;
  struct addrinfo *server;
  
  // Clear the struct
  memset(&hints, 0 ,sizeof hints);
  // IPv4
  hints.ai_family = AF_INET;
  // TCP stream sockets
  hints.ai_socktype = SOCK_STREAM;

  // Error checking
  status = getaddrinfo(ipAddress, port, & hints, &server);
  if (status != 0)
  {
    fprintf(stderr, "Incorrect port number.\n", gai_strerror(status));
    exit(1);
  }
  return server;
}


// Create the socket Source: Beej's guid
int createSocket(struct addrinfo *server)
{
  // Socket file descriptor
  int socketfd = socket(server->ai_family, server->ai_socktype, server->ai_protocol);

  // Error checking
  if (socketfd == -1)
  {
    fprintf(stderr, "Socket not created.\n");
    exit(1);
  }
  return socketfd;
}

// This function now connects the socket
void connectSocket(int socketfd, struct addrinfo *connection)
{
  int connectionStatus = connect(socketfd, connection->ai_addr, connection->ai_addrlen);

  // Error checking
  if (connectionStatus == -1)
  {
    fprintf(stderr, "Socket connection failed.\n");
    exit(1);
  }
}



/* Function to retrieve the files in the directory
** Source: http://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program
*/
int listFiles(char **fileList)
{
  // Initialize variables
  int n = 0;
  DIR *d;
  struct dirent *dir;
  d = opendir(".");

  
  if (d)
  {
    // While directory is not empty copy file name to array
    while ((dir = readdir(d)) != NULL)
    {
      if(dir->d_type == DT_REG)
      {
        strcpy(fileList[n], dir->d_name);
        n++;
      }
    }
    closedir(d);
  }
  return n;
}


/* Function to check if file requested exists . Returns 0 if it doesn't and 1 if it does.
*/
int doesFileExist(char **fileList, int numberOfFiles, char *filename)
{
  // Boolean variable to determine if file does exist 
  int exists = 0;
  int n = 0;

  for (n = 0; n < numberOfFiles; n++)
  {
    if (strcmp(fileList[n], filename) == 0)
    {
      exists = 1;
    }
  }
  return exists;
}


void sendFile(char *ipAddress, char *port, char *filename)
{
  sleep(5);
  // Struct for server info with ip address
  struct addrinfo *server = createIPAddress(ipAddress, port);

  // Create the socket
  int socket = createSocket(server);

  // Connect the socket
  connectSocket(socket, server);


  // Create buffer to hold message
  char buffer[2048];
  // Clear buffer
  memset(buffer, 0, sizeof(buffer));

  // Open file descriptor
  int fileDescriptor = open(filename, O_RDONLY);
  
  // Read entire file
  while(1)
  {
    // Calculate number of bytes to be read
    int numberBytes = read(fileDescriptor, buffer, sizeof(buffer) - 1);
    if(numberBytes == 0)
    	break;

    if(numberBytes < 0)
    {
      fprintf(stderr, "Unable to read file\n");
      return;
    }
    void *token = buffer;
    
    // Loop until all bytes are sent
    while(numberBytes > 0)
    {
      int bytesWritten = send(socket, token, sizeof(buffer), 0);
      // Error checking for bytes written
      if (bytesWritten < 0)
      {
        fprintf(stderr, "File not sent.\n");
	return;
      }
      numberBytes = numberBytes - bytesWritten;
      token = token + bytesWritten;
    }
    // Clear the buffer
    memset(buffer, 0, sizeof(buffer));
  }

  memset(buffer, 0, sizeof(buffer));
  strcpy(buffer, "finished");
  
  send(socket, buffer, sizeof(buffer), 0);

  // FInally close the socket and free the structure we created at the beginning
  close(socket);
  freeaddrinfo(server);
}


/* This is the function that sends all the names of the files in the directory if the user uses the -g command
*/
void sendDirectory(char *ipAddress, char *port, char *fileList, int number)
{
  int n;
  char *string = "finished";
  // Struct for server with IP address info
  struct addrinfo *server = createIPAddress(ipAddress, port);

  // Create the socket
  int socket = createSocket(server);

  // Connect the socket
  connectSocket(socket, server);

  // Loop to send all files
  for (n = 0; n < number; n++)
  {
    send(socket, fileList[n], 100, 0);
  }

  // Message to notify client all files have been sent
  send(socket, string, strlen(string), 0);

  // Close the socket and free the struct
  close(socket);
  freeaddrinfo(server);
}


/* Function to create an array of strings to hold files in directory
*/
char **createArray(int size)
{
  // Initialize variables
  int n;
  char **list = malloc(size * sizeof(char *));

  // Create an array to hold the filenames up to 1000 characters and make sure they are clear in memory
  for (n = 0; n < size; n++)
  {
    list[n] = malloc(1000 * sizeof(char));
    memset(list[n], 0, sizeof(list[n]));
  }
  return list;
}

/* This function deletes the array that was used to hold the file names
*/
void deleteArray(char **list, int size)
{
  int n;

  // Loop through array and delete each filename
  for (n = 0; n < size; n++)
  {
    free(list[n]);
  }
  // And finally free the entire array from memory
  free(list);
}


/* This is the meat of the program. This function is what handles what the client sends to the server and sends back what the client has requested
*/
void connection(int newfd)
{
  // Setup ACK messages to make program run smoothly
  char *pass = "pass";
  char *fail = "fail";
  // Initialize variables that were sent by client
  char option[100];
  char port[50];
  char address[100];
  char filename[100];

  // Clear variables before use
  memset(option, 0, sizeof(option));
  memset(port, 0, sizeof(port));
  memset(address, 0, sizeof(address));
  memset(filename, 0, sizeof(filename));

  // Receive port number and send ACK
  recv(newfd, port, sizeof(port) - 1, 0);
  send(newfd, pass, strlen(pass), 0);

  // Receive command option and send ACK
  recv(newfd, option, sizeof(option) - 1, 0);
  send(newfd, pass, strlen(pass), 0);


  // Receive IP address
  recv(newfd, address, sizeof(address) - 1, 0);

  // Print the connection
  printf("Connection from %s\n", address);

  // Check what command was sent by client
  if(strcmp(option, "l") == 0)
  {
    // Send file list was requested by client
    send(newfd, pass, strlen(pass), 0);
    printf("List directory requested on port %s.\n", port);
    printf("Sending file list to %s:%s.\n", address, port);
    
    // Create file list
    char **fileList = createArray(1000);
    // Retrieve number of files
    int fileNumber = listFiles(fileList);

    // Send the directory to the port number
    sendDirectory(address, port, fileList, fileNumber);
    
    // Free file list array in memory to prevent seg fault
    deleteArray(fileList, 1000);
  }
  
  // Client requested a single file
  else if(strcmp(option, "g") == 0)
  {
    // Send ACK
    send(newfd, pass, strlen(pass), 0);

    // Receive filename
    recv(newfd, filename, sizeof(filename) - 1, 0);
    printf("File \"%s\" requested on port %s.\n", filename, port);

    // Check if file exists
    char **fileList = createArray(1000);
    int number = listFiles(fileList);
    int foundFile = doesFileExist(fileList, number, filename);

    // If the file was found send it
    if(foundFile)
    {
      printf("File \"%s\" requested on port %s\n.", filename, port);
      char *fileFound = "File found";
      send(newfd, fileFound, strlen(fileFound), 0);
      
      // Create a new string to hold the ./
      char *newFileName[200];
      memset(newFileName, 0, sizeof(newFileName));
      strcpy(newFileName, "./");
      char *total = newFileName + strlen(newFileName);
      total += sprintf(address, port, newFileName);

      // And finally send the file
      sendFile(address, port, newFileName);
    }

    // If file was not found on display error message
    else
    {
      printf("File not found. Sending error mesasage to %s:%s\n", address, port);
      char *fileNotFound = "File not found";
      
      // Send error message to client
      send(newfd, fileNotFound, strlen(fileNotFound), 0);
    }
    // Delete the file list array
    deleteArray(fileList, 1000);
  }

  // Client used incorrect command
  else
  {
    send(newfd, fail, strlen(fail), 0);
    printf("Invalid command.\n");
  }

  printf("Waiting for new connections(inside of connection() function)\n");

}



// Main function
int main(int argc, char *argv[])
{
  // Make sure correct number of arguments was used for server program
  if (argc != 2)
  {
    fprintf(stderr, "Error, incorrect number of arguments.\n");
    exit(1);
  }
  
  // Display success message if port has been opened
  printf("Server has been opened on port number %s\n", argv[1]);

  // Create struct for server info
  struct addrinfo *server = getAddress(argv[1]);

  // Create socket file descriptor
  int socketfd = createSocket(server);

  // Bind the socket
  if(bind(socketfd, server->ai_addr, server->ai_addrlen) == -1)
  {
    close(socketfd);
    fprintf(stderr, "Error binding socket.\n");
    exit(1);
  }

  // Setup socket to listen Source: Beej's Guide Page 36
  if(listen(socketfd, 20) == -1)
  {
    perror("listening error.\n");
    exit(1);
  }

  
  // Struct for client address
  struct sockaddr_storage clientAddress;
  socklen_t addressSize = sizeof(clientAddress);
  
  // Search for connection from client
  while(1)
  {
    addressSize = sizeof(clientAddress);
    int newfd = accept(socketfd, (struct addrinfo *)&clientAddress, &addressSize);

    // If client hasn't connected yet keep waiting
    if (newfd == -1)
    {
      continue;
    }

    connection(newfd);
    close(newfd);
  }

  // Free the addrinfo struct as recommended by Beej's guide
  freeaddrinfo(server);
}
