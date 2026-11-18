Feature: Quote pricing for retail and trade customers
  As a dealership user
  I need quotes to use the current price build-up
  So that totals stay correct to the penny before any fleet change

  # Behaviour contract only. Verified by CTest (unit/seam). Terminal UI flows use @terminal + BDD/runner.
  # Truth: docs/SYSTEM.md Pricing and calc_quote in calc.c
  # Retail/trade goldens only. Fleet scenarios live in BDD/features/fleet.feature.

  Scenario: Retail quote with salesman 5% and trade-in matches invoice 50230
    Given a list price of 9295.00
    And a retail customer
    And a salesman discount of 5%
    And a trade-in of 1500.00
    When a quote is calculated
    Then the trade discount is 0.00
    And the salesman discount amount is 464.75
    And the net price is 8830.25
    And the delivery fee is 150.00
    And the VAT is 1796.05
    And the registration fee is 55.00
    And the total is 9331.30

  Scenario: Trade quote with 10% off list and no salesman discount matches invoice 50231
    Given a list price of 13495.00
    And a trade customer
    And a salesman discount of 0%
    And a trade-in of 0.00
    When a quote is calculated
    Then the trade discount is 1349.50
    And the salesman discount amount is 0.00
    And the net price is 12145.50
    And the delivery fee is 150.00
    And the VAT is 2459.10
    And the registration fee is 55.00
    And the total is 14809.60

  Scenario: Retail quote with salesman 8% matches invoice 50232
    Given a list price of 10495.00
    And a retail customer
    And a salesman discount of 8%
    And a trade-in of 0.00
    When a quote is calculated
    Then the trade discount is 0.00
    And the salesman discount amount is 839.60
    And the net price is 9655.40
    And the delivery fee is 150.00
    And the VAT is 1961.08
    And the registration fee is 55.00
    And the total is 11821.48

  Scenario: Salesman discount below 0 is treated as 0
    Given a list price of 1000.00
    And a retail customer
    And a salesman discount of -5%
    And a trade-in of 0.00
    When a quote is calculated
    Then the salesman discount percent used is 0
    And the salesman discount amount is 0.00
    And the net price is 1000.00
    And the total is 1435.00

  Scenario: Salesman discount above 15 is treated as 15
    Given a list price of 1000.00
    And a retail customer
    And a salesman discount of 40%
    And a trade-in of 0.00
    When a quote is calculated
    Then the salesman discount percent used is 15
    And the salesman discount amount is 150.00
    And the net price is 850.00
    And the total is 1255.00

  Scenario: Trade discount then salesman discount on what is left
    Given a list price of 10000.00
    And a trade customer
    And a salesman discount of 5%
    And a trade-in of 0.00
    When a quote is calculated
    Then the trade discount is 1000.00
    And the salesman discount amount is 450.00
    And the net price is 8550.00
    And the VAT is 1740.00
    And the total is 10495.00

  Scenario: Money amounts round to two decimal places
    Given a list price of 1234.56
    And a trade customer
    And a salesman discount of 0%
    And a trade-in of 0.00
    When a quote is calculated
    Then the trade discount is 123.46
    And the net price is 1111.10
    And the VAT is 252.22
    And the total is 1568.32

  Scenario: Delivery and registration fees are fixed
    Given a list price of 9295.00
    And a retail customer
    And a salesman discount of 5%
    And a trade-in of 1500.00
    When a quote is calculated
    Then the delivery fee is 150.00
    And the registration fee is 55.00
