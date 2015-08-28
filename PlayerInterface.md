  1. One mouse click interface - no drag&drop, no right mouse button, no keyboard. To support touch screens
  1. Two views - all visible tiles around unit and current tile view. We need overview for movement orders and there is lots of units on tile to interact with.
  1. Less menus more visuals. Thats just more user friendly.
  1. Closup mode show tile where you standing and all neighbour tiles - so you can move in closeup mode and interact with units on adjacent tiles.
  1. In far mode you do not control camera
  1. In closeup mode you can rotate camera

? Hot to represent all tile units on screen, so player can easily click on each?
That really depends on how many units are on tile.
Obviously - there should not be too much - or that will not be tile based game.
But how many units are on tile really depends only on how large tile is.
We can link how tile represented to 2d size model - i.e. since clicks are in 2d too, mapping 2d sizes to screen will guarantee no overlapping units.

? Another problem related to tiles with multiple not oriented units is walls: even simple fence require entire tile space