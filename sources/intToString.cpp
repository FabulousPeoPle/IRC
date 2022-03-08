
#include <sstream>

std::string intToString(int num)
{
    std::stringstream ss;

    ss << num;
    return (ss.str());
}