#include "utils.hpp"

std::string CurrentTime()
{
    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    char buf[22] = { 0 };
    std::strftime(buf, sizeof(buf), "%T %Z %d/%m/%y", std::localtime(&now));
    
    return std::string(buf);
}