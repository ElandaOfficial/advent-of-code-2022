/**
    ====================================================================================================================
    
    This work is free. You can redistribute it and/or modify it under the
    terms of the Do What The Fuck You Want To Public License, Version 2,
    as published by Sam Hocevar. See the COPYING file or http://www.wtfpl.net/ 
    for more details.
    
    ====================================================================================================================
    
    @author Elanda
    @file   main.cpp
    @date   02, December 2022
    
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
    /** The character index of the opponents figure. */
    constexpr int offsetOpponent = 0;
    
    /** The character index of my figure. */
    constexpr int offsetMe = 2;
    
    //==================================================================================================================
    struct Round
    {
        //==============================================================================================================
        char opponent;
        char me;
        
        //==============================================================================================================
        [[nodiscard]]
        static Round fromInput(const std::string &input) noexcept
        {
            // This only works because we know that every line of input is consistent
            // Otherwise we would probably do this either by tokenisation or regexing
            return { static_cast<char>(input[offsetOpponent] - 'A' + 1), static_cast<char>(input[offsetMe] - 'X' + 1) };
        }
    };
    
    //==================================================================================================================
    // Today I learned, the modulo '%' operator is not actually doing "modulo" but "remainder" instead (this seems to be the case for every language)
    // So I had to search for a real modulo implementation on the webs: https://stackoverflow.com/a/12277233/13170332
    [[nodiscard]]
    constexpr long modulo(long a, long b) noexcept
    {
        return (a % b + b) % b;
    }
    
    //==================================================================================================================
    constexpr std::array<int(*)(int), 3> strategies {
        // lose
        [](int figure) -> int
        {
            return modulo((figure - 2), 3) + 1;
        },
        
        // draw
        [](int figure) -> int
        {
            return figure;
        },
        
        // win
        [](int figure) -> int
        {
            return ((figure % 3) + 1);
        }
    };
    
    //==================================================================================================================
    [[nodiscard]]
    constexpr int calculateResult(int figureOpponent, int figureMe) noexcept
    {
        const int factor = ::modulo(((figureOpponent % 3) - figureMe), 3) + 1;
        return 3 * (factor % 3);
    }
}
//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************
// region Main
//======================================================================================================================
int main()
{
    std::fstream file(INPUT_FILE);
    std::string  input;
    
    int opponent_points = 0;
    int my_points       = 0;
    
    int opponent_actual_points = 0;
    int my_actual_points       = 0;
    
    while (std::getline(file, input))
    {
        const ::Round round = ::Round::fromInput(input);
        
        // The drama of guessing what a column means
        const int result = ::calculateResult(round.opponent, round.me);
        opponent_points += (round.opponent + result);
        my_points       += (round.me       + (6 - result));
        
        // The drama of then actually hearing what it really is about
        const int needed_fig = strategies[round.me - 1](round.opponent);
        
        // Would probably be better to cache all possible results inside a lookup table and make a lookup instead
        // of calculating it everytime again, but... idrc
        const int result_2 = ::calculateResult(round.opponent, needed_fig);
        
        opponent_actual_points += (round.opponent + result_2);
        my_actual_points       += (needed_fig     + (6 - result_2));
    }
    
    std::cout << "I have won with " << my_points << " points, hooray! (opponent has: " << opponent_points << ")\n";
    std::cout << "For real though, actually I won with " << my_actual_points
                  << " points, hooray! (opponent has: "  << opponent_actual_points << ')';
    
    return 0;
}
//======================================================================================================================
// endregion Main
//**********************************************************************************************************************
