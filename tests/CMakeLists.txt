# Allow spaces and punctuation in add_test(NAME ...) so titles match Gherkin 1:1.
if(POLICY CMP0110)
    cmake_policy(SET CMP0110 NEW)
endif()

add_executable(test_smoke test_smoke.c)
add_test(NAME test_smoke COMMAND test_smoke)

add_executable(test_pricing
    test_pricing.c
    ${CMAKE_SOURCE_DIR}/calc.c
)
target_include_directories(test_pricing PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(test_pricing PRIVATE m)

# Scenario titles map 1:1 to features/pricing.feature
add_test(
    NAME "Retail quote with salesman 5% and trade-in matches invoice 50230"
    COMMAND test_pricing retail_50230
)
add_test(
    NAME "Trade quote with 10% off list and no salesman discount matches invoice 50231"
    COMMAND test_pricing trade_50231
)
add_test(
    NAME "Retail quote with salesman 8% matches invoice 50232"
    COMMAND test_pricing retail_50232
)
add_test(
    NAME "Salesman discount below 0 is treated as 0"
    COMMAND test_pricing salesman_below_0
)
add_test(
    NAME "Salesman discount above 15 is treated as 15"
    COMMAND test_pricing salesman_above_15
)
add_test(
    NAME "Trade discount then salesman discount on what is left"
    COMMAND test_pricing trade_then_salesman
)
add_test(
    NAME "Money amounts round to two decimal places"
    COMMAND test_pricing money_round
)
add_test(
    NAME "Delivery and registration fees are fixed"
    COMMAND test_pricing fixed_fees
)
add_test(
    NAME "rnd2 rounds half-up to two decimal places"
    COMMAND test_pricing rnd2
)
