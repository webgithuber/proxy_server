#include <winsock2.h>
#include <windows.h>
#include<ws2tcpip.h>
#include <iostream>
#include <unistd.h>
#include<thread>
#include <bits/stdc++.h>

#include "proxy_parse.h"


using namespace std;

#define MAX_Bytes 10*(1<<10)


/*
  proxy_parse.c -- a HTTP Request Parsing Library.
  COS 461  
*/

#include "proxy_parse.h"

#define DEFAULT_NHDRS 8
#define MAX_REQ_LEN 65535
#define MIN_REQ_LEN 4



#define MAX_BYTES (1<<11)

#include "proxy_parse.h"
#include "LRU.hpp"
LRU Cache;

// Follow the steps below to use this file

// 1. In main file add the following include 
// #include "NewFile1.cpp"

// 2. Add the following in the code
// sayHelloFromNewFile1();





// #pragma comment(lib, "ws2_32")


// WSADATA is a structure used to hold information about the Winsock implementation.
// Winsock is essentially a library of functions that developers
//  can use to create network-enabled applications.

// It initializes the Winsock DLL (Dynamic Link Library) and prepares it for use.
//  Without calling WSAStartup, none of the Winsock functions (like socket(), bind(), connect(), etc.) 
//  will work.

// WSAStartup
// WORD it take input as version
//as output WSADATA structure that will be filled in by WSAStartup
//  with details about the Windows Sockets implementation,

// On success: A SOCKET handle (non-negative integer) representing the created socket.

//struct sockaddr_in
// This structure is used when setting up sockets for network communication, such as binding
//  a socket to an IP address and port or specifying a remote address to connect to.

//client1 and client2 will send data to server and server will respond to each;



 char* convert_Request_to_string(struct ParsedRequest *req)
{

	/* Set headers */
	ParsedHeader_set(req, "Host", req -> host);
	ParsedHeader_set(req, "Connection", "close");

	int iHeadersLen = ParsedHeader_headersLen(req);

	char *headersBuf;

	headersBuf = (char*) malloc(iHeadersLen + 1);

	if (headersBuf == NULL) {
		fprintf (stderr," Error in memory allocation  of headersBuffer ! \n");
		exit (1);
	}


	ParsedRequest_unparse_headers(req, headersBuf, iHeadersLen);
	headersBuf[iHeadersLen] = '\0';
     //cout<<"headersBuf: "<<headersBuf<<"\n";


	int request_size = strlen(req->method) + strlen(req->path) + strlen(req->version) + iHeadersLen + 4;
	
	char *serverReq;

	serverReq = (char *) malloc(request_size + 1);

	if(serverReq == NULL){
		fprintf (stderr," Error in memory allocation for serverrequest ! \n");
		exit (1);
	}

	serverReq[0] = '\0';
	strcpy(serverReq, req->method);
	strcat(serverReq, " ");
	strcat(serverReq, req->path);
	strcat(serverReq, " ");
	strcat(serverReq, req->version);
	strcat(serverReq, "\r\n");
	strcat(serverReq, headersBuf);
     //strcat(serverReq, "\r\n");

	free(headersBuf);

	return serverReq;

}

int CreatServerSocket(char * host,int port=80)
{
     int remotesocket=socket(AF_INET,SOCK_STREAM,0);
     if(remotesocket<0){cout<<"error in creating socket\n";}
     struct hostent* ip=gethostbyname(host);
     struct sockaddr_in server_addr;
     server_addr.sin_family=AF_INET;
     server_addr.sin_port=htons(port);
     cout<<host<<" ";
     cout<<"ip: "<<inet_ntoa(*((struct in_addr*)ip->h_addr_list[0]));
     server_addr.sin_addr.s_addr=inet_addr(inet_ntoa(*((struct in_addr*)ip->h_addr_list[0])));
     
     if(connect(remotesocket,(struct sockaddr *)&server_addr,sizeof(server_addr))<0)
     {
          cout<<"error in connecting serevr\n";
     }
     return remotesocket;
     
}

