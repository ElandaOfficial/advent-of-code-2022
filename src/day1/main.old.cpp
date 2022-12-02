/**
    ====================================================================================================================
    
    This work is free. You can redistribute it and/or modify it under the
    terms of the Do What The Fuck You Want To Public License, Version 2,
    as published by Sam Hocevar. See the COPYING file or http://www.wtfpl.net/ 
    for more details.
    
    ====================================================================================================================
    
    @author Elanda
    @file   main.cpp
    @date   01, December 2022
    
    ====================================================================================================================
 */

#include <array>
#include <iostream>
#include <random>
#include <regex>
#include <vector>



//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
namespace
{
    //==================================================================================================================
    using namespace std::string_view_literals;
    
    //==================================================================================================================
    /** The name and number of elves, frantically searching for our beloved calories. */
    constexpr std::array elves {
        "Boogy"sv,
        "Grumpy"sv,
        "Stoopi-d"sv,
        "Big-G-Elfman"sv,
        "Yourmama-Stevenson"sv
    };
    
    /** Some welcoming phrases of Santa. */
    constexpr std::array introductionPhrases {
        "Welcome {} my friend, what have you gotten for us today?"sv,
        "What about you {}? What is it that's supposed to cease our hunger?"sv,
        "Can you please show us what you got for us {}?"sv,
        "Oh... oh it's you {}, ok... well weird, but anyway, what have you gotten for us?"sv,
        "Ah THERE they are, my beloved elf {}, please open your hands and show us everything that you have found!"sv,
        "Sup {}, what u got?"sv,
        "I don't want to be tsundere or so Mr. {}, but give me all you got or 360 no-scope - it's on you."sv,
        "So... you arrived at last my beloved underling {}, present to our eyes the secrets your hands might hide."sv
    };
    
    //==================================================================================================================
    [[nodiscard]]
    std::string getMessage(size_t phraseId, std::size_t elfId)
    {
        const std::regex pattern(R"(\{\})");
        std::string      output (introductionPhrases[phraseId]);
        std::smatch      match;
        
        while (std::regex_search(output, match, pattern))
        {
            output = match.prefix().str() + std::string(elves[elfId]) + match.suffix().str();
        }
        
        return output;
    }
    
    [[nodiscard]]
    std::vector<int> createIndices()
    {
        std::vector<int> result;
        std::generate_n(std::back_inserter(result), introductionPhrases.size(),
                        [counter = 0]() mutable 
                        {
                            return counter++;
                        });
        
        std::random_device rd;
        std::mt19937       g(rd());
        std::shuffle(result.begin(), result.end(), g);
        
        result.resize(elves.size());
        return result;
    }
}
//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************
// region main
//======================================================================================================================
int main(int argc, char **argv)
{
    using ElfInventory = std::vector<int>;
    
    struct ElfScore // NOLINT
    {
        std::size_t index;
        int         calories { -1 };
    };
    
    std::cout << "Ho ho ho, happy early christmas my dear underlings, namely, elves.\n"
                 "Unfortunately, before we can engage in distributing kids toys, we need to fill up our stamina with "
                     "plenty of calories.\n"
                 "For this very reason, I'd like you to help me pick up anything that could aid us in our quest,\n"
                 "I am going to dispatch 5 of you of which all of you will return with as many supplies as you can "
                     "find.\n"
                 "So go now, venture away and bring me the finest crops and fruits that you may get.\n"
                 "-- 23 days later --\n"
                 "Ho ho ho, there you are my dear underlings, please report what goods you have found.\n"
                 "What are these goods - supposed to fill our bellies such that the next day will be\n"
                 "another journey on our sanitary facilities.\n\n";
    
    const std::vector<int> indices = ::createIndices();
    
    ElfScore highscore;
    
    for (auto it = ::elves.begin(); it != elves.end(); ++it)
    {
        const std::size_t index  = std::distance(elves.begin(), it);
        const std::string phrase = ::getMessage(indices[index], index);
        
        std::cout << phrase << '\n'
                  << "(Enter calorie numbers line by line, you can have as many as you want. "
                     "Or leave empty to jump to the next elf.)\n" << std::flush;
        
        int calorie_count = 0;
        
        while (true)
        {
            std::string input;
            (void) std::getline(std::cin, input, '\n');
            
            if (input.empty())
            {
                break;
            }
            
            try
            {
                const int input_calories = std::stoi(input);
                
                if ((static_cast<std::size_t>(calorie_count) + input_calories) > std::numeric_limits<int>::max())
                {
                    std::cout << "Uiuiui, that is bit too much calories, let's ignore this item - go on!\n" << std::flush;
                    continue;
                }
                
                calorie_count += input_calories;
            }
            catch (const std::exception &ex)
            {
                std::cout << "Not a valid calorie or too high, try again:\n" << std::flush;
                continue;
            }
        }
        
        if (calorie_count > highscore.calories)
        {
            highscore = { index, calorie_count };
        }
    }
    
    std::cout << "\n\n"
                 "Wohoo ho ho, that is quite the food everyone.\n"
                 "Especially you " << ::elves[highscore.index] << ", you have done well with your "
                     << highscore.calories << " calories.\n\n"
                 "AND NOW LET'S BEGIN HO HO HO HO MERRY CHRISTMAS EVERYONE!\n";
    
    return 0;
}
//======================================================================================================================
// endregion main
//**********************************************************************************************************************
