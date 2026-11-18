Feature: Fleet quote on the terminal
  As dealership staff
  I need a fleet quote to show the Fleet discount line and total
  So that TICKET-142 acceptance is visible in the real UI

  @terminal
  Scenario: Fleet quote shows Fleet discount line and total
    Given isolated data has a fleet customer and a 10000 list vehicle
    When the application starts against isolated data
    And staff opens a quote for that fleet vehicle with zero discounts
    Then the quote shows Fleet discount 400.00 and total 11755.00
    And a screenshot is saved as "fleet_quote"