int datafromclient(int sockid,mutex &mtx)
{   
	cout<<"started";
	int MAX_BUFFER_SIZE = MAX_BYTES;

	char buf[MAX_BUFFER_SIZE];

	int newsockfd = sockid;

	char *request_message;  // Get message from URL

	request_message = (char *) malloc(MAX_BUFFER_SIZE); 

	if (request_message == NULL) {
		fprintf (stderr," Error in memory allocation ! \n");
		exit (1);
	}	

	request_message[0] = '\0';
     int bytes_recieved = recv(newsockfd, request_message, MAX_BUFFER_SIZE, 0) ;
     int len;
     int t_bytes=0;
	while (bytes_recieved>=1)
     {
          t_bytes+=bytes_recieved;
          len=strlen(request_message);
          if(strstr(request_message, "\r\n\r\n") == NULL) {  // determines end of request

	     bytes_recieved = recv(newsockfd, request_message+len, MAX_BUFFER_SIZE-len, 0) ;
          }
          else
          {
               break;
          }

     }
     int request_size=strlen(request_message);
     char *url=new char[request_size];
     for (int i = 0; i < request_size; i++)
     {
          url[i]=request_message[i];
     }
     cout<<"bytes: "<<t_bytes<<"\n";
     char *buff;
     if(Cache.lack.try_lock())
     {
       buff=Cache.find(request_message);
       Cache.lack.unlock();
     }

     if(buff==NULL){cout<<"find failed;\n";}
     else
     {
          char response[MAX_BYTES];
          request_message[0] = '\0';
          int pos=0;
          int size=strlen(buff);
          while (pos<size)
          {
               response[pos]=buff[pos];pos++;
          }
          send(sockid,response,size,0);
          cout<<"sent from cache";
          
     }


//      //cout<<"(("<<request_message<<"))";
     
//     cout<<"[[";
//     if(strstr(request_message, "\r\n\r\n"))
//     {
//      cout<<"it contains \r\n\r\n";
//      //cout<<request_message;
//     }
//     cout<<"]]";
//     sleep(10);
    //cout<<"index: "<<*index<<"\n";

	struct ParsedRequest *req;    // contains parsed request

	req = ParsedRequest_create();
	if (req->port == NULL)             // if port is not mentioned in URL, we take default as 80 
		{req->port = (char *) "80";}
     request_message[5000-1]='\0';
	if (ParsedRequest_parse(req, request_message, 5000) < 0) {		
	close(sockid);	return 0;
	}
    

	

	/*final request to be sent*/
	
	char*  browser_req  = convert_Request_to_string(req);		
	//cout<<browser_req;
	//cout<<"testing\n";
    //cout<<req->host<<" "<<req->path<<"\n";
    
	int remotesocketid;

	remotesocketid = CreatServerSocket(req->host, 80);
	
	// /* To run on IIIT proxy, comment above line and uncomment below line */
			
	// //char iiitproxy[] = "172.31.1.4";
	// //char iiitport[] = "8080";
	
	// // iServerfd = createserverSocket(iiitproxy, iiitport);

	// writeToserverSocket(browser_req, iServerfd, total_recieved_bits);
	// writeToClient(newsockfd, iServerfd);


	// ParsedRequest_destroy(req);

     int byte_send=send(remotesocketid,browser_req,strlen(browser_req),0);
     char buffer[100000];
     byte_send=recv(remotesocketid,buffer,100000-1,0);
     byte_send=send(sockid,buffer,byte_send,0);
     cout<<"sent back to the client\n";
	char *temp=(char*)malloc(sizeof(char)*byte_send+1);
     for (int i = 0; i < byte_send/sizeof(char); i++)
     {
          temp[i]=buffer[i];
     }
     
          Cache.add(url,t_bytes,temp,byte_send);
     
     
     
	close(remotesocketid);   // close the sockets
	close(sockid);

	//  Doesn't make any sense ..as to send something
	
mtx.unlock();
return 0;
}



int main() {
    WSADATA wsaData;
    SOCKET ListenSocket = INVALID_SOCKET;
    struct sockaddr_in serverAddr;
    vector<SOCKET> Client;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed" << std::endl;
        return 1;
    }

    // Create a socket
    ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ListenSocket == INVALID_SOCKET) {
        std::cerr << "Error creating socket: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Set up the sockaddr_in structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8080);

    // Bind the socket
    if (bind(ListenSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Bind failed: " << WSAGetLastError() << std::endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // Listen for incoming connections
    listen(ListenSocket, 5);
    std::cout << "Server is listening on port 8080..." << std::endl;

    
    vector<thread>pool(5);
    vector<mutex>mtx(5);
    int i=1;
    while (true) {


        // Check if a new connection is requested
       
            SOCKET ClientSocket = accept(ListenSocket, NULL, NULL);
            if (ClientSocket == INVALID_SOCKET) {
                std::cerr << "Accept failed: " << WSAGetLastError() << std::endl;
            } else {
                std::cout << "New client connected: " << ClientSocket << "\n";
                Client.push_back(ClientSocket);  // Add new client
            }
        
        while (++i)
        {
          i=i%5;
          if(mtx[i].try_lock())
          {
          cout<<"mtx available\n";
          
          
          pool[i]=thread(datafromclient,ClientSocket,ref(mtx[i]));
          
          
          break;
          }
          //cout<<i;
          
        }
     
    }

    // Cleanup
    closesocket(ListenSocket);
    WSACleanup();
    return 0;
}
