#include "utils.hpp"

int ssplit(std::string str, std::string delim, std::vector<std::string> &msg)
{

    size_t pos = 0;  
	std::string token1; // define a string variable  
  
    // use find() function to get the position of the delimiters  
    while ((pos = str.find(delim)) != std::string::npos)  
    {

        token1 = str.substr(0, pos); // store the substring
        // cout << "token >>> " + token1 << endl;
        msg.push_back(token1);

        str.erase(0, pos + delim.length());  /* erase() function store the current positon and move to next token. */
    }
	msg.push_back(str);
    // cout << "split remain:  " + str + "||"<< endl;
    return 0;
}

int ssplit(std::string str, std::string delim, std::queue<std::string> &msg)
{

    size_t pos = 0;  
	std::string token1; // define a string variable  
  
    // use find() function to get the position of the delimiters  
    while ((pos = str.find(delim)) != std::string::npos)  
    {
        token1 = str.substr(0, pos); // store the substring
        // cout << "token >>> " + token1 << endl;
        msg.push(token1);

        str.erase(0, pos + delim.length());  /* erase() function store the current positon and move to next token. */
    }
	if (str.size() != 0)
		msg.push(str);
    // cout << "split remain:  " + str + "||"<< endl;
    return 0;
}

std::string trim(std::string& s, const char* t)
{
    s.erase(s.find_last_not_of(t) + 1);
    s.erase(0, s.find_first_not_of(t));
    return s;
}

std::string strToken(std::string str, std::string  delim)
{    
    static std::string  strDup = str;

    if (str.size())
        strDup = str;

    if (!strDup.size())
        return "";

    int found = strDup.find(delim);

    if (static_cast<unsigned long>(found) == std::string::npos)
    {
        std::string toReturn = strDup;
        strDup = "";
        return (toReturn);
    }

    std::string toReturn = strDup.substr(0, found + delim.size() - 1);
    
    strDup = strDup.substr(found + delim.size() - 1, std::string::npos);
    return (toReturn);
}