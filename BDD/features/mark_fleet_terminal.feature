Feature: Mark existing customer as fleet via terminal
  As dealership staff
  I need to set an existing customer to fleet in the UI
  So that fleet quotes apply without editing data files by hand

  @terminal
  Scenario: Existing retail customer can be marked as fleet on the terminal
    When the application starts against isolated data
    And staff marks customer 2008 as fleet via the menus
    Then customer 2008 is persisted as fleet
    And a screenshot is saved as "mark_existing_fleet"
