#include <iostream>
#include<stdio.h>
#define _WINSOCK_DEPRECATED_NO_WARNINGS
using namespace std;
#include <winsock2.h>
int main()
{
    // Define a structure to hold Winsock data
    WSADATA wsData;
    sockaddr_in serverAddr;


    // Initialize Winsock with version 2.2
    int nret = WSAStartup(MAKEWORD(2, 2), &wsData);
    if (nret < 0)
    {
        cout << "Failed to initialize Winsock" << endl;
        return -1;
    }

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);//open a socket listener

    // Check if the server socket creation was successful
    if (serverSocket < 0)
    {
        cout << "Failed to initialize listener socket" << endl;
        WSACleanup(); // Clean up Winsock
        return -1;
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("192.168.100.4");
    serverAddr.sin_port = htons(12345);
    memset(&(serverAddr.sin_zero), 0, 8);

    nret = connect(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (nret < 0)
    {
        cout << "Cannot connect to server!" << endl;
        return -1;
    }
    char buffer[1024];
    memset(buffer, 0, 1024);
    recv(serverSocket, buffer, sizeof(buffer), 0);
    cout << "Server says: " << buffer << endl;
    cout << endl << "Enter(upload) to Upload text in server or Enter(download) to Download text from the server" << endl;
    fgets(buffer, 1023, stdin);
    send(serverSocket, buffer, strlen(buffer) - 1, 0);
    bool isupload = false;
    while (true)
    {
        memset(buffer, 1024, 0);
        if (strcmp(buffer, "upload\n") == 0 || isupload == true)
        {
            isupload = true;
            cout << "Enter a message to upload or Enter (stop) to upload all previous data sent to server" << endl;
            fgets(buffer, 1023, stdin);
            send(serverSocket, buffer, strlen(buffer) - 1, 0);
            if(strcmp(buffer, "stop\n")==0)
                isupload=false;
        }
        if (isupload == false)
        {
            cout << endl << "Enter(upload) to Upload text in server or Enter(download) to Download text from the server" << endl;
            fgets(buffer, 1023, stdin);
        }
        if (strcmp(buffer, "download\n") == 0)
        {
            send(serverSocket, buffer, strlen(buffer) - 1, 0);
            recv(serverSocket, buffer, sizeof(buffer), 0);
            cout << buffer;
        }
   }


}
