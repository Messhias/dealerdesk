cmake_minimum_required(VERSION 3.16)
project(dealerdesk C)

# Equivalent to the challenge one-liner: gcc -o dealerdesk *.c -lm
add_executable(dealerdesk
    main.c
    calc.c
    dbfile.c
    screen.c
    sale.c
    stock.c
    customer.c
)
# libm is required on Unix; MSVC provides math in the CRT (needed for Win CD builds).
if(UNIX)
    target_link_libraries(dealerdesk PRIVATE m)
endif()

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

# BDD terminal runner needs PTY + zlib (Unix host or Docker). Skip on Windows CD.
if(UNIX)
    add_subdirectory(BDD)
endif()
