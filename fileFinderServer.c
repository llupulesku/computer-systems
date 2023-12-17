//	Compile with:
//	$ gcc fileFinderServer.c -o fileFinderServer -lpthread -g

//---		Header file inclusion					---//

#include	"fileFinder.h"
#include	<errno.h>	// For perror()
#include	<pthread.h>	// For pthread_create()


//---		Definition of constants:				---//

const int	ERROR_FD		= -1;


//---		Definition of global vars:				---//

//  PURPOSE:  To be non-zero for as long as this program should run, or '0'
//	otherwise.


//---		Definition of functions:				---//

//  PURPOSE:  To recursively look for a _file_ named 'filenameCPtr' in the
//	directory named 'dirnameCPtr'.  If it is found then the full path is
//	placed in 'filepathBuffer' of length 'filepathBufferLen', and the
//	_integer_ '1' is returned.  Returns the integer '0' otherwise.
int		didFindFile	(char*		filepathBuffer,
				 size_t		filepathBufferLen,
				 const char*	dirnameCPtr,
				 const char*	filenameCPtr
				)
{
  //  I.  Application validity check:

  //  II.  Look in 'dirnameCPtr' for 'filenameCPtr':
  DIR*			dirPtr;
  struct dirent*	entryPtr;
  char	 		tempBuffer[BUFFER_LEN];
  struct stat		statBuffer;

  //  II.A.  Base case, look for file itself in 'dirnameCPtr':
  //  YOUR CODE HERE:
  //const char* dirnameCPtr = (argc > 1) ? argv[1] : ".";
 dirPtr = opendir(dirnameCPtr);
  

  if(dirPtr== NULL)
  {
     fprintf(stderr,"Cannot open %s\n",dirnameCPtr);
     exit(EXIT_FAILURE);
  }

  //  II.B.  Recursive case: look in subdirectories of 'dirnameCPtr':
  //  YOUR CODE HERE:
  while  ( (entryPtr = readdir(dirPtr)) != NULL)
  {
    snprintf(tempBuffer, BUFFER_LEN,"%s/%s", dirnameCPtr, entryPtr->d_name);
    stat(tempBuffer,&statBuffer);

    if  (S_ISREG(statBuffer.st_mode))
     printf("%23s\t%lld\n",tempBuffer,statBuffer.st_size);
    else
    if  (S_ISDIR(statBuffer.st_mode))
      printf("%23s\t(dir)\n",tempBuffer);
    else
     printf("%23s\t(other)\n",tempBuffer);

}
closedir(dirPtr);
return(EXIT_SUCCESS);

  //  III.  Finished:
  return(0);
}


//  PURPOSE:  To handle the client being communicated with socket file
//	descriptor '*(int*)vPtr'.  Returns 'NULL'.
void*		handleClient	(void*		vPtr
				)
{
  //  I.  Application validity check:

  //  II.  Handle client:
  //  II.A.  Get file descriptor:
  int*	   intArray = (int*)vPtr;
  int 	   clientFd = intArray[0];
  int	   threadNum = intArray[1];

  //  YOUR CODE HERE:
   //int*    argArray    = (int)vPtr; 
  //int    fd        = intArray[0]; 
   free(intArray);

  printf("Thread %d starting.\n",threadNum);

  //  II.B.  Read filename:
  char 	filenameBuffer[BUFFER_LEN];
  char 	filePathBuffer[BUFFER_LEN];
  int	filenameLen;
  int	temp;
  int	readLen;

  //  YOUR CODE HERE:
   read(clientFd ,&filenameLen, sizeof(filenameLen)); 
   filenameLen = ntohl(filenameLen);

  if  ( didFindFile(filePathBuffer,BUFFER_LEN,".",filenameBuffer) )
  {
    int		fileFd;
    char	fileBuffer[BUFFER_LEN];
    int		filePathLen	= strlen(filePathBuffer);

    //  YOUR CODE HERE:
    int toSend=htonl(1);
    //write(fd, toSend, sizeof(toSend));
    fprintf(stderr,"I found the file ");
    return(NULL);
  }
  else
  {
    //  YOUR CODE HERE:
    int toSend=htonl(0);
   // write(fd, toSend, sizeof(toSend));
    fprintf(stderr,"I did not find the file ");

  }

  //  III.  Finished:
  printf("Thread %d quitting.\n",threadNum);
  //  YOUR CODE HERE:

  return(NULL);
}


