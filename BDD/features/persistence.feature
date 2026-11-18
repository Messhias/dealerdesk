Feature: Flat-file persistence under data/
  As a dealership user
  I need stock, customers, invoices, and id counters to load and save reliably
  So that daily work survives restarts without corrupting production files

  # Behaviour contract only. Verified by CTest (unit/seam). Terminal UI flows use @terminal + BDD/runner.
  # Truth: docs/SYSTEM.md Persistence and dbfile.c
  # CTest chdirs into a private temp tree with its own data/; repo data/ stays clean.
  # Production dbfile is unchanged (fixed relative data/ path).

  Scenario: Load skips comment and empty lines in stock file
    Given a private temp working tree with a data directory
    And a stock.dat file with a comment line, a blank line, and one vehicle record
    When all data files are loaded
    Then exactly 1 vehicle is in memory
    And that vehicle id is 1001

  Scenario: Load skips comment and empty lines in customers file
    Given a private temp working tree with a data directory
    And a customers.dat file with a comment line, a blank line, and one customer record
    When all data files are loaded
    Then exactly 1 customer is in memory
    And that customer id is 2001
    And that customer type is R

  Scenario: Stock round-trip save then reload preserves the record
    Given a private temp working tree with a data directory
    And one vehicle in memory with id 1001 plate AB12 CDE
    When stock is saved
    And in-memory stock is cleared
    And all data files are loaded
    Then exactly 1 vehicle is in memory
    And that vehicle plate is AB12 CDE
    And that vehicle list price is 10495.00

  Scenario: Customers and next ids round-trip save then reload
    Given a private temp working tree with a data directory
    And one customer in memory with id 2001 name Sample Buyer type T
    And next ids vehicle 1013 customer 2009 invoice 50233
    When customers and ids are saved
    And in-memory customers and ids are cleared
    And all data files are loaded
    Then exactly 1 customer is in memory
    And that customer name is Sample Buyer
    And that customer type is T
    And next vehicle id is 1013
    And next customer id is 2009
    And next invoice number is 50233

  Scenario: Missing data files leave empty tables without crashing
    Given a private temp working tree with an empty data directory
    And next ids preset to vehicle 1000 customer 2000 invoice 50000
    When all data files are loaded
    Then exactly 0 vehicles are in memory
    And exactly 0 customers are in memory
    And exactly 0 invoices are in memory
    And next vehicle id remains 1000
    And next customer id remains 2000
    And next invoice number remains 50000

  Scenario: Sample repo data loads expected counts and next ids
    Given the repository data directory
    When all data files are loaded
    Then exactly 12 vehicles are in memory
    And exactly 8 customers are in memory
    And exactly 3 invoices are in memory
    And next vehicle id is 1013
    And next customer id is 2009
    And next invoice number is 50233
