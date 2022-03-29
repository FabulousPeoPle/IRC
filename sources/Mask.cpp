#include "Mask.hpp"

Mask::Mask(void) : isHostNameMask(false), isWildCardMask(false), hostNameMaskPrefixSize(5)
{
}

Mask::~Mask(void)
{
}

Mask::Mask(const std::string& mask) : mask(mask), isHostNameMask(false), isWildCardMask(false), hostNameMaskPrefixSize(5)
{
    if (!m_isMaskValid())
        throw MaskNotValid();
}

bool        Mask::m_isHostNameMask(void)
{
    if (this->mask == Mask::hostNameMaskPrefix)
        return (true);
    if (this->mask.substr(0, this->hostNameMaskPrefixSize) != Mask::hostNameMaskPrefix)
        return (false);
    if (this->mask.size() < MIN_FULL_MASK_SIZE)
        return (false);
    return (true);
}

bool        Mask::m_isWildCardMask(void)
{
    if (countChars(this->mask, '*') != 1)
        return (false);
    if (this->mask[0] != '*' && *(this->mask.end() - 1) != '*')
        return (false);
    return (true);
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
    return (false);
}

std::string    Mask::hostNameMaskPrefix = "*!*@*";