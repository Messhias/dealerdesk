Feature: Customer add find and list
  As a dealership user
  I need customer add find and list rules to stay stable
  So that retail and trade customer ops do not drift from current behaviour

  # Behaviour contract only. Verified later by CTest (no Gherkin runner yet).
  # Truth: docs/SYSTEM.md Customers, and main.c customer path.
  # Truth exercised via customer.c seams used by main.c (no stdin).
  # Baseline R and T add/find/list. Fleet add/mark lives in features/fleet.feature.

  Scenario: Adding a customer requires a non-empty name
    Given an empty customer table
    When add is attempted with an empty name
    Then the add fails
    And the customer table remains empty

  Scenario: Valid add assigns next customer id and default retail type
    Given an empty customer table
    And next customer id 2101
    When a retail customer is added with name Sarah Whitfield phone 07700 900123 email sarah@example.com
    Then the add succeeds
    And exactly 1 customer is in memory
    And that customer id is 2101
    And that customer type is retail
    And next customer id is 2102

  Scenario: Valid add with trade confirmed sets type T
    Given an empty customer table
    And next customer id 2105
    When a trade customer is added with name Fleet Motors Ltd phone 01234 567890 email trade@example.com
    Then the add succeeds
    And that customer id is 2105
    And that customer type is trade
    And next customer id is 2106

  Scenario: Add is rejected when customer table is full
    Given a full customer table
    And next customer id 2999
    When add is attempted with name Overflow Test
    Then the add fails
    And next customer id is still 2999

  Scenario: Find by name uses case-sensitive substring match
    Given one customer named Sarah Whitfield
    When find by name uses Whit
    Then the match count is 1
    When find by name uses whit
    Then the match count is 0

  Scenario: Find with empty substring matches all customers
    Given two customers named Sarah Whitfield and Tom Bradley
    When find by name uses an empty substring
    Then the match count is 2

  Scenario: Find with no matching substring returns zero matches
    Given one customer named Sarah Whitfield
    When find by name uses ZZZZ
    Then the match count is 0

  Scenario: List customers includes every stored customer
    Given one retail customer
    And one trade customer
    When the customer list is shown
    Then both customers are visible
