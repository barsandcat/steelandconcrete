# Size/Space #
## Goal: Limit amount of units on tile ##

### Straight forward solution: give each unit position and do not allow units models to intersect ###
  * **-** To heavy on computation
  * **-** Server needs to load models: **unacceptable!**
  * **+** No problem of placing horses over grass and horses under tree

### Simple solution: tile has limited volume, units have size ###
  * **+** Very cheap
  * **-** grass problem: if tile full of horses - there is no place for grass!: **unacceptable!**

Space problem interleaves with positioning problem.
Positioning in horizontal space can be thrown away, since tiles server for same purpose.

### Vertical positioning + simple solution ###
> For each height tile has limited volume
  * **+** No problem of placing horses over grass and horses under tree
  * **+** Cheap enough
  * **-** Some what strange way of moving up/down

This allows us to avoid grass/horse/tree problem.
Units can have volume for each height separately, and when unit moves in tile it claims what volumes at what heights it requires. To change it "height" unit have to move to same tile with new height claims.