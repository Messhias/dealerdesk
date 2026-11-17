# Allow spaces and punctuation in add_test(NAME ...) so titles match Gherkin 1:1.
if(POLICY CMP0110)
    cmake_policy(SET CMP0110 NEW)
endif()

set(TEST_COMMON test_common.c)

add_executable(test_pricing
    test_pricing.c
    ${TEST_COMMON}
    ${CMAKE_SOURCE_DIR}/calc.c
)
target_include_directories(test_pricing PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
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

add_executable(test_persistence
    test_persistence.c
    ${TEST_COMMON}
    ${CMAKE_SOURCE_DIR}/dbfile.c
)
target_include_directories(test_persistence PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(test_persistence PRIVATE m)

# Scenario titles map 1:1 to features/persistence.feature
# Sample-data scenario needs repo-root CWD so data/ resolves.
add_test(
    NAME "Load skips comment and empty lines in stock file"
    COMMAND test_persistence skip_stock_comments
)
add_test(
    NAME "Load skips comment and empty lines in customers file"
    COMMAND test_persistence skip_customer_comments
)
add_test(
    NAME "Stock round-trip save then reload preserves the record"
    COMMAND test_persistence stock_round_trip
)
add_test(
    NAME "Customers and next ids round-trip save then reload"
    COMMAND test_persistence customers_ids_round_trip
)
add_test(
    NAME "Missing data files leave empty tables without crashing"
    COMMAND test_persistence missing_files
)
add_test(
    NAME "Sample repo data loads expected counts and next ids"
    COMMAND test_persistence sample_repo_data
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_executable(test_quote_sell
    test_quote_sell.c
    ${TEST_COMMON}
    ${CMAKE_SOURCE_DIR}/sale.c
    ${CMAKE_SOURCE_DIR}/calc.c
    ${CMAKE_SOURCE_DIR}/dbfile.c
)
target_include_directories(test_quote_sell PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(test_quote_sell PRIVATE m)

# Scenario titles map 1:1 to features/quote_sell.feature
add_test(
    NAME "Sold vehicle cannot be quoted"
    COMMAND test_quote_sell sold_cannot_quote
)
add_test(
    NAME "Available vehicle can be quoted"
    COMMAND test_quote_sell available_can_quote
)
add_test(
    NAME "Invoice packs trade and salesman discounts into disc"
    COMMAND test_quote_sell packs_disc
)
add_test(
    NAME "Invoice packs delivery and registration into fees 205.00"
    COMMAND test_quote_sell packs_fees
)
add_test(
    NAME "Confirming a sale marks vehicle sold and appends invoice"
    COMMAND test_quote_sell confirm_sale
)
add_test(
    NAME "Cancelling a sale leaves stock and invoices unchanged"
    COMMAND test_quote_sell cancel_sale
)

add_executable(test_stock
    test_stock.c
    ${TEST_COMMON}
    ${CMAKE_SOURCE_DIR}/stock.c
    ${CMAKE_SOURCE_DIR}/dbfile.c
)
target_include_directories(test_stock PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(test_stock PRIVATE m)

# Scenario titles map 1:1 to features/stock.feature
add_test(
    NAME "Adding a vehicle requires plate and positive list price"
    COMMAND test_stock add_requires_plate_price
)
add_test(
    NAME "Valid add assigns next id and in-stock status"
    COMMAND test_stock valid_add
)
add_test(
    NAME "Find by plate matches exactly including case"
    COMMAND test_stock find_exact_case
)
add_test(
    NAME "Change price updates when new price is positive"
    COMMAND test_stock change_price_positive
)
add_test(
    NAME "Change price leaves price unchanged when new price is not positive"
    COMMAND test_stock change_price_rejected
)
add_test(
    NAME "Reserve toggles between in stock and reserved"
    COMMAND test_stock reserve_toggle
)
add_test(
    NAME "Sold vehicle cannot be reserved"
    COMMAND test_stock sold_cannot_reserve
)
add_test(
    NAME "List stock omits sold vehicles"
    COMMAND test_stock list_omits_sold
)
