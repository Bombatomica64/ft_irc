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
	public:
		MockClient(std::string server_ip, int server_port) 
		{
			m_clientSocket = socket(AF_INET, SOCK_STREAM, 0);
			if (m_clientSocket < 0)
				throw std::runtime_error("Error creating socket");
			m_clientAddr.sin_family = AF_INET;
			m_clientAddr.sin_port = htons(server_port);
			if (inet_pton(AF_INET, server_ip.c_str(), &m_clientAddr.sin_addr) <= 0)
				throw std::runtime_error("Invalid address");
			if (connect(m_clientSocket, (struct sockaddr *)&m_clientAddr, sizeof(m_clientAddr)) < 0)
				throw std::runtime_error("Connection failed");
		}
		~MockClient() 
		{
			close(m_clientSocket);
		}
		std::string receive( void) 
		{
			char buffer[1024] = {0};
			read(m_clientSocket, buffer, 1024);
			return std::string(buffer);
		}
		void send(std::string message) 
		{
			::send(m_clientSocket, message.c_str(), message.length(), 0);
		}
};

// Test fixture for the IRC server
class ServerTest : public ::testing::Test {
protected:
    void SetUp() override 
	{
		std::string argv[] = {"./server", "6667", "2"};
        server = new Server(argv[1], argv[2]);
		client = new MockClient(server->get_ip(), server->get_port());
    }

    void TearDown() override {
        delete server;
    }

private:
    Server* server;
	MockClient* client;
};

// Test case for the
TEST(ServerTest, basic)
{
	client->send("PASS :2\r\n");
	client->send("NICK test\r\n");
	client->send("USER test 0 * :test\r\n");
	client->receive();

	client->send("NICK test\r\n");
	EXPECT_EQ(client->receive(), ":irc 433 test :\r\n");
	// EXPECT_EQ(client->receive(), ":irc 431 :No nickname given\r\n");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}