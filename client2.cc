#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <iostream>

using namespace std;

#define MESSAGE_LENGTH 1024
#define PORT 7777

int main() {
  int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (clientSocket == -1) {
    cerr << "Creation of Socket failed!" << endl;
    exit(1);
  }

  sockaddr_in serverAddr;
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  serverAddr.sin_port = htons(PORT);
  serverAddr.sin_family = AF_INET;

  if (connect(clientSocket, (struct sockaddr*)&serverAddr,
              sizeof(serverAddr)) == -1) {
    cerr << "Connection with the server failed!" << endl;
    close(clientSocket);
    exit(1);
  }

  cout << "Connected to the server as Client 2." << endl;

  while (true) {
    char message[MESSAGE_LENGTH];

    cout << "Enter the message you want to send to the server (Client 2): ";
    cin.getline(message, MESSAGE_LENGTH);

    ssize_t bytesSent = send(clientSocket, message, strlen(message), 0);
    if (bytesSent <= 0) {
      cerr << "Error sending message to the server!" << endl;
      break;
    }

    if (strncmp("end", message, 3) == 0) {
      cout << "Client 2 Exited." << endl;
      break;
    }

    bzero(message, MESSAGE_LENGTH);

    // Receive the message from the server
    ssize_t bytesRead = recv(clientSocket, message, sizeof(message), 0);
    if (bytesRead <= 0) {
      cerr << "Error receiving message from the server!" << endl;
      break;
    }

    cout << "Data received from the server: " << message << endl;
  }

  // Close the socket
  close(clientSocket);

  return 0;
}
