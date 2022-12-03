/**
    ====================================================================================================================
    
    This work is free. You can redistribute it and/or modify it under the
    terms of the Do What The Fuck You Want To Public License, Version 2,
    as published by Sam Hocevar. See the COPYING file or http://www.wtfpl.net/ 
    for more details.
    
    ====================================================================================================================
    
    @author Elanda
    @file   main.cpp
    @date   03, December 2022
    
    ====================================================================================================================
 */

#include <algorithm>
#include <array>
#include <iostream>
#include <string_view>



//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
namespace
{
    //==================================================================================================================
    // We include the input as an array
    constexpr std::array input {
        #include "input.txt"
    };
    
    constexpr std::size_t maxCompartmentSize = []()
    {
        std::size_t max = 0;
        
        for (const char *rucksack : input)
        {
            const std::string_view entry { rucksack };
            const std::size_t      csize = (entry.length() / 2);
            
            if (csize > max)
            {
                max = csize;
            }
        }
        
        return max;
    }();
    
    constexpr std::size_t priorityOffsetLower = ('a' -  1);
    constexpr std::size_t priorityOffsetUpper = ('A' - 27);
    
    //==================================================================================================================
    using UniqueArray = std::array<char, maxCompartmentSize>;
    
    //==================================================================================================================
    [[nodiscard]]
    constexpr auto filterCompartmentDuplicates(std::string_view data)
    {
        UniqueArray buffer {};
        int         index = 0;
        
        for (const char c : data)
        {
            auto it = buffer.begin();
            
            for (; *it != 0; ++it)
            {
                if (*it == c)
                {
                    break;
                }
            }
            
            if (*it == 0)
            {
                buffer[index++] = c;
            }
        }
        
        return buffer;
    }
    
    //==================================================================================================================
    template<const auto &AllInputs, std::size_t Index>
    struct Group
    {
        //==============================================================================================================
        using NextGroup = std::conditional_t<((Index + 3) == AllInputs.size()), void, Group<AllInputs, Index + 3>>;
        
        //==============================================================================================================
        static constexpr std::array<std::string_view, 3> group {
            AllInputs[Index],
            AllInputs[Index + 1],
            AllInputs[Index + 2]
        };
    };
    
    template<const auto &AllInputs, std::size_t Index>
    struct Entry
    {
        static constexpr std::string_view value = AllInputs[Index];
    };
    
    template<class Input, std::size_t Offset, std::size_t Length>
    struct Compartment
    {
        static constexpr std::string_view inventory = std::string_view(Input::value).substr(Offset, Length);
    };
    
    template<class Input>
    struct Rucksack
    {
        //==============================================================================================================
        static constexpr std::string_view input { Input::value };
        static constexpr std::size_t      inventoryOffset = (input.size() / 2);
        
        //==============================================================================================================
        using Compartment1 = Compartment<Input, 0,               inventoryOffset>;
        using Compartment2 = Compartment<Input, inventoryOffset, (input.length() - inventoryOffset)>;
        
        //==============================================================================================================
        [[nodiscard]]
        static constexpr std::size_t calculatePriorities() noexcept
        {
            std::size_t       priorities = 0;
            const UniqueArray filtered   = filterCompartmentDuplicates(Compartment1::inventory);
            
            for (auto it = filtered.begin(); *it != 0; ++it)
            {
                const char c = *it;
                
                for (const char c2 : Compartment2::inventory)
                {
                    if (c == c2)
                    {
                        // Upper
                        if (c <= 'Z')
                        {
                            priorities += (c - priorityOffsetUpper);
                        }
                        
                        // Lower
                        else
                        {
                            priorities += (c - priorityOffsetLower);
                        }
                        
                        break;
                    }
                }
            }
            
            return priorities;
        }
    };
    
    template<std::size_t Size, std::size_t Offset, class Sequence = std::make_index_sequence<Size>>
    struct make_index_sequence_off;
    
    template<std::size_t Size, std::size_t ...Indices, std::size_t Offset>
    struct make_index_sequence_off<Size, Offset, std::index_sequence<Indices...>>
    {
        using type = std::index_sequence<Indices + Offset...>;
    };
    
