Feature: CoolBoard

# ------- BATTERY -------
Scenario: Low level of battery reaches
    Given the CoolBoard is ON
    And I reset the CoolBoard
    When the battery of the CoolBoard reaches a low level
    And I replug the CoolBoard on DC
    Then the CoolBoard is working normally