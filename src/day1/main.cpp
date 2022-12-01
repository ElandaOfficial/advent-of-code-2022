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

#include <fstream>
#include <iostream>
#include <queue>
#include <string>



//**********************************************************************************************************************
// region main
//======================================================================================================================
int main()
{
    //==================================================================================================================
    struct ElfScore
    {
        //==============================================================================================================
        int score;
        int number;
        
        //==============================================================================================================
        ElfScore(int parScore, int parNumber)
            : score(parScore),
              number(parNumber)
        {}
    };
    
    //==================================================================================================================
    std::ifstream file(INPUT_FILE); // File url defined in CMake script
    
    if (!file.is_open())
    {
        std::cout << "Couldn't open input";
        return 1;
    }
    
    static constexpr auto comparator = [](auto &&left, auto &&right) { return (left.score < right.score); };
    std::priority_queue<ElfScore, std::vector<ElfScore>, decltype(comparator)> p_queue(comparator);
    
    std::string line;
    int         score = 0;
    
    for (int i = 1; std::getline(file, line);)
    {
        if (!line.empty())
        {
            score += std::stoi(line);
        }
        else if (score > 0)
        {
            p_queue.emplace(std::exchange(score, 0), i++);
        }
    }
    
    const ElfScore top1 = p_queue.top();
    std::cout << "Elf numero " << top1.number << " is the big boss with " << top1.score << " calories!\n";
    p_queue.pop();
    
    const ElfScore top2 = p_queue.top();
    p_queue.pop();
    const ElfScore top3 = p_queue.top();
    std::cout << "Top three elves carry in sum: " << (top1.score + top2.score + top3.score);
    
    return 0;
}
//======================================================================================================================
// endregion main
//**********************************************************************************************************************
