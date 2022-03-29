
#include <string>

int                 countChars(std::string str, char c)
{
    int count = 0;

    for (int i = 0; i < static_cast<int>(str.size()); ++i)
    {
        if (str[i] == c)
            ++count;
    }
    return (count);
}