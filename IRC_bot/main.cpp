#include "Bot.hpp"
#include <fcntl.h>
// using namespace std;

// int m_send(int toFd, std::string message)
// {
//     int bytesSent = 0;
//     message += END_STRING;
//     int size = message.size();

//     while (size)
//     {
//         // TODO: What the last parameter?
//         bytesSent = send(toFd, message.data() + bytesSent, size, 0);
//         if (bytesSent == -1)
//             return (-1);
//         size -= bytesSent;
//     }
//     return (0);
// }

// int m_recv(int toFd, queue<string> &msg)
// {
//     char buffer[1024];
//     int bytesRcev = 0;
//     string s;
	
//     bytesRcev = recv(toFd, buffer, 1023, 0);
//     if (bytesRcev == 0)
//         return (0);
// 	buffer[bytesRcev] = '\0';
// 	cout << "bs----" << buffer << "----be" << endl;
//     ssplit(string(buffer), END_STRING, msg);
//     return (0);
// }


// std::string CurrentTime()
// {
//     std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

//     char buf[22] = { 0 };
//     std::strftime(buf, sizeof(buf), "%T %Z %d/%m/%y", std::localtime(&now));
    
//     return std::string(buf);
// }

// int prviMsg(int sock, string to, string msg)
// {
//     m_send(sock, "PRIVMSG " + to + ":" + "It's "+ msg);

//     return 0;
// }


// // typedef struct s_msg
// // {
// // 	std::string user;
// // 	std::string nick;
// // 	std::string host;
// // 	std::string cmd;
// // 	std::string args;
// // 	std::string msg;
// // }t_msg;

// int msg_parse(std::string msg, t_msg &p_msg)
// {
// 	if (msg.find("!bota ") != std::string::npos)
// 	{
// 		if ((p_msg.nick = strToken(msg.erase(0, 1), "!")).size() == 0)
// 			return -1;
// 		msg.erase(0, p_msg.nick.size());
// 		if ((p_msg.user = strToken(msg.erase(0, 1), "@")).size() == 0)
// 			return -2;
// 		msg.erase(0, p_msg.user.size());
// 		if ((p_msg.host = strToken(msg.erase(0, 1), " ")).size() == 0)
// 			return -3;
// 		msg.erase(0, p_msg.host.size());
// 		if ((p_msg.cmd = strToken(msg.erase(0, 1), " ")).size() == 0)
// 			return -4;
// 		msg.erase(0, p_msg.cmd.size());
// 		if ((p_msg.args = strToken(msg.erase(0, 1), ":")).size() == 0)
// 			return -5;
// 		msg.erase(0, p_msg.args.size() + 1);
// 		if ((p_msg.msg = msg).size() == 0)
// 			return -6;	
// 		msg.erase(0, p_msg.msg.size());
// 		trim(p_msg.nick, " \t\n\r");
// 		trim(p_msg.user, " \t\n\r");
// 		trim(p_msg.host, " \t\n\r");
// 		trim(p_msg.cmd, " \t\n\r");
// 		trim(p_msg.args, " \t\n\r");
// 		trim(p_msg.msg, " \t\n\r");
// 	}
// 	else
// 		return -7;
// 	return 0;
// }

// int cmd_dispatcher(int sock, t_msg p_msg)
// {
// 	std::vector<std::string> s_msg;
// 	std::string temp;

// 	ssplit(p_msg.msg, " ", s_msg);
// 	if (s_msg.size() < 1 || s_msg[0] != "!bota")
// 		return NOTACMD;
// 	else if (s_msg.size() == 1)
// 		return EC_USAGE;
// 	else if (s_msg.size() == 2)
// 	{
// 		if (s_msg[1] == "time")
// 			prviMsg(sock, p_msg.nick, CurrentTime());
// 	}
// 	else if (s_msg.size() == 3)
// 	{
// 		if (s_msg[1] == "whatis")
// 		{
// 			wiki_desc(s_msg[2], temp);
// 			prviMsg(sock, p_msg.nick, temp);
// 		}
// 	}
// 	std::cout << s_msg[0] << std::endl;
// 	return 0;
// }

// void cmd_handler(int sock, string msg)
// {
// 	t_msg p_msg;
// 	int err = 0;
// 	//std::cout << msg << endl;
// 	if (!msg_parse(msg, p_msg))
// 	{
// 		std::cout << p_msg.nick << std::endl;
// 		std::cout << p_msg.user << std::endl;
// 		std::cout << p_msg.host << std::endl;
// 		std::cout << p_msg.cmd << std::endl;
// 		std::cout << p_msg.args << std::endl;
// 		std::cout << p_msg.msg << std::endl;
// 		err = cmd_dispatcher(sock, p_msg);
// 	}
// }

// int q_handler(int sock, queue<string> &msg_q)
// {
//     string msg;
//     while (msg_q.empty() != true)
//     {
//         msg = msg_q.front();
// 		//cout << "q >> " << msg << endl;
// 	 	msg_q.pop();
// 		cmd_handler(sock, msg);
//     }
// 	return 0;
// }

// bool auth(int sock, string user, string nick)
// {
//     std::string auth = "USER " + user + " * * : " + user + " is here" + END_STRING + "NICK " + nick + END_STRING + "PASS 133";
// 	return m_send(sock, auth);
// }

// bool ch_join(int sock, std::string channel_name)
// {
//     std::string msg = "JOIN #" + channel_name;
// 	return m_send(sock, msg);
// }

// bool lusers(int sock)
// {
//     std::string msg = "LUSERS";
// 	return m_send(sock, msg);
// }

int main(void)
{
	Bot bota;

	bota.idBot("bota");
	bota.startBot("0.0.0.0", 6667, "133");
}

// int main(int argc, char const *argv[])
// {
//     int sock = 0, valread;
//     struct sockaddr_in serv_addr;
//     char buffer[1024] = {0};
//     queue<string> msg;

// 	if (argc != 3)
// 	{
// 		std::cout << "usage : bota [ip] [port]" << endl;
// 		return 1;
// 	}
//     if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
//     {
//         printf("\n Socket creation error \n");
//         return -1;
//     }
//     /*int status = fcntl(sock, F_SETFL, O_NONBLOCK);
//     if (status == -1){
//         perror("calling fcntl");
//         // handle the error.  By the way, I've never seen fcntl fail in this way
//     }*/
//     serv_addr.sin_family = AF_INET;
//     serv_addr.sin_port = htons(atoi(argv[2]));
//     // Convert IPv4 and IPv6 addresses from text to binary form
//     if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0) 
//     {
//         printf("\nInvalid address/ Address not supported \n");
//         return -1;
//     }
   
//     if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
//     {
// 		perror("conect : ");
//         printf("\nConnection Failed \n\n");
//         return -1;
//     }
//     printf("\nConnection GOOD \n");
// 	auth(sock, "bota", "bota"); // returns 0 when working.
//     cout << "auth good ??" << std::endl;
// 	ch_join(sock, "ch1");
//     while(1)
//     {
//         m_recv(sock, msg);
// 		//cout << "yolo " << msg.size() << endl;
//         q_handler(sock, msg);
//         // cout << msg.front() << endl;
//         // cout << "nl" << endl;
//         // char buffer[1024] = {0};
//         // scanf("%s", buffer);
//         // auth = buffer;
//         // auth += END_STRING;
//         // m_send(sock, auth);
//         // read(sock, buffer, 1024);
//         // printf("%s\n",buffer );
//     }

//     // send(sock , auth1 , strlen(auth1) , 0 );


//     // printf("Hello message sent\n");
//     // valread = read( sock , buffer, 1024);
//     return 0;
// }
