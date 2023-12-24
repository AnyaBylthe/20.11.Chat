#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>

using namespace std;

#define MESSAGE_LENGTH 1024
#define PORT 7777

int main() {
  int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocket == -1) {
    cerr << "Socket creation failed!" << endl;
    exit(1);
  }

  sockaddr_in serverAddr;
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serverAddr.sin_port = htons(PORT);
  serverAddr.sin_family = AF_INET;

  if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) ==
      -1) {
    cerr << "Socket binding failed!" << endl;
    close(serverSocket);
    exit(1);
  }

  if (listen(serverSocket, 5) == -1) {
    cerr << "Socket is unable to listen for new connections!" << endl;
    close(serverSocket);
    exit(1);
  } else {
    cout << "Server is listening for new connections..." << endl;
  }

  sockaddr_in client1Addr, client2Addr;
  socklen_t client1Length, client2Length;

  int client1Socket =
      accept(serverSocket, (struct sockaddr*)&client1Addr, &client1Length);
  if (client1Socket == -1) {
    cerr << "Server is unable to accept data from client 1!" << endl;
    close(serverSocket);
    exit(1);
  }

  cout << "Connection established with client 1: "
       << inet_ntoa(client1Addr.sin_addr) << ":" << ntohs(client1Addr.sin_port)
       << endl;

  int client2Socket =
      accept(serverSocket, (struct sockaddr*)&client2Addr, &client2Length);
  if (client2Socket == -1) {
    cerr << "Server is unable to accept data from client 2!" << endl;
    close(serverSocket);
    exit(1);
  }

  cout << "Connection established with client 2: "
       << inet_ntoa(client2Addr.sin_addr) << ":" << ntohs(client2Addr.sin_port)
       << endl;

  close(serverSocket);

  // Communication Establishment
  while (true) {
    char message[MESSAGE_LENGTH];

    // Receive from client 1
    bzero(message, MESSAGE_LENGTH);
    read(client1Socket, message, sizeof(message));
    if (strncmp("end", message, 3) == 0) {
      cout << "Client 1 Exited." << endl;
      break;
    }
    cout << "Data received from client 1: " << message << endl;

    // Send to client 2
    ssize_t bytesSent = write(client2Socket, message, sizeof(message));
    if (bytesSent <= 0) {
      cerr << "Error sending message to client 2!" << endl;
      break;
    }

    // Receive from client 2
    bzero(message, MESSAGE_LENGTH);
    read(client2Socket, message, sizeof(message));
    if (strncmp("end", message, 3) == 0) {
      cout << "Client 2 Exited." << endl;
      break;
    }
    cout << "Data received from client 2: " << message << endl;

    // Send to client 1
    bytesSent = write(client1Socket, message, sizeof(message));
    if (bytesSent <= 0) {
      cerr << "Error sending message to client 1!" << endl;
      break;
    }
  }

  // Close sockets
  close(client1Socket);
  close(client2Socket);

  return 0;
}
