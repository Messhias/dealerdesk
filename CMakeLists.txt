cmake_minimum_required(VERSION 3.16)
project(dealerdesk C)

# Equivalent to the challenge one-liner: gcc -o dealerdesk *.c -lm
add_executable(dealerdesk
    main.c
    calc.c
    dbfile.c
    screen.c
    sale.c
)
target_link_libraries(dealerdesk PRIVATE m)

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
