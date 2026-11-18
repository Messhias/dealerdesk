Feature: Stock vehicle mutations
  As a dealership user
  I need add find price reserve and list rules to stay stable
  So that daily stock ops do not drift from current behaviour

  # Behaviour contract only. Verified by CTest (unit/seam). Terminal UI flows use @terminal + BDD/runner.
  # Truth: docs/SYSTEM.md Vehicles / Stock menus, and main.c stock path.
  # Truth exercised via stock.c seams used by main.c (no stdin). Plate match is exact.

  @terminal
  Scenario: Adding a vehicle requires plate and positive list price
    Given an empty stock table
    When add is attempted with an empty plate and list price 10000.00
    Then the add fails
    And the stock table remains empty
    When add is attempted with plate AB12 CDE and list price 0.00
    Then the add fails
    And the stock table remains empty

  @terminal
  Scenario: Valid add assigns next id and in-stock status
    Given an empty stock table
    And next vehicle id 1013
    When a vehicle is added with plate AB12 CDE make Ford model Fiesta year 2019 mileage 1000 cost 7000.00 list price 9295.00
    Then the add succeeds
    And exactly 1 vehicle is in memory
    And that vehicle id is 1013
    And that vehicle status is in stock
    And next vehicle id is 1014

  @terminal
  Scenario: Find by plate matches exactly including case
    Given one vehicle with plate yf20 hbn
    When find by plate uses yf20 hbn
    Then the first match index is 0
    When find by plate uses YF20 HBN
    Then there is no match

  @terminal
  Scenario: Change price updates when new price is positive
    Given one in-stock vehicle priced 10995.00
    When the list price is set to 11495.00
    Then the price change succeeds
    And that vehicle price is 11495.00

  @terminal
  Scenario: Change price leaves price unchanged when new price is not positive
    Given one in-stock vehicle priced 10995.00
    When the list price is set to 0.00
    Then the price change is rejected
    And that vehicle price is still 10995.00

  @terminal
  Scenario: Reserve toggles between in stock and reserved
    Given one in-stock vehicle
    When reserve is toggled
    Then the toggle succeeds
    And that vehicle status is reserved
    When reserve is toggled again
    Then the toggle succeeds
    And that vehicle status is in stock

  @terminal
  Scenario: Sold vehicle cannot be reserved
    Given one sold vehicle
    When reserve is toggled
    Then the toggle fails
    And that vehicle status remains sold

  @terminal
  Scenario: List stock omits sold vehicles
    Given one in-stock vehicle
    And one reserved vehicle
    And one sold vehicle
    When the stock list hides sold vehicles
    Then the in-stock vehicle is visible
    And the reserved vehicle is visible
    And the sold vehicle is hidden
    When the stock list shows all vehicles
    Then the sold vehicle is visible
