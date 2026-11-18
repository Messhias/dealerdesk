# Terminal UI BDD runner (drives dealerdesk via PTY). Not part of the CTest gate.
# Every .c under BDD/runner and BDD/step_definitions must be listed here.

find_package(ZLIB REQUIRED)

add_executable(bdd_runner
    runner/main.c
    runner/console.c
    runner/screenshot.c
    runner/console.h
    runner/screenshot.h
    step_definitions/steps_registry.c
    step_definitions/steps_common.c
    step_definitions/stock_steps.c
    step_definitions/customers_steps.c
    step_definitions/pricing_steps.c
    step_definitions/quote_sell_steps.c
    step_definitions/fleet_steps.c
    step_definitions/persistence_steps.c
    step_definitions/steps.h
    step_definitions/steps_common.h
)

target_include_directories(bdd_runner PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/runner
    ${CMAKE_CURRENT_SOURCE_DIR}/step_definitions
)

target_compile_options(bdd_runner PRIVATE -Wall -Wextra)

# Match historical link line: -lutil -lz (PTY on Linux/macOS; zlib for PNG).
target_link_libraries(bdd_runner PRIVATE ZLIB::ZLIB util)

# Sit next to dealerdesk in the build dir (build/bdd_runner, build-docker/bdd_runner).
set_target_properties(bdd_runner PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)
