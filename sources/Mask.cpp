#include "Mask.hpp"

Mask::Mask(void) : isHostNameMask(false), isWildCardMask(false)
{
}

Mask::~Mask(void)
{
}

Mask::Mask(const std::string& mask) : mask(mask), isHostNameMask(false), isWildCardMask(false)
{
    if (!m_isMaskValid())
        throw MaskNotValid();
}


bool        Mask::m_isMaskValid(void)
{
    if (m_isHostNameMask())
    {
        this->isHostNameMask = true;
        return (true);
    }
    else if (m_isWildCardMask())
    {
        this->isWildCardMask = true;
        return (true);
    }
    return (false)
}

std::string    Mask::hostNameMaskPrefix = "*!*@*";