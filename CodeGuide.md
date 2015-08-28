## Client-server code separation ##
  * There should be no functions that use different code path for client and server. There should be if server or clients clauses.
  * Just as with generic encapsulation - isolate functionality not structures. It was bad idea to separate server and client units, and it was good idea to use FOV class.