/**
    ====================================================================================================================
    
    This work is free. You can redistribute it and/or modify it under the
    terms of the Do What The Fuck You Want To Public License, Version 2,
    as published by Sam Hocevar. See the COPYING file or http://www.wtfpl.net/ 
    for more details.
    
    ====================================================================================================================
    
    @author Elanda
    @file   main.cpp
    @date   05, December 2022
    
    ====================================================================================================================
 */

#include "../aoc_utility.h"

#include <array>
#include <fstream>
#include <iostream>
#include <queue>
#include <list>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>



//**********************************************************************************************************************
// region Namespace
//======================================================================================================================
namespace
{
    // We pretend the input is some sort of actual type of document that follows a certain standard (like json, xml ect.)
    // The name for this imaginary document standard shall be: ICMS (International Crate Diagram Schematic)
    //
    // The rules for the diagram are:
    // - Each row represents a line of crates in each stack
    // - Since these are stacks, crates can only be taken from the top; the same is true for putting crates on
    // - Any additional crates is an error
    // - A crate is in the format "[ID]", depending on their significance in the line, they belong to the stack 
    //   previously parsed from the line numbers; IDs are one character
    // - Any crate has its own ID but there can be two or more with the same ID
    // - To make sure a crate belongs to a certain stack, the crate will determine,
    //   based on the column of the id of the crate, if it is within the start and end column of a stack id
    // - If a crate is out of a stack, this is an error
    // - If, from bottom to top, at least one crate is out of id bounds,
    //   all crates above that one are considered "floating", and be it just this one crate, this is an error
    class IcmsDocument
    {
    public:
        class Stack;
        
        /** Contains the information about our crates. */
        struct Crate
        {
            Stack *owner { nullptr };
            char  id     {       0 };
        };
        
        /** A pile of containers, nothing more... nothing less. */
        class Stack : private std::stack<Crate, std::list<Crate>>
        {
        public:
            explicit Stack(std::string parId) noexcept
                : id(std::move(parId))
            {}
            
            Stack(std::string parId, std::initializer_list<Crate> parCrates)
                noexcept(std::is_nothrow_move_constructible_v<std::list<Crate>>)
                : std::stack<Crate, std::list<Crate>>(parCrates),
                  id(std::move(parId))
            {}
            
            //==========================================================================================================
            /** Gets the id of the stack. */
            [[nodiscard]]
            const std::string &getId() const noexcept
            {
                return id;
            }
            
            //==========================================================================================================
            [[nodiscard]]
            const Crate& topCrate() const noexcept
            {
                return top();
            }
            
            /** Place a crate on the top. */
            void putOn(Crate crate)
            {
                crate.owner = this;
                push(crate);
            }
            
            /** Put on a list of crates. */
            void putOn(const std::vector<Crate> &crates)
            {
                for (const Crate &crate : crates)
                {
                    putOn(crate);
                }
            }
            
            /** Take a crate from the top. */
            Crate takeOff()
            {
                Crate crate = top();
                crate.owner = nullptr;
                pop();
                
                return crate;
            }
            
            /** Take a number of crates from the top. */
            std::vector<Crate> takeOff(int amount)
            {
                std::vector<Crate> crates;
                crates.reserve(amount);
                
                for (amount -= 1; amount >= 0; --amount)
                {
                    crates.emplace_back(takeOff());
                }
                
                return crates;
            }
            
            /** Take a number of crates from the top. */
            std::vector<Crate> liftOff(int amount)
            {
                std::vector<Crate> crates;
                crates.resize(amount);
                
                for (amount -= 1; amount >= 0; --amount)
                {
                    crates[amount] = takeOff();
                }
                
                return crates;
            }
            
            //==========================================================================================================
            /** Reads the sticker on the crate. UB if index is out of bounds. */
            [[nodiscard]]
            const Crate &readCrate(int index) const noexcept
            {
                auto it = c.begin();
                std::advance(it, index);
                return *it;
            }
            
        private:
            std::string id;
        };
        
        struct Instruction
        {
            int amount;
            int from;
            int to;
        };
        
        //==============================================================================================================
        /** Constructs an invalid document. */
        IcmsDocument() noexcept = default;
        
        //==============================================================================================================
        /** Have we parsed our created our document? */
        [[nodiscard]]
        bool isValidDocument() const noexcept
        {
            return parsed;
        }
        
        [[nodiscard]]
        operator bool() const noexcept // NOLINT
        {
            return isValidDocument();
        }
        
        //==============================================================================================================
        std::vector<Stack>& getStacks() noexcept
        {
            return stacks;
        }
        
        const std::vector<Instruction>& getInstructions() const noexcept
        {
            return instructions;
        }
        
    private:
        // It got late, I don't care
        friend class IcmsParser;
        
        //==============================================================================================================
        std::vector<Stack>       stacks;
        std::vector<Instruction> instructions;
        bool                     parsed { false };
        
