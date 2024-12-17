#include "WinSock2.h";
#include <Ws2tcpip.h>
#include <iostream>;


class Server
{
public:

	SOCKET serverSocket = INVALID_SOCKET;
	SOCKET acceptSocket = INVALID_SOCKET;
	const int PORT = 35355;
	WSADATA wsaData;
	int wsaerr;
	WORD wVersionRequested = MAKEWORD(2, 2);


public:

	Server()
	{
		wsaerr = WSAStartup(wVersionRequested, &wsaData);
		if (wsaerr != 0)
		{
			std::cout << "Winsock dll not found" << std::endl;
			return;
		}
		std::cout << "Winsock dll found..." << std::endl;
		std::cout << "Status: " << wsaData.szSystemStatus << std::endl;

		serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (serverSocket == INVALID_SOCKET)
		{
			std::cout << "Invalid socket: " << WSAGetLastError() << std::endl;
			WSACleanup();
			return;
		}
		std::cout << "Socket is ready..." << std::endl;

		sockaddr_in service;
		service.sin_family = AF_INET;
		InetPtonA(AF_INET, std::string("0.0.0.0").c_str(), &service.sin_addr.s_addr);

		//service.sin_addr.s_addr = INADDR_ANY;
		service.sin_port = htons(PORT);

		if (bind(serverSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR)
		{
			std::cout << "Binding failed: " << WSAGetLastError() << std::endl;
			closesocket(serverSocket);
			WSACleanup();
			return;
		}
		std::cout << "Binding successful..." << std::endl;

		if (listen(serverSocket, 1) == SOCKET_ERROR)
		{
			std::cout << "Error listening on socket: " << WSAGetLastError() << std::endl;
		}

		std::cout << "Listening is on. Waiting for connections..." << std::endl;

		acceptSocket = accept(serverSocket, NULL, NULL);
		if (acceptSocket == INVALID_SOCKET)
		{
			std::cout << "Acception failed: " << WSAGetLastError() << std::endl;
			WSACleanup();
			return;
		}
		std::cout << "Connection accepted..." << std::endl;

		char welcomeMsg[] = "Welcome to server!";
		send(acceptSocket, welcomeMsg, sizeof(welcomeMsg), 0);

	}

	bool ServerLoop()
	{
		char buffer[200];

		int byteCount = recv(acceptSocket, buffer, 200, 0);
		if (byteCount < 0)
		{
			std::cout << "No information received: " << WSAGetLastError() << std::endl;
			return false;
		}
		else if (byteCount == 0)
		{
			std::cout << "No information received: (Client disconnected)" << std::endl;
			return false;
		}

		std::cout << "Data received: " << buffer << std::endl;
		return true;
	}

	void CloseServer()
	{
		std::cout << "Closing the server..." << std::endl;
		closesocket(serverSocket);
		WSACleanup();
	}

	~Server()
	{
		std::cout << "Server shutted down!" << std::endl;
	}
};

int main()
{
	Server* server = new Server();

	bool isRunning = true;
	while (isRunning) 
	{
		isRunning = server->ServerLoop();
	}
	server->CloseServer();
	delete server;

	std::cin.get();
}