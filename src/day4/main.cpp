/**
    ====================================================================================================================
    
    This work is free. You can redistribute it and/or modify it under the
    terms of the Do What The Fuck You Want To Public License, Version 2,
    as published by Sam Hocevar. See the COPYING file or http://www.wtfpl.net/ 
    for more details.
    
    ====================================================================================================================
    
    @author Elanda
    @file   main.cpp
    @date   04, December 2022
    
    ====================================================================================================================
 */

#include <array>
#include <fstream>
#include <iostream>
#include <string>



//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
namespace
{
    //==================================================================================================================
    // A range of section ids
    struct Section
    {
        int startNr;
        int endNr;
    };
    
    // A pair of elves that define their sections
    struct ElfPair
    {
        //==============================================================================================================
        Section sectionElf1;
        Section sectionElf2;
        
        //==============================================================================================================
        [[nodiscard]]
        constexpr bool containsContained() const noexcept
        {
            return ((sectionElf1.startNr >= sectionElf2.startNr && sectionElf1.endNr <= sectionElf2.endNr)
                 || (sectionElf2.startNr >= sectionElf1.startNr && sectionElf2.endNr <= sectionElf1.endNr));
        }
        
        [[nodiscard]]
        constexpr bool intersects() const noexcept
        {
            return ((sectionElf1.endNr >= sectionElf2.startNr && sectionElf2.endNr >= sectionElf1.endNr)
                 || (sectionElf2.endNr >= sectionElf1.startNr && sectionElf1.endNr >= sectionElf2.endNr));
        }
        
        //==============================================================================================================
        [[nodiscard]]
        static ElfPair fromString(const std::string &input) noexcept
        {
            ElfPair pair;
            
            const std::array vars {
                &pair.sectionElf1.startNr,
                &pair.sectionElf1.endNr,
                &pair.sectionElf2.startNr,
                &pair.sectionElf2.endNr
            };
            auto var_it = vars.begin();
            
            for (auto it = input.begin(); it != input.end(); ++it)
            {
                std::array<char, 4> buffer { *it };
                auto buf_it = buffer.begin() + 1;
                
                for (; (std::next(it) != input.end() && *it >= '0' && *it <= '9'); ++it)
                {
                    *buf_it++ = *std::next(it);
                }
                
                const int result = std::strtol(buffer.data(), nullptr, 10);
                *(*var_it++) = result;
            }
            
            return pair;
        }
    };
}
//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************
// region Main
//======================================================================================================================
int main()
{
    std::fstream input(INPUT_FILE);
    
    if (!input.is_open())
    {
        return 1;
    }
    
    std::string line;
    int         contained_count = 0;
    int         intersect_count = 0;
    
    while (std::getline(input, line))
    {
        const ::ElfPair pair = ::ElfPair::fromString(line);
        
        if (pair.containsContained())
        {
            (void) ++contained_count;
        }
        
        if (pair.intersects())
        {
            (void) ++intersect_count;
        }
    }
    
    std::cout << "In "       << contained_count << " pairs there is a significant containment, reporter states.\n";
    std::cout << "At least " << intersect_count << " of the pairs intersect section-wise, not good dawg.\n";
    return 0;
}
//======================================================================================================================
// endregion Main
//**********************************************************************************************************************
