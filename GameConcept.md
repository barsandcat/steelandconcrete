### Requirements ###
  * Multi-player - It is always fun
  * TCP/IP - Simple, and it is not action game
  * Thin client, server decides every thing - The only way to avoid cheating
  * Pure client-server protocol - client requests, server responds. Greatly simplifies protocol
  * Client performance dose not depend on map size. The only way to build **really** big worlds
  * Units scale from building to insects.

### Game design ###
  * Geodesic grid. The only way to correctly divide sphere surface. And we need sphere for climate model
  * No multi tile units. It will be to hard to render them properly on spherical surface, move them, build ai for them
  * Units have size. Tile have space. Space and size can be spcified for different heights: SizeSpace
  * Units direction does not matter. It is simple, and it works in many other games.
  * Units can have capacity. To have people inside buildings, cars etc
  * Tile has height and water volume. Water volume affects units on ground and under ground. We can not use plain water tile - it does not allow to build small rivers ponds, etc and we need them if we going to build some climate model.
  * Player interacts with world through his avatar unit. This is not an a god game. It is hard to build it multi-player other wise.
  * No fp camera. Game update step and tile based movement will result in very unpleasant movement.
  * No camera panning. Camera centres around you. We will show only small part of world around player, panning will break immersion. PlayerInterface
  * Players takes control over unit. Disconnect == release control of unit. I don't want people to waste their life in game, and I want sessions length like month. Than means all units in game are self sustainable AI's

### Planned ###
  * Climate model. Температура, давление, осадки, ветер, стихийные бедствия
  * Knowledge model. Книги, обучение по юнитно
  * Language evolution
  * Genome model. Для всех животных и растений - селекция, гетерозис, мутация
  * Parasites model. Чума, сложные циклы паразитов
  * Economy model. Обмен по юнитно, воровство, владение землей
  * Production model. Как обычно, добыча ресурсов, постройка, переработка
  * Stable update. Порядок обновления юнитов, зданий, тайлов не влияет на результат обновления
  * Time measure part of knowledge model. Возможны разные календари и их отсутствие.
  * Primary needs - Еда, вода, сон, туалет