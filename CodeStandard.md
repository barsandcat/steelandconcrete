# Code standard #
> ## Editor setup ##
    1. Indent: 4 spaces, no tabs
    1. Charset: UTF8
    1. Line endings: CR/LF
> ## Formatting and naming ##
    1. Identifiers
```
    TileGrid        // Класс
    IInterface      // Интерфейс
    MoveTile        // Функция
    SetTilePosition // Функция записи 
    GetTilePosition // Функция чтения
    IsEmpty         // Функция-предикат
    mField          // Поле
    someVariable    // Локальная переменная
    aParameter      // Параметр функции
    СONST_PARAM     // Константа
    SocketSharedPtr // Умный указатель на объект 
    ListScopedArray // Умный указатель на массив
```
> > > NO gloabs.
    1. Braces and spaces
      * Положение фигурных скобок - всегда на новой линии, парные скобки выровнены по вертикали
      * Между именем функции и скобой пробел не ставится
      * Между ключевым словом и скобой ставится один пробел
      * Бинарные операторы отделяются от операндов пробелами
      * Унарные операторы не отделяются от операнда пробелом
      * Скобки не отделяются от содержимого пробелами
```
void Killer::Function(int aParameter) 
{
    const int max = 1;
    const bool key = false;
    if (aParameter < max && !key)
    {                  
        mField = aParameter;
    }
}
```
    1. Позиция `*` и &
```

Type* mFiled;
int& aParameter;
```
    1. cpp header
```
#include <pch.h> 
#include <header.h>

#include <Ogre.h>
```