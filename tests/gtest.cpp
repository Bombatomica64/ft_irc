#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <Channel.hpp>

// MOCKMETHOD( void, join_channel, (Client& client, std::string parameters), (override) );
// Mock class for network communication
class MockClient
{
private:
	int m_clientSocket;
	struct sockaddr_in m_clientAddr;
	int m_serverSocket;
	struct sockaddr_in m_serverAddr;

public:
	MockClient(std::string server_ip, int server_port)
	{
		std::cerr << "server_ip: " << server_ip << std::endl;
		m_clientSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (m_clientSocket == -1)
			throw std::runtime_error("Failed to create socket.");
		m_clientAddr.sin_family = AF_INET;
		m_clientAddr.sin_port = htons(6667);
		if (inet_pton(AF_INET, server_ip.c_str(), &(m_clientAddr.sin_addr)) <= 0)
			throw std::runtime_error("Invalid address/Address not supported.");
		if (connect(m_clientSocket, (struct sockaddr *)&m_clientAddr, sizeof(m_clientAddr)) < 0)
			throw std::runtime_error("Connection failed.");
	}
	~MockClient()
	{
		close(m_clientSocket);
	}
	std::string receive(void)
	{
		std::cout << " ci arrivo" << std::endl;
		char buffer[1024] = {0};
		ssize_t bytes = 10;
		std::string response;
		while ((bytes = recv(m_clientSocket, buffer, 1, 0)) > 0)
		{
			std::cout << "Received: " << buffer << std::endl;
			response += buffer;
		}
		std::cout << "Received: " << response << std::endl;
		return response;
	}
	void send(std::string message)
	{
		::send(m_clientSocket, message.c_str(), message.length(), 0);
	}
};

// Test fixture for the IRC server
class ServerTest : public ::testing::Test
{
protected:
	void SetUp() override
	{
		server_pid = fork();
		if (server_pid == 0)
		{
			int fd = open("logs/server.log", O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd != -1)
            {
                dup2(fd, STDOUT_FILENO);
                close(fd);
			}
			Server server("6667", "password");
			pause();
			exit(0);
		}
		else if (server_pid > 0)
		{
			sleep(1);
			client = new MockClient("10.11.1.5", 6667);
		}
		else
		{
			// Fork failed
			std::cerr << "Failed to fork server process" << std::endl;
			exit(1);
		}
	}

	void TearDown() override
	{
		delete client;
		if (server_pid > 0)
		{
			// Terminate the server process
			kill(server_pid, SIGTERM);
			waitpid(server_pid, nullptr, 0);
		}
	}

protected:
	MockClient *client;
	pid_t server_pid;
};

// Test case for the
TEST_F(ServerTest, basicLogin)
{
	if (server_pid == 0)
		return;
	client->send("PASS :2\r\n");
	client->send("NICK test\r\n");
	client->send("USER test 0 * :test\r\n");
	std::string response = client->receive();

	client->send("NICK test2\r\n");
	response = client->receive();
	EXPECT_EQ(response, ":test2 NICK test2 \r\n");
	kill(server_pid, SIGTERM);
}

int main(int argc, char **argv)
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}