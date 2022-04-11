// CS 2690 Program 1 
// Simple Windows Sockets Echo Client (IPv6)
// Last update: 2/26/2022
// <Hunter Keating> <CS 2690-601> <2/26/2022>
// <Windows 10> <Microsoft Visual Studio 2019>
//
// Usage: WSEchoClientv6 <server IPv6 address> <server port> <"message to echo">
// Companion server is WSEchoServerv6
// Server usage: WSEchoServerv6 <server port>
//
// This program is coded in conventional C programming style, with the 
// exception of the C++ style comments.
//
// I declare that the following source code was written by me or provided
// by the instructor. I understand that copying source code from any other 
// source or posting solutions to programming assignments (code) on public
// Internet sites constitutes cheating, and that I will receive a zero 
// on this project if I violate this policy.
// ----------------------------------------------------------------------------

// Minimum required header files for C Winsock program
#include <stdio.h>       // for print functions
#include <stdlib.h>      // for exit() 
#include <winsock2.h>	 // for Winsock2 functions
#include <ws2tcpip.h>    // adds support for getaddrinfo & getnameinfo for v4+6 name resolution
#include <Ws2ipdef.h>    // optional - needed for MS IP Helper

// #define ALL required constants HERE, not inline 
// #define is a macro, don't terminate with ';'  For example...

#define RCVBUFSIZ 500
#define CORRECTARGUMENTCOUNT 4

// declare any functions located in other .c files here
void DisplayFatalErr(char* errMsg); // writes error message before abnormal termination

void main(int argc, char* argv[]) {   // argc is # of strings following command, argv[] is array of ptrs to the strings
	//Makes sure there is the correct number of arguments
	if (argc != CORRECTARGUMENTCOUNT)
	{
		fprintf(stderr, "Wrong number of arguments\n");
		exit(1);
	}

	// Declare ALL variables and structures for main() HERE, NOT INLINE (including the following...)
	WSADATA wsaData;                // contains details about WinSock DLL implementation
	struct sockaddr_in6 serverInfo;	// standard IPv6 structure that holds server socket info

	// Retrieve the command line arguments. (Sanity checks not required, but port and IP addr will need
	// to be converted from char to int.  See slides 11-15 & 12-3 for details.)
	char* serverIPaddr = argv[1];
	unsigned short serverPort = atoi(argv[2]);


	// Initialize Winsock 2.0 DLL. Returns 0 if ok. If this fails, fprint error message to stderr as above & exit(1).  
	int winsock = WSAStartup(MAKEWORD(2, 0), &wsaData);
	if (winsock != 0) {
		fprintf(stderr, "Winsock did not initialize\n");
		exit(1);
	}

	// Create an IPv6 TCP stream socket.  Now that Winsock DLL is loaded, we can signal any errors as shown on next line:
	int sock;
	sock = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
	if (sock != INVALID_SOCKET)
	{
		//printf("Socket created successfully.  Press any key to continue...\n");
	}
	else
	{
		DisplayFatalErr("socket() function failed.\n");
	}
	// Display helpful confirmation messages after key socket calls like this:
	//getchar();     // needed to hold console screen open


	//______________________________________________________________________________________________________________________
	// If doing extra credit IPv4 address handling option, add the setsockopt() call as follows...
	// if (perrno = setsockopt(sock, IPROTO_IPV6, IPV6_V6ONLY, (char *)&v6Only, sizeof(v6Only)) != 0)
	//     DisplayFatalErr("setsockopt() function failed.");  
	//______________________________________________________________________________________________________________________


	// Zero out the sockaddr_in6 structure and load server info into it.  See slide 11-15.
	memset(&serverInfo, 0, sizeof(serverInfo));
	serverInfo.sin6_family = AF_INET6;
	serverInfo.sin6_port = htons(serverPort);

	u_short WSAAPI htons(u_short hostshort);
	inet_pton(AF_INET6, serverIPaddr, &serverInfo.sin6_addr);
	// Attempt connection to the server.  If it fails, call DisplayFatalErr() with appropriate message,
	if (connect(sock, (struct sockaddr *)&serverInfo, sizeof(serverInfo)) < 0)
	{
		DisplayFatalErr("connect() function failed.\n");
	}
	else
	{
		//printf("Connected\n");
	}//end if/else

	// Send message to server (without '\0' null terminator). Check for null msg (length=0) & verify all bytes were sent...
	// 	   
	// ...else call DisplayFatalErr() with appropriate message as before
	int msgLen = strlen(argv[3]);
	int size = sizeof(serverInfo);
	if (msgLen == 0)
	{
		DisplayFatalErr("Empty message\n");
	}
	else if (send(sock, argv[3], msgLen, 0) != msgLen)
	{
		DisplayFatalErr("Not all bytes were sent\n");
	}//end of if/else
	
	// Retrieve the message returned by server.  Be sure you've read the whole thing (could be multiple segments). 
	// Manage receive buffer to prevent overflow with a big message.
	// Call DisplayFatalErr() if this fails.  (Lots can go wrong here, see slides.)
	int bytesRead;
	char rcvBuffer[BUFSIZ] = { 0 };
	int messageIdx = 0;
	while (messageIdx < msgLen) {
		int recvRet = recv(sock, &rcvBuffer[messageIdx], RCVBUFSIZ - 1, 0);
		if (recvRet == SOCKET_ERROR) {
			DisplayFatalErr("recv failed", 7);
		}
		else if (recvRet == 0) {
			break; // no more bytes to receive
		}
		else {
			messageIdx += recvRet;
		}
	}

	// Display ALL of the received message, in printable C string format.
	printf(rcvBuffer);
	printf("\n");
	// Close the TCP connection (send a FIN) & print appropriate message.
	closesocket(sock);
	// Release the Winsock DLL
	WSACleanup();
	exit(0);
}

