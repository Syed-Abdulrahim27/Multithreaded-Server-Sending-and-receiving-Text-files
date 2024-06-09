#include <iostream>
#include <winsock2.h>
#include<vector>
#include<fstream>
using namespace std;
const int MAX_CLIENTS = 10;

DWORD WINAPI HandleClient(LPVOID lpParam);
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

    int Serversocket = socket(AF_INET, SOCK_STREAM, 0);//open a socket listener

    // Check if the server socket creation was successful
    if (Serversocket < 0)
    {
        std::cerr << "Failed to initialize listener socket" << endl;
        WSACleanup(); // Clean up Winsock
        return -1;
    }


    // Create a sockaddr_in structure to define server details 
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY; // Bind to any available network interface
    serverAddr.sin_port = htons(12345); // Bind to port 12345
    memset(&(serverAddr.sin_zero), 0, 8);

    //Bind the socket to the specified address and port
    nret = bind(Serversocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (nret < 0)
    {
        std::cerr << "Bind failed" << std::endl;
        //closesocket(serverSocket); // Close the socket 
        WSACleanup(); // Clean up Winsock
        return -1;
    }

    //Start listening for incoming connections with a maximum backlog queue size
    nret = listen(Serversocket, 5);
    if (nret < 0)
    {
        cout << "Listen failed" << endl;
        //closesocket(serverSocket); // Close the socket
        WSACleanup(); // Clean up Winsock
        return -1;
    }
    else
        cout << "Server is listening on port 12345..." << endl;

    //accepting client request
    wprintf(L"Waiting for client to connect...\n");

    vector<HANDLE> clientThreads;

    while (true)
    {
        sockaddr_in ClientAddr;
        int Clientaddrlen = sizeof(ClientAddr);
        SOCKET Clientsocket = accept(Serversocket, (struct sockaddr*)&ClientAddr, &Clientaddrlen);
        if (Clientsocket == INVALID_SOCKET)
        {
            cout << "accept() failed: " << endl;
            //closesocket(serverSocket);
            WSACleanup();
            return -1;
        }
        cout << "Client connected" << endl;
        const char* responseMessage = "Hello from the server";
        int sendResult = send(Clientsocket, responseMessage, strlen(responseMessage), 0);
        if (sendResult == SOCKET_ERROR) {
            cout << "send() failed: " << endl;
        }
        else {
            cout << "Hello message sent" << endl;
        }
        HANDLE hThread = CreateThread(NULL, 0, HandleClient,reinterpret_cast<LPVOID>(Clientsocket), 0, NULL);
        clientThreads.push_back(hThread);
        CloseHandle(hThread);
    }


    closesocket(Serversocket);
    WSACleanup();
}


DWORD WINAPI HandleClient(LPVOID lpParam)
{
    SOCKET clientSocket = reinterpret_cast<SOCKET>(lpParam);
    char buffer[1024];
    int bytesReceived=0;
    while (1)
    {
        memset(buffer, 0, 1024);//used for cleaning the buffer
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) 
        {
            if (bytesReceived == 0) 
            {
                // Client has closed the connection
                cout << "Client disconnected" << endl;
            }
            else 
            {
                cout << "receive message failed: " << endl;
            }
            break; // Exit the loop
        }

        if (strcmp(buffer, "upload") == 0)
        {
            ofstream outputfile("Text.txt");
            while (true)
            {
                memset(buffer, 0, 1024);
                bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
                if (bytesReceived <= 0)
                    break; // Exit the loop
                
                if (strcmp(buffer, "stop")==0)
                    break;
                int sizeofdata = 0;
                for (int i = 0; buffer[i] != 0; i++)
                    sizeofdata++;
                outputfile.write(buffer, sizeofdata);
                const char* nextline = "\n";
                outputfile.write(nextline, 1);
            }
            outputfile.close();
            cout << "File uploaded Successfully"<<endl;
        }
        else if (strcmp(buffer, "download") == 0)
        {
           /* memset(buffer, 0, 1024);
            const char* responseMessage = "Enter filename to download?";
            int sendResult = send(clientSocket, responseMessage, strlen(responseMessage), 0);
            bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);*/

            string filename = "Text.txt";
            ifstream inputfile(filename);
            if (!inputfile.is_open())
            {
                closesocket(clientSocket);
                return 1;
            }

            while (true)
            {
                inputfile.read(buffer, sizeof(buffer));
                int bytesRead = inputfile.gcount();
                if (bytesRead <= 0)
                    break;
                send(clientSocket, buffer, bytesRead, 0);
            }
            inputfile.close();
            cout << "File sent to client!" << endl;
        }

        // Process and print received data
        cout  << buffer;

        // Echo the data back to the client
       // send(clientSocket, buffer, bytesReceived, 0);
    }
    closesocket(clientSocket);
    return 0;
}

