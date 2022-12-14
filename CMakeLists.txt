########################################################################################################################
cmake_minimum_required(VERSION 3.24)

########################################################################################################################
project(advent_of_code_2022)

set(CMAKE_CXX_STANDARD          17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

########################################################################################################################
function(create_day n)
    set(DAY_TARGET day_${n})
    set(DAY_MAIN   "${CMAKE_CURRENT_LIST_DIR}/src/day${n}/main.cpp")
    set(DAY_INPUT  "${CMAKE_CURRENT_LIST_DIR}/src/day${n}/input.txt")
    
    if (NOT EXISTS "${DAY_MAIN}")
        file(MAKE_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/src/day${n}")
        
        file(TOUCH "${DAY_INPUT}")
        
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

########################################################################################################################
if ("${AOC_2022_SESSION_KEY}" STREQUAL "")
    #message(WARNING "Session key is empty, inputs won't be downloaded automatically")
endif()

########################################################################################################################
set(DAY_CURRENT_DAY 5)

foreach(i RANGE 1 ${DAY_CURRENT_DAY})
    create_day(${i} preprocess_input)
endforeach()
