
#include <ctype.h>

bool	isAlphaNum(char c)
{
    return (isalnum(c) || c == '_');
}

