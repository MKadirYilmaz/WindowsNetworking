#include "WinSock2.h";
#include <Ws2tcpip.h>
#include <iostream>

class Client
{


public:
	SOCKET clientSocket = INVALID_SOCKET;
	const int PORT = 35355;
	WSAData wsaData;
	int wsaerr;
	WORD wVersionRequired = MAKEWORD(2, 2);

private:
	bool bConnectionEstablished = false;

public:
	Client(std::string serverIP)
	{
		wsaerr = WSAStartup(wVersionRequired, &wsaData);
		if (wsaerr != 0)
		{
			std::cout << "Winsock dll not found" << std::endl;
			return;
		}
		std::cout << "Winsock dll found..." << std::endl;
		std::cout << "Status: " << wsaData.szSystemStatus << std::endl;

		clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (clientSocket == INVALID_SOCKET)
		{
			std::cout << "Invalid socket: " << WSAGetLastError() << std::endl;
			WSACleanup();
			return;
		}
		std::cout << "Socket is ready..." << std::endl;

		sockaddr_in clientService;
		clientService.sin_family = AF_INET;
		InetPtonA(AF_INET, std::string(serverIP).c_str(), &clientService.sin_addr.s_addr);
		clientService.sin_port = htons(PORT);
		
		if (connect(clientSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR)
		{
			std::cout << "Client connection failed: Failed to connect..." << std::endl;
			WSACleanup();
			return;
		}
		std::cout << "Client connection succeed..." << std::endl;
		bConnectionEstablished = true;
		char buffer[200];
		int recvBytes = recv(clientSocket, buffer, 200, 0);
		if (recvBytes == SOCKET_ERROR)
		{
			std::cout << "Server response not received: " << WSAGetLastError() << std::endl;
		}
		else
		{
			std::cout << "Server response: " << buffer << std::endl;
		}
		std::cout << "Client can start sending and receiving data..." << std::endl;

	}

	bool NetworkLoop()
	{

		char buffer[200];
		std::cout << "Enter your message: (Enter 0 to exit)" << std::endl;
		std::cin.getline(buffer, 200);
		switch (buffer[0])
		{
		case '0':
			EndSession();
			return false;
		default:
			SendData<char[]>(buffer, sizeof(buffer));
			return true;
			
		}

	}
	template <typename T>
	void SendData(T data, int pcktSize)
	{
		int byteCount = send(clientSocket, (char*)data, pcktSize, 0);
		if (byteCount == SOCKET_ERROR)
		{
			std::cout << "Data couldn't send: " << WSAGetLastError() << std::endl;
			return;
		}
		std::cout << "Bytes send: " << byteCount << std::endl;
	}

	void EndSession()
	{
		std::cout << "Ending the session..." << std::endl;
		closesocket(clientSocket);
		WSACleanup();
	}

	bool IsConnectionValid() const
	{
		return bConnectionEstablished;
	}

	~Client()
	{
		std::cout << "Client session shutted down!" << std::endl;
	}
};

int main()
{
	std::string serverIP;
	std::cout << "Please enter the ip adress of the server to connect: " << std::endl;
	std::cin >> serverIP;
	std::cin.get();
	Client* client = new Client(serverIP);

	bool isRunning = client->IsConnectionValid();
	while (isRunning)
	{
		isRunning = client->NetworkLoop();
	}

	delete client;

	std::cin.get();
}