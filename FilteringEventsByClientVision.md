Game Events:
  * UnitEnter
  * UnitLeave
  * UnitRemove - unit deleted
  * UnitCreate - unit crated
  * UnitCommandDone - ?
  * TileTemperature

Vision events:
  * TileShow
  * TileHide

How to calculate what tiles are visible? How to update visible tiles set?
  * Using Breadth-first limited search to find-out what tiles are visible on client request
  * Store events in tiles
  * Store move in tile event and move out tile event separately on each tile
  * Store what tiles were visible last update, if some are not visible now, hide them next update