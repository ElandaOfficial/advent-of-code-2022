########################################################################################################################
cmake_minimum_required(VERSION 3.24)

########################################################################################################################
project(advent_of_code_2022)

set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(AOC_2022_SESSION_KEY "" CACHE STRING "The session key for the automatic input download")

########################################################################################################################
function(create_day n preprocessor_func)
    set(DAY_TARGET day_${n})
    set(DAY_MAIN   "${CMAKE_CURRENT_LIST_DIR}/src/day${n}/main.cpp")
    set(DAY_INPUT  "${CMAKE_CURRENT_LIST_DIR}/src/day${n}/input.txt")
    
    if (NOT EXISTS "${DAY_MAIN}")
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/src/day${n}")
        
        #if ("${AOC_2022_SESSION_KEY}" STREQUAL "")
            file(TOUCH "${DAY_INPUT}")
        #else()
        #    file(DOWNLOAD "https://adventofcode.com/2022/day/${n}/input" "${DAY_INPUT}"
        #        HTTPHEADER "Cookie: session=${AOC_2022_SESSION_KEY}")
        #endif()
        
        if (NOT "${preprocessor_func}" STREQUAL "")
            cmake_language(CALL "${preprocessor_func}" ${n} "${DAY_INPUT}")
        endif()
        
        string(TIMESTAMP DAY_MAIN_DAYNUM
            "%d")
        
        file(READ "${CMAKE_CURRENT_LIST_DIR}/main.in.cpp" DAY_MAIN_TEMPLATE)
        file(CONFIGURE
            OUTPUT  "${DAY_MAIN}"
            CONTENT "${DAY_MAIN_TEMPLATE}"
            @ONLY)
    endif()
    
    add_executable(${DAY_TARGET}
        ${DAY_MAIN})
    
    target_compile_definitions(${DAY_TARGET}
        PRIVATE
            INPUT_FILE="${DAY_INPUT}")
endfunction()

function(preprocess_input n input_file)
    file(READ "${input_file}" file_data)
    
    if (${n} EQUAL 3)
        string(REGEX REPLACE [[(.+)]] [[{ "\0", },]] file_data "${file_data}")
    elseif (${n} EQUAL 4)
        string(REGEX REPLACE [[([0-9]+)-([0-9]+),([0-9]+)-([0-9]+)]] [[{ \1_r-\2,\3_r-\4 },]] file_data "${file_data}")
    endif()
    
    file(WRITE "${input_file}" "${file_data}")
endfunction()

########################################################################################################################
if ("${AOC_2022_SESSION_KEY}" STREQUAL "")
    #message(WARNING "Session key is empty, inputs won't be downloaded automatically")
endif()

########################################################################################################################
create_day(1 preprocess_input)
create_day(2 preprocess_input)
create_day(3 preprocess_input)
create_day(4 preprocess_input)
