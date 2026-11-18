Feature: Terminal runner reaches the real binary
  As a developer
  I need the BDD terminal harness to drive dealerdesk
  So that fleet UI scenarios can assert production behaviour

  # Unit/seam scenarios stay in other feature files and CTest.
  # This file is executed by BDD/runner (tag @terminal).

  @terminal
  Scenario: Main Menu is visible after start
    When the application starts against isolated data
    Then the screen shows "Main Menu"
