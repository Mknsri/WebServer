#include <iostream>
#include <WinSock2.h>

#pragma comment(lib, "Ws2_32.lib")

#define LISTEN_PORT 80
#define BACKLOG 5
#define PROTOCOL "HTTP/1.1"


int main(int argc, char *argv[]) {

	const int requiredWinSockVersion = 2;

	WSADATA wsaData;

	if (WSAStartup(MAKEWORD(requiredWinSockVersion, 0), &wsaData) == NO_ERROR) {
		if (LOBYTE(wsaData.wVersion) >= requiredWinSockVersion) {
			// Create socket
			SOCKET listenSocket;
			listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			if (listenSocket == INVALID_SOCKET) {
				std::cout << "Socket creation failed" << WSAGetLastError() << std::endl;
			} else {
				sockaddr_in serverService;
				serverService.sin_family = AF_INET;
				serverService.sin_addr.s_addr = INADDR_ANY;
				serverService.sin_port = htons(LISTEN_PORT);

				if (bind(listenSocket, (SOCKADDR*)&serverService, sizeof(serverService)) != SOCKET_ERROR) {
					if (listen(listenSocket, BACKLOG) != SOCKET_ERROR) {
						// Creating the socket  OK,
						// Server actions start here
						std::cout << "Server started..." << std::endl;
						bool running = true;
						while (running) {
							sockaddr_in remoteAddress;
							int remoteAddressLen = sizeof(remoteAddress);
							SOCKET remoteSocket;

							remoteSocket = accept(listenSocket, (sockaddr*)&remoteAddress, &remoteAddressLen);
							if (remoteSocket != SOCKET_ERROR) {
								std::cout << "Client connecting " << inet_ntoa(remoteAddress.sin_addr) << ":" 
										  << ntohs(remoteAddress.sin_port) << std::endl;

								char HTMLMessage[] = "\r\n\r\n<html><body><h1>Ei suotta elämys</h1></body></html>";
								char sendBuffer[1000];
								char recvBuffer[1000];
								int bytesSent = 0;
								int bytesRecv = 0;

								// Receive from client
								bytesRecv = recv(remoteSocket, recvBuffer, (int)sizeof(recvBuffer), 0);
								if (bytesRecv != SOCKET_ERROR) {
									recvBuffer[bytesRecv] = '\0';
								}
								else {
									std::cout << "Error receiving: " << WSAGetLastError() << std::endl;
									closesocket(listenSocket);
									WSACleanup();	
								}

								std::cout << "Server received message: " << recvBuffer << "From: "  
										  << inet_ntoa(remoteAddress.sin_addr) << ":" << ntohs(remoteAddress.sin_port) 
										  << "\n" << "Bytes received: " << bytesRecv << std::endl;

								// Send response
								sprintf_s(sendBuffer,"%s %d %s", PROTOCOL, 200, "OK");
								bytesSent = send(remoteSocket, sendBuffer, (int)strlen(sendBuffer)+1, 0);
								if (bytesSent != SOCKET_ERROR) {
									sendBuffer[bytesSent] = '\0';
									std::cout << "Server sent message: " << sendBuffer << std::endl;
								} else {
									std::cout << "Error sending response: " << WSAGetLastError() << std::endl;
									closesocket(listenSocket);
									WSACleanup();	
								}

								// Send content
								bytesSent = send(remoteSocket, HTMLMessage, (int)strlen(HTMLMessage), 0);
								if (bytesSent != SOCKET_ERROR) {
									HTMLMessage[bytesSent] = '\0';
									std::cout << "Server sent message: " << HTMLMessage << std::endl;
									closesocket(remoteSocket);
								} else {
									std::cout << "Error sending content: " << WSAGetLastError() << std::endl;
									closesocket(listenSocket);
									WSACleanup();
								}

							} else {
								std::cout<< "Error accepting connection:" << WSAGetLastError() << std::endl;
								closesocket(listenSocket);
								WSACleanup();
							}


						}
					} else {
						std::cout<< "Error starting listening:" << WSAGetLastError() << std::endl;
						closesocket(listenSocket);
						WSACleanup();
					}
				} else {
					std::cout << "Error binding socket: " << WSAGetLastError() << std::endl;
					closesocket(listenSocket);
					WSACleanup();
				}
			}
		} else {
			std::cout << "Required Winsock version (" << requiredWinSockVersion << ")is not available" << std::endl;
		}

		if (WSACleanup() != 0) {
			std::cout << "Cleanup failed: " << WSAGetLastError() << std::endl;
		}

	} else
	{
		std::cout << "Startup failed: " << WSAGetLastError() << std::endl;
	}

	return 0;
}