        //==============================================================================================================
        IcmsDocument(std::vector<Stack> parStacks, std::vector<Instruction> parInstructions) noexcept
            : stacks      (std::move(parStacks)),
              instructions(std::move(parInstructions)),
              parsed      (true)
        {}
    };
    
    class IcmsParser
    {
    public:
        [[nodiscard]]
        static IcmsDocument parseDocument(const char *file)
        {
            IcmsParser parser(file);
            auto it = parser.lines.begin();
            std::vector<IcmsDocument::Stack>       stacks       = parser.parseSchematic   (it);
            std::vector<IcmsDocument::Instruction> instructions = parser.parseInstructions(it);
            
            return { std::move(stacks), std::move(instructions) };
        }
        
    private:
        struct Token
        {
            std::string content;
            int         line;
            int         column;
            int         length;
        };
        
        struct InstructionToken
        {
            std::string id;
            std::string value;
        };
        
        //==============================================================================================================
        std::vector<std::string> lines;
        
        //==============================================================================================================
        [[nodiscard]]
        static std::vector<std::string> readLines(const std::string &file)
        {
            std::fstream input(file);
            
            if (!input.is_open())
            {
                throw std::runtime_error("File '" + file + "' not found");
            }
            
            std::string              line;
            std::vector<std::string> lines;
            lines.reserve(1024);
            
            while (std::getline(input, line))
            {
                lines.emplace_back(std::move(line));
            }
            
            lines.shrink_to_fit();
            return lines;
        }
        
        //==============================================================================================================
        static void parseError(const std::string &message, int lineNumber)
        {
            std::stringstream ss;
            ss << "Invalid crate-schematic on line '" << lineNumber << "': " << message;
            throw std::runtime_error(ss.str());
        }
        
        //==============================================================================================================
        static void readIdTokens(std::vector<Token> &tokens,
                                 const std::string  &line,
                                 std::size_t        startIndex,
                                 int                lineNumber)
        {
            for (auto it = (line.begin() + static_cast<int>(startIndex)); it != line.end(); ++it)
            {
                const char c = *it;
                
                if (aoc::isWhitespace(c))
                {
                    continue;
                }
                
                if (aoc::isIdentifier(c))
                {
                    const int column = static_cast<int>(std::distance(line.begin(), it));
                    
                    std::array<char, 512> id_buffer { c };
                    auto id_it = id_buffer.begin();
    
                    (void) ++it;
                    
                    for (; it != line.end(); ++it)
                    {
                        const char n = *it;
                        
                        if (aoc::isWhitespace(n))
                        {
                            break;
                        }
                        
                        if (aoc::isIdentifier(n))
                        {
                            *(++id_it) = n;
                            continue;
                        }
                    }
                    
                    const int id_len = static_cast<int>(std::strlen(id_buffer.data()));
                    (void) tokens.emplace_back(Token{ id_buffer.data(), lineNumber, column, id_len });
                    
                    if (it == line.end())
                    {
                        break;
                    }
                    
                    continue;
                }
                
                parseError(std::string("'") + c + "' is not a valid starting token", lineNumber);
            }
        }
        
        static void readCrateTokens(std::queue<Token> &tokens,
                                    const std::string &line,
                                    std::size_t       startIndex,
                                    int               lineNumber)
        {
            for (auto it = (line.begin() + static_cast<int>(startIndex)); it != line.end(); ++it)
            {
                const char c = *it;
                
                if (aoc::isWhitespace(c))
                {
                    continue;
                }
                
                if (c == '[')
                {
                    if (aoc::peekNext(it, line.end()) == 0)
                    {
                        parseError("unterminated crate definition", lineNumber);
                    }
                    
                    (void) ++it;
                    
                    if (aoc::peekNext(it, line.end()) != ']')
                    {
                        parseError("unterminated crate definition", lineNumber);
                    }
                    
                    (void) tokens.emplace(Token {
                        std::string{ *it },
                        lineNumber,
                        static_cast<int>(std::distance(line.begin(), it)),
                        1
                    });
                    
                    (void) ++it;
                    
                    continue;
                }
                
                parseError(std::string("'") + c + "' is not a valid starting token", lineNumber);
            }
        }
        
        //==============================================================================================================
        explicit IcmsParser(const std::string &documentFile)
            : lines(readLines(documentFile))
        {}
        
