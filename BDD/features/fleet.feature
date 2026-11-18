Feature: Fleet customer discount on quotes
  As a dealership user
  I need fleet customers to get an automatic 4% vehicle discount
  So that fleet contracts stay consistent and salesman discount stays free to negotiate

  # Behaviour contract only. Verified by CTest (unit/seam). Terminal UI flows use @terminal + BDD/runner.
  # Truth: TICKET-142.md, docs/SYSTEM.md Pricing (after fleet), calc_quote in calc.c
  # Fleet type is F. Fees (delivery, registration) are not discounted.
  # Existing retail/trade totals must stay correct to the penny.

  @terminal
  Scenario: Fleet quote shows 4% off list with no salesman discount
    Given a list price of 10000.00
    And a fleet customer
    And a salesman discount of 0%
    And a trade-in of 0.00
    When a quote is calculated
    Then the trade discount is 0.00
    And the fleet discount is 400.00
    And the salesman discount amount is 0.00
    And the net price is 9600.00
    And the delivery fee is 150.00
    And the VAT is 1950.00
    And the registration fee is 55.00
    And the total is 11755.00

  @terminal
  Scenario: Fleet discount then salesman discount on what is left
    Given a list price of 10000.00
    And a fleet customer
    And a salesman discount of 5%
    And a trade-in of 0.00
    When a quote is calculated
    Then the fleet discount is 400.00
    And the salesman discount amount is 480.00
    And the net price is 9120.00
    And the VAT is 1854.00
    And the total is 11179.00

  @terminal
  Scenario: Fleet fees stay fixed delivery 150 and registration 55
    Given a list price of 10495.00
    And a fleet customer
    And a salesman discount of 0%
    And a trade-in of 0.00
    When a quote is calculated
    Then the fleet discount is 419.80
    And the delivery fee is 150.00
    And the registration fee is 55.00
    And the net price is 10075.20
    And the total is 12325.24

  @terminal
  Scenario: Retail quote totals stay unchanged to the penny after fleet
    Given a list price of 9295.00
    And a retail customer
    And a salesman discount of 5%
    And a trade-in of 1500.00
    When a quote is calculated
    Then the trade discount is 0.00
    And the fleet discount is 0.00
    And the salesman discount amount is 464.75
    And the total is 9331.30

  @terminal
  Scenario: Trade quote totals stay unchanged to the penny after fleet
    Given a list price of 13495.00
    And a trade customer
    And a salesman discount of 0%
    And a trade-in of 0.00
    When a quote is calculated
    Then the trade discount is 1349.50
    And the fleet discount is 0.00
    And the salesman discount amount is 0.00
    And the total is 14809.60

  @terminal
  Scenario: Adding a new customer as fleet sets type F
    Given an empty customer table
    And next customer id 2201
    When a fleet customer is added with name Greenway Fleet New phone 01632 960700 email newfleet@example.com
    Then the add succeeds
    And that customer id is 2201
    And that customer type is fleet
    And next customer id is 2202

  @terminal
  Scenario: Existing retail customer can be marked as fleet
    Given one retail customer named Greenway Taxis with id 2008
    When that customer type is set to fleet
    Then that customer type is fleet