    template<std::size_t Size, std::size_t Offset>
    using make_index_sequence_off_t = typename make_index_sequence_off<Size, Offset>::type;
    
    // A simple placeholder with a list of index sequences, we can unpack this one to make use of them individually
    template<class ...IndexSequences>
    struct ISeqArray {};
    
    // We want to split up our indices into several std::index_sequence types
    template<std::size_t Size, std::size_t LastSize, class ...Sequences>
    struct IndexSequenceSplitter
        : IndexSequenceSplitter<Size,
                                LastSize + std::min<std::size_t>(Size - LastSize, 50),
                                Sequences...,
                                make_index_sequence_off_t<std::min<std::size_t>(Size - LastSize, 50), LastSize>>
    {};
    
    template<std::size_t Size, class ...Sequences>
    struct IndexSequenceSplitter<Size, Size, Sequences...>
    {
        using type = ISeqArray<Sequences...>;
    };
    
    // We want to split up our fold expressions to max 50 calculations, because there is a limit to them (which we are exceeding)
    template<const auto &AllInputs, std::size_t Result, class ...IndexSequences>
    struct FoldExpressionSplitter;
    
    template<const auto &AllInputs, std::size_t Result, class ...OtherSequences, std::size_t ...CurrentIndices>
    struct FoldExpressionSplitter<AllInputs, Result, std::index_sequence<CurrentIndices...>, OtherSequences...>
        : FoldExpressionSplitter<AllInputs,
                                 Result + (Rucksack<Entry<AllInputs, CurrentIndices>>::calculatePriorities() + ...),
                                 OtherSequences...>
    {};
    
    template<const auto &AllInputs, std::size_t Result, std::size_t ...CurrentIndices>
    struct FoldExpressionSplitter<AllInputs, Result, std::index_sequence<CurrentIndices...>>
    {
        static constexpr std::size_t value
            = (Result + (Rucksack<Entry<AllInputs, CurrentIndices>>::calculatePriorities() + ...));
    };
    
    // A tiny wrapper
    template<const auto &AllInputs, class IndexSequenceArray>
    struct PriorityEvaluator;
    
    template<const auto &AllInputs, class ...IndexSequences>
    struct PriorityEvaluator<AllInputs, ISeqArray<IndexSequences...>>
    {
        static constexpr int priorities = FoldExpressionSplitter<AllInputs, 0, IndexSequences...>::value;
    };
    
    // Evaluates all badge's priorities
    template<class Group>
    struct BadgeEvaluator
    {
    private:
        using Next = typename Group::NextGroup;
        
        //==============================================================================================================
        static constexpr std::size_t evaluateGroup()
        {
            for (const char c : Group::group[0])
            {
                for (const char c2 : Group::group[1])
                {
                    if (c == c2)
                    {
                        for (const char c3 : Group::group[2])
                        {
                            if (c2 == c3)
                            {
                                // Upper
                                if (c <= 'Z')
                                {
                                    return (c - priorityOffsetUpper);
                                }
                                
                                // Lower
                                else
                                {
                                    return (c - priorityOffsetLower);
                                }
                                
                                break;
                            }
                        }
                    }
                }
            }
            
            return 0;
        }
        
        static constexpr std::size_t getNext()
        {
            if constexpr (std::is_same_v<Next, void>)
            {
                return 0;
            }
            else
            {
                return BadgeEvaluator<Next>::priorities;
            }
        }
        
    public:
        static constexpr std::size_t priorities = evaluateGroup() + getNext();
    };
}
//======================================================================================================================
// endregion Namespace
//**********************************************************************************************************************
// region Main
//======================================================================================================================
int main()
{
    using IndexList = ::IndexSequenceSplitter<::input.size(), 0>::type;
    std::cout << "All duplicates priorities is: " << ::PriorityEvaluator<::input, IndexList>::priorities << '\n';
    
    std::cout << "All badge's priorities is: " << ::BadgeEvaluator<::Group<::input, 0>>::priorities << '\n';
    
    return 0;
}
//======================================================================================================================
// endregion Main
//**********************************************************************************************************************