//  PURPOSE:  To run the server by 'accept()'-ing client requests from
//	'listenFd' and doing them.
void		doServer	(int		listenFd
				)
{
  //  I.  Application validity check:

  //  II.  Server clients:
  pthread_t		threadId;
  pthread_attr_t threadAttr;
  int			threadCount	= 0;

  //  YOUR CODE HERE:
   pthread_attr_init(&threadAttr);
   pthread_attr_setdetachstate(&threadAttr,PTHREAD_CREATE_DETACHED);

  


  while  (1)
  {
    int* clientFdPtr 	= (int*)malloc(2*sizeof(int));

    //  YOUR CODE HERE:
  
    clientFdPtr[0] = accept(listenFd,NULL,NULL);
    clientFdPtr[1] = threadCount;
    threadCount++;
    pthread_create(&threadId,&threadAttr,handleClient,(void*)clientFdPtr);

  }

  pthread_attr_destroy(&threadAttr);

  //  III.  Finished:
}

//  PURPOSE:  To decide a port number, either from the command line arguments
//	'argc' and 'argv[]', or by asking the user.  Returns port number.
int		getPortNum	(int	argc,
				 char*	argv[]
				)
{
  //  I.  Application validity check:

  //  II.  Get listening socket:
  int	portNum;

  if  (argc >= 2)
    portNum	= strtol(argv[1],NULL,0);
  else
  {
    char	buffer[BUFFER_LEN];

    printf("Port number to monopolize? ");
    fgets(buffer,BUFFER_LEN,stdin);
    portNum	= strtol(buffer,NULL,0);
  }

  //  III.  Finished:  
  return(portNum);
}


//  PURPOSE:  To attempt to create and return a file-descriptor for listening
//	to the OS telling this server when a client process has connect()-ed
//	to 'port'.  Returns that file-descriptor, or 'ERROR_FD' on failure.
int		getServerFileDescriptor
				(int		port
				)
{
  //  I.  Application validity check:

  //  II.  Attempt to get socket file descriptor and bind it to 'port':
  //  II.A.  Create a socket
  int socketDescriptor = socket(AF_INET, // AF_INET domain
			        SOCK_STREAM, // Reliable TCP
			        0);

  if  (socketDescriptor < 0)
  {
    perror("socket");
    return(ERROR_FD);
  }

  //  II.B.  Attempt to bind 'socketDescriptor' to 'port':
  //  II.B.1.  We'll fill in this datastruct
  struct sockaddr_in socketInfo;

  //  II.B.2.  Fill socketInfo with 0's
  memset(&socketInfo,'\0',sizeof(socketInfo));

  //  II.B.3.  Use TCP/IP:
  socketInfo.sin_family = AF_INET;

  //  II.B.4.  Tell port in network endian with htons()
  socketInfo.sin_port = htons(port);

  //  II.B.5.  Allow machine to connect to this service
  socketInfo.sin_addr.s_addr = INADDR_ANY;

  //  II.B.6.  Try to bind socket with port and other specifications
  int status = bind(socketDescriptor, // from socket()
		    (struct sockaddr*)&socketInfo,
		    sizeof(socketInfo)
		   );

  if  (status < 0)
  {
    perror("bind");
    return(ERROR_FD);
  }

  //  II.B.6.  Set OS queue length:
  listen(socketDescriptor,5);

  //  III.  Finished:
  return(socketDescriptor);
}


int		main		(int	argc,
				 char*	argv[]
				)
{
  //  I.  Application validity check:

  //  II.  Do server:
  int 	      port	= getPortNum(argc,argv);
  int	      listenFd	= getServerFileDescriptor(port);
  int	      status	= EXIT_FAILURE;

  if  (listenFd >= 0)
  {
    doServer(listenFd);
    close(listenFd);
    status	= EXIT_SUCCESS;
  }

  //  III.  Finished:
  return(status);
}
