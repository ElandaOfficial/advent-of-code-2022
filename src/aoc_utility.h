/**
    ====================================================================================================================
    
    This work is free. You can redistribute it and/or modify it under the
    terms of the Do What The Fuck You Want To Public License, Version 2,
    as published by Sam Hocevar. See the COPYING file or http://www.wtfpl.net/ 
    for more details.
    
    ====================================================================================================================
    
    @author Elanda
    @file   aoc_utility.h
    @date   05, December 2022
    
    ====================================================================================================================
 */

#pragma once

#include <string_view>
#include <type_traits>



namespace aoc
{
    template<class T, std::enable_if_t<std::is_integral_v<T>>* = nullptr>
    [[nodiscard]]
    constexpr bool isLowerCase(T character) noexcept
    {
        const auto c = static_cast<unsigned char>(character);
        return (c >= 'a' && c <= 'z');
    }
    
    template<class T, std::enable_if_t<std::is_integral_v<T>>* = nullptr>
    [[nodiscard]]
    constexpr bool isUpperCase(T character) noexcept
    {
        const auto c = static_cast<unsigned char>(character);
        return (c >= 'A' && c <= 'Z');
    }
    
    template<class T, std::enable_if_t<std::is_integral_v<T>>* = nullptr>
    [[nodiscard]]
    constexpr bool isLetter(T character) noexcept
    {
        return (isLowerCase(character) || isUpperCase(character));
    }
    
    template<class T, std::enable_if_t<std::is_integral_v<T>>* = nullptr>
    [[nodiscard]]
    constexpr bool isDigit(T character) noexcept
    {
        const auto c = static_cast<unsigned char>(character);
        return (c >= '0' && c <= '9');
    }
    
    template<class T, std::enable_if_t<std::is_integral_v<T>>* = nullptr>
    [[nodiscard]]
    constexpr bool isAlphanumeric(T character) noexcept
    {
        const auto c = static_cast<unsigned char>(character);
        return (isDigit(c) || isLetter(c));
    }
    
    template<class T, std::enable_if_t<std::is_integral_v<T>>* = nullptr>
    [[nodiscard]]
    constexpr bool isIdentifier(T character) noexcept
    {
        const auto c = static_cast<unsigned char>(character);
        return (isDigit(c) || isLetter(c) || c == '_' || c == '-');
    }
    
    template<class T, std::enable_if_t<std::is_integral_v<T>>* = nullptr>
    [[nodiscard]]
    constexpr bool isWhitespace(T character) noexcept
    {
        const auto c = static_cast<unsigned char>(character);
        return (c == ' ' || c == '\t' || c == '\r' || c == '\n' || c == '\v' || c == '\f');
    }
    
    template<class T, std::enable_if_t<std::is_convertible_v<T, std::string_view>>* = nullptr>
    [[nodiscard]]
    constexpr bool isEmptyLine(T &&input) noexcept
    {
        const std::string_view data(std::forward<T>(input));
        
        for (const char c : data) //NOLINT
        {
            if (!isWhitespace(c))
            {
                return false;
            }
        }
        
        return true;
    }
    
    template<class It>
    [[nodiscard]]
    constexpr char peekNext(It pos, It end) noexcept
    {
        if (pos != end)
        {
            return *(pos + 1);
        }
        
        return 0;
    }
}
