#ifndef MASK_HPP
#define MASK_HPP

#include <string>

class Mask
{
    public:

                Mask(const std::string& mask);
                ~Mask(void);
    
        Mask&   operator=(const Mask& maskRef);

    private:

                Mask(void);

        const std::string   mask;

        static std::string  hostNameMaskPrefix;
        bool                isHostNameMask;
        bool                isWildCardMask;

        bool                m_isMaskValid(void);
        bool                m_isHostNameMask(void);
        bool                m_isWildCardMask(void);

        bool                m_isHostNameMatch(void) const;
        bool                m_isChannelMatch(void) const;
        bool                m_isNickNameMatch(void) const;

        class MaskNotValid: public std::exception
        {
            public:
                virtual const char* what(void) const throw() { return ("Mask not valid"); };
        };
};


#endif