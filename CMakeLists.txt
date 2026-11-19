cmake_minimum_required(VERSION 3.16)
project(dealerdesk C)

# Equivalent to the challenge one-liner: gcc -o dealerdesk *.c -lm
# List headers on the target so IDEs (Rider/CLion) do not mark them as orphans.
add_executable(dealerdesk
    main.c
    calc.c
    calc.h
    dbfile.c
    dbfile.h
    screen.c
    screen.h
    sale.c
    sale.h
    stock.c
    stock.h
    customer.c
    customer.h
    globals.h
    version.h
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