        //==============================================================================================================
        [[nodiscard]]
        std::vector<IcmsDocument::Stack> parseSchematic(decltype(lines.begin()) &it)
        {
            std::vector<std::queue<Token>> tokens_crate;
            std::vector<Token>             tokens_id;
            
            for (; it != lines.end(); ++it)
            {
                const std::string &line    = *it;
                const int         line_num = static_cast<int>(std::distance(lines.begin(), it)) + 1;
                
                if (aoc::isEmptyLine(line))
                {
                    continue;
                }
                
                for (auto c_it = line.begin(); c_it != line.end(); ++c_it)
                {
                    const char c = *c_it;
                    
                    if (aoc::isWhitespace(c))
                    {
                        continue;
                    }
                    
                    if (c == '[')
                    {
                        readCrateTokens(tokens_crate.emplace_back(), line, std::distance(line.begin(), c_it), line_num);
                        break;
                    }
                    
                    if (aoc::isDigit(c))
                    {
                        readIdTokens(tokens_id, line, std::distance(line.begin(), c_it), line_num);
                        break;
                    }
                    
                    parseError(std::string("'") + c + "' is not a valid starting token", line_num);
                }
                
                if (!tokens_id.empty())
                {
                    (void) ++it;
                    break;
                }
            }
            
            std::vector<IcmsDocument::Stack> stacks;
            
            for (const auto &id : tokens_id)
            {
                IcmsDocument::Stack stack(id.content);
                
                for (auto list_it = tokens_crate.rbegin(); list_it != tokens_crate.rend(); ++list_it)
                {
                    std::queue<Token> &tokens = *list_it;
                    
                    if (tokens.empty())
                    {
                        break;
                    }
                    
                    const Token crate = tokens.front();
                    
                    if (crate.column < id.column)
                    {
                        parseError("encountered floating or excess crate '[" + crate.content
                                       + "]' at column " + std::to_string(crate.column + 1),
                                   crate.line);
                    }
                    
                    if (crate.column >= id.column && crate.column < (id.column + id.length))
                    {
                        stack.putOn(IcmsDocument::Crate{ &stack, crate.content[0] });
                        tokens.pop();

                        continue;
                    }
                    
                    break;
                }
                
                (void) stacks.emplace_back(std::move(stack));
            }
            
            for (auto list_it = tokens_crate.rbegin(); list_it != tokens_crate.rend(); ++list_it)
            {
                const std::queue<Token> &tokens = *list_it;
                
                if (!tokens.empty())
                {
                    const Token crate = tokens.front();
                    parseError("encountered floating or excess crate '[" + crate.content
                                   + "]' at column " + std::to_string(crate.column + 1),
                               crate.line);
                }
            }
            
            return stacks;
        }
        
        [[nodiscard]]
        std::vector<IcmsDocument::Instruction> parseInstructions(decltype(lines.begin()) &it)
        {
            std::vector<IcmsDocument::Instruction> instructions;
            
            for (; it != lines.end(); ++it)
            {
                std::string line     = *it;
                const int   line_num = static_cast<int>(std::distance(lines.begin(), it) + 1);
                
                if (aoc::isEmptyLine(line))
                {
                    continue;
                }
                
                std::vector<std::string> arguments;
                std::size_t              pos;
                
                while ((pos = line.find(' ')) != std::string::npos)
                {
                    (void) arguments.emplace_back(line.substr(0, pos));
                    (void) line.erase(0, pos + 1);
                }
                arguments.emplace_back(line);
                
                instructions.emplace_back(IcmsDocument::Instruction{
                    std::stoi(arguments[1]),
                    std::stoi(arguments[3]),
                    std::stoi(arguments[5])
                });
            }
            
            return instructions;
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
    ::IcmsDocument document;
            
    try
    {
        document = ::IcmsParser::parseDocument(INPUT_FILE);
    }
    catch (const std::exception &ex)
    {
        std::cout << "Exception caught: " << ex.what();
        return 1;
    }
    
    for (const auto &[amount, from, to] : document.getInstructions())
    {
        ::IcmsDocument::Stack &from_stack = document.getStacks()[from - 1];
        ::IcmsDocument::Stack &to_stack   = document.getStacks()[to - 1];
        to_stack.putOn(from_stack.takeOff(amount));
    }
    
    std::cout << "The crate that ends up on each stack is: ";
    
    for (const ::IcmsDocument::Stack &stack : document.getStacks())
    {
        std::cout << stack.topCrate().id;
    }
    
    std::cout << '\n';
    
    try
    {
        document = ::IcmsParser::parseDocument(INPUT_FILE);
    }
    catch (const std::exception &ex)
    {
        std::cout << "Exception caught: " << ex.what();
        return 1;
    }
    
    for (const auto &[amount, from, to] : document.getInstructions())
    {
        ::IcmsDocument::Stack &from_stack = document.getStacks()[from - 1];
        ::IcmsDocument::Stack &to_stack   = document.getStacks()[to - 1];
        to_stack.putOn(from_stack.liftOff(amount));
    }
    
    std::cout << "The crate that ends up on each stack after a complete lift off is: ";
    
    for (const ::IcmsDocument::Stack &stack : document.getStacks())
    {
        std::cout << stack.topCrate().id;
    }
    
    std::cout << '\n';
    
    return 0;
}
//======================================================================================================================
// endregion Main
//**********************************************************************************************************************
