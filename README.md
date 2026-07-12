# About
**Auto Pause On Death** is a mod which allows you to control when the game should pause if you die while playing a level.

# Waypoints
A **waypoint** represents a certain point in the level where dying should pause the game.
Waypoints have four different modes:
- From Start Only (STA), where if you die after passing that percentage of the level from 0%, the game will pause
- From StartPos Only (SP), where if you die after passing that percentage of the level relative to a StartPos, the game will pause (for example, if you have a StartPos at 45% and a waypoint set to SP with a trigger of 5%, the game will pause if you die after 50%).
- On Exact Percentage (ONP), where if you die exactly on that percentage in the level, the game will pause (for example, pausing only if you die at 20%, and nowhere else).
- From Anywhere (ANY), which will pause when any of the previously stated cases are met.

Waypoints can either be **global** or **level-specific**. A global waypoint means it will save and work across any level you play, and a level-specific waypoint only works on a specific level. Waypoints are level-specific by default.

# Editing Waypoints
Waypoints can be created, edited and configured to your liking through the **pause menu**. Click the "AutoPause" button to bring up the UI.

# Issues and bugs
If you find any bugs using this mod, please open up an Issue on the GitHub repository to report it!
