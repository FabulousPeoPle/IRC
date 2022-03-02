#include "Bot.hpp"

Bot::Bot(void)
{
    this->sock = -1;
    this->passwd = "";
    this->port = -1;
    this->userName = "";
    this->nickName = "";
    this->host = "";
}

Bot::~Bot(){}

void Bot::idBot(std::string userName, std::string nickName)
{
    this->userName = userName;
    this->nickName = nickName;
}

void Bot::idBot(std::string userName)
{
    this->userName = userName;
    this->nickName = userName;
}

int Bot::startBot(std::string host, int port, std::string passwd)
{
    if (this->userName == "" || this->nickName == "")
    {   
        printf("Bot don't have a Id \n");
        return -1;
    }
    this->host = host;
    this->port = port;
    this->passwd = passwd;
    if ((this->sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }
    this->serv_addr.sin_family = AF_INET;
    this->serv_addr.sin_port = htons(port);
    if(inet_pton(AF_INET, host.c_str(), &this->serv_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
    if (connect(this->sock, (struct sockaddr *)&this->serv_addr, sizeof(this->serv_addr)) < 0)
    {
        printf("Connection Failed");
        return -1;
    }
    this->m_auth();
    this->m_listner();
    return 0;
}

int Bot::m_auth(void)
{
    std::string user = "USER " + this->userName + " * * : hello World";
    std::string nick = "NICK " + this->nickName;
    int err = 0;
    std::string passwd = "PASS " + this->passwd;

    if ((err = m_send(user)))
        return err;
    if ((err = m_send(nick)))
        return err;
    if ((err = m_send(passwd)))
        return err;
	return 0;
}

int Bot::m_listner(void)
{
    std::queue<std::string> msg;
    while(1)
    {
        this->m_recv(msg);
        this->m_qHandler(msg);
    }
}

int Bot::m_send(std::string msg)
{
    int bytesSent = 0;
    msg += END_STRING;
    int size = msg.size();

    while (size)
    {
        // TODO: What the last parameter?
        bytesSent = send(this->sock, msg.data() + bytesSent, size, 0);
        if (bytesSent == -1)
            return (-1);
        size -= bytesSent;
    }
    return (0);
}

int Bot::m_recv(std::queue<std::string> &msg)
{
    char buffer[1024];
    int bytesRcev = 0;
	
    bytesRcev = recv(this->sock, buffer, 1023, 0);
    if (bytesRcev == 0)
        return (0);
	buffer[bytesRcev] = '\0';
    ssplit(std::string(buffer), END_STRING, msg);
    return (0);
}

int Bot::m_qHandler(std::queue<std::string> &msg_q)
{
    std::string msg;
    while (msg_q.empty() != true)
    {
        msg = msg_q.front();
	 	msg_q.pop();
		m_cmdHandler(msg);
    }
	return 0;
}

void Bot::m_cmdHandler(std::string msg)
{
	t_msg p_msg;
	int err = 0;

	if (m_msgParse(msg, p_msg) == 0)
    {
        std::cout << "pmsg -------- S" << std::endl;
        std::cout << p_msg.nick << std::endl;
        std::cout << p_msg.user << std::endl;
        std::cout << p_msg.host << std::endl;
        std::cout << p_msg.cmd << std::endl;
        std::cout << p_msg.args << std::endl;
        std::cout << p_msg.msg << std::endl;
        std::cout << "pmsg -------- E" << std::endl;
	    err = m_cmdDispatcher(p_msg);
    }
}

int Bot::m_invCmdHandler(t_msg &p_msg)
{
    std::vector<std::string> s_msg;

    ssplit(p_msg.msg, " ", s_msg);
    if (s_msg.size() != 1 || s_msg[0][0] != '#')
        return -1;
    m_send("JOIN " + s_msg[0]);
    m_privMsg(s_msg[0], "I shall thank you for the invite " + p_msg.nick);
    return 1;
}

int Bot::m_pmCmdHandler(t_msg &p_msg)
{
    std::vector<std::string> s_msg;
    std::string temp;

    ssplit(p_msg.msg, " ", s_msg);
    if (s_msg.size() < 1 || s_msg[0] != "!bota")
        return NOTACMD;
    else if (s_msg.size() == 1)
        return EC_USAGE;
    else if (s_msg.size() == 2)
    {
        if (s_msg[1] == "time")
            m_privMsg(p_msg.nick, "It's " + CurrentTime());
    }
    else if (s_msg.size() == 3)
    {
        if (s_msg[1] == "whatis")
        {
            wiki_desc(s_msg[2], temp);
            m_privMsg(p_msg.nick, temp);
        }
    }
    return 1;
}

int Bot::m_msgParse(std::string msg, t_msg &p_msg)
{
    std::cout << p_msg.cmd << std::endl;
	if ((p_msg.nick = strToken(msg.erase(0, 1), "!")).size() == 0)
			return -1;
	msg.erase(0, p_msg.nick.size());
	if ((p_msg.user = strToken(msg.erase(0, 1), "@")).size() == 0)
		return -2;
	msg.erase(0, p_msg.user.size());
	if ((p_msg.host = strToken(msg.erase(0, 1), " ")).size() == 0)
		return -3;
	msg.erase(0, p_msg.host.size());
	if ((p_msg.cmd = strToken(msg.erase(0, 1), " ")).size() == 0)
		return -4;
	msg.erase(0, p_msg.cmd.size());
	if ((p_msg.args = strToken(msg.erase(0, 1), ":")).size() == 0)
		return -5;
	msg.erase(0, p_msg.args.size() + 1);
	if ((p_msg.msg = msg).size() == 0)
		return -6;	
	msg.erase(0, p_msg.msg.size());
	trim(p_msg.nick, " \t\n\r");
	trim(p_msg.user, " \t\n\r");
	trim(p_msg.host, " \t\n\r");
	trim(p_msg.cmd, " \t\n\r");
    trim(p_msg.args, " \t\n\r");
	trim(p_msg.msg, " \t\n\r");
    return 0;
}

int Bot::m_cmdDispatcher(t_msg p_msg)
{
	std::vector<std::string> s_msg;
	std::string temp;

    std::cout << p_msg.cmd << std::endl;
    if (p_msg.cmd == "PRIVMSG")
        m_pmCmdHandler(p_msg);
    else if(p_msg.cmd == "INVITE")
        m_invCmdHandler(p_msg);
	return 0;
}

int Bot::m_privMsg(std::string to, std::string msg)
{
    m_send("PRIVMSG " + to + ":" + msg);
    return 0;
}