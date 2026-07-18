## About
**Auto Pause On Death** is a mod which allows you to control when the game should pause if you die while playing a level.

## Waypoints
A **waypoint** represents a certain point in the level where dying should pause the game.
Waypoints have four different modes:
- From Start Only (STA), where if you die after passing that percentage of the level from 0%, the game will pause
- From StartPos Only (SP), where if you die after passing that percentage of the level relative to a StartPos, the game will pause (for example, if you have a StartPos at 45% and a waypoint set to SP with a trigger of 5%, the game will pause if you die after 50%).
- On Exact Percentage (ONP), where if you die exactly on that percentage in the level, the game will pause (for example, pausing only if you die at 20%, and nowhere else).
- From Anywhere (ANY), which will pause when any of the previously stated cases are met.

Waypoints can either be **global** or **level-specific**. A global waypoint means it will save and work across any level you play, and a level-specific waypoint will only work on a particular level. Waypoints are level-specific by default.

Only classic levels are supported. This includes playtesting editor levels, but not in the editor menu.
## Editing Waypoints
Waypoints can be created, edited and configured to your liking through the **pause menu**. Click the "AutoPause" button to bring up the UI. The UI will only show waypoints that affect the level you are playing (that is, global waypoints and ones specific to the level). Toggling a waypoint from global to level-specific means it will become specific to the level you are currently playing.

## Why not just use the Auto Retry setting?
Auto Pause On Death aims to provide fine-tuned control to only pause under specific circumstances instead of on every attempt. Bringing up the pause menu instead of the auto retry menu also allows you to use other third-party tools (**none are included with this mod**) to inspect your death, as many of these tools do not work with the auto retry menu.

If you want to inspect your deaths when you're far into a level, and can't pause for whatever reason (e.g. if you can't react quick enough to the death, or focusing on that too is too distracting) then this is the mod for you!

## Bug Reporting
If you find any bugs using this mod, please open up an Issue on the [GitHub repository](https://github.com/12345koip/gd_no_auto_retry_mod) to report it!
