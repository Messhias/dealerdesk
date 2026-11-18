Feature: Quote and sell vehicle
  As a dealership user
  I need quote and sell rules to stay stable
  So that sold stock is blocked and invoices pack discounts and fees correctly

  # Behaviour contract only. Verified by CTest (unit/seam). Terminal UI flows use @terminal + BDD/runner.
  # Truth: docs/SYSTEM.md Quote / Sell / Screen vs saved invoice, and main.c sell path.
  # Tests prefer sale seams (no stdin). Cancel means the sale is not applied.

  Scenario: Sold vehicle cannot be quoted
    Given a vehicle with status sold
    When quote eligibility is checked
    Then the vehicle cannot be quoted

  Scenario: Available vehicle can be quoted
    Given a vehicle with status in stock
    When quote eligibility is checked
    Then the vehicle can be quoted

  Scenario: Invoice packs trade and salesman discounts into disc
    Given a list price of 10000.00
    And a trade customer
    And a salesman discount of 5%
    And a trade-in of 0.00
    When a quote is calculated
    And an invoice is packed from that quote
    Then the packed invoice disc is 1450.00
    # trade 10% of 10000.00 = 1000.00; salesman 5% of 9000.00 = 450.00; sum = 1450.00

  Scenario: Invoice packs delivery and registration into fees 205.00
    Given a list price of 9295.00
    And a retail customer
    And a salesman discount of 5%
    And a trade-in of 1500.00
    When a quote is calculated
    And an invoice is packed from that quote
    Then the packed invoice fees are 205.00
    And the packed invoice disc is 464.75

  Scenario: Confirming a sale marks vehicle sold and appends invoice
    Given a private temp working tree with a data directory
    And one in-stock vehicle id 1101 priced 9295.00
    And one retail customer id 2101
    And next invoice number 50233
    When a sale is completed for that vehicle and customer with salesman 5% and trade-in 1500.00
    And invoices stock and ids are saved
    And in-memory tables are cleared
    And all data files are loaded
    Then exactly 1 invoice is in memory
    And that invoice disc is 464.75
    And that invoice fees are 205.00
    And that invoice total is 9331.30
    And the vehicle status is sold
    And next invoice number is 50234

  Scenario: Cancelling a sale leaves stock and invoices unchanged
    Given a private temp working tree with a data directory
    And one in-stock vehicle id 1101 priced 9295.00
    And one retail customer id 2101
    And next invoice number 50233
    When the sale is cancelled without completing
    And invoices stock and ids are saved
    And in-memory tables are cleared
    And all data files are loaded
    Then exactly 0 invoices are in memory
    And the vehicle status is in stock
    And next invoice number is 50233
