Настройка gcc и vc++ компиляторов, так чтобы они вели себя похоже:

gcc:
  * + -Wreturn-type: Проверка что функция возвращает что нибудь
  * + -Wsign-compare: signed/unsigned mismatch (C4018)
  * + -Wunused-variable: unreferenced local variable (C4101)

  * ? Проверка на то что в срр не подключен рсн для gcc

vc++
  * - C4244 conversion from 'float' to 'int', possible loss of data
  * - С4800 forcing value to bool 'true' or 'false'
  * - C4996 The POSIX name for this item is deprecated
  * - C4068 Unknown pragma
  * + C4239 nonstandard extension used ... A non-const reference may only be bound to an lvalue.