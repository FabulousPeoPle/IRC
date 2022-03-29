#ifndef MASK_HPP
#define MASK_HPP

#include <string>

// TODO: utils should have their own hpp
int         countChars(std::string str, char c);


// the mask should be [ *!*@*.smth ], so it should at least have size 7
#define MIN_FULL_MASK_SIZE 7

class Mask
{
    public:

                Mask(const std::string& mask);
                ~Mask(void);
    
        Mask&   operator=(const Mask& maskRef);

    private:

                Mask(void);

        const std::string   mask;



        bool                isHostNameMask;
        bool                isWildCardMask;

        static std::string  hostNameMaskPrefix;
        const int           hostNameMaskPrefixSize;

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