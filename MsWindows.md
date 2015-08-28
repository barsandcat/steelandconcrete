## Небходимые файлы ##
  * [Microsoft Visual C++ 2008 Express Edition](http://www.microsoft.com/express/Downloads/#2008-Visual-CPP)
  * [NASM](http://www.nasm.us/pub/nasm/releasebuilds/?C=M;O=D)
  * [DirectX SDK](http://msdn.microsoft.com/en-us/directx/aa937788.aspx)
  * [OpenAL SDK](http://connect.creativelabs.com/openal/Downloads/Forms/AllItems.aspx)
  * [Python 2.x.x](http://www.activestate.com/activepython/downloads/)
  * [NSIS](http://nsis.sourceforge.net/Download/)
  * [TortoiseHG](http://tortoisehg.bitbucket.org/)
## Сборка проекта ##
  * Поставить все выше перечисленное
  * [Сделать чекаут репозитория](http://code.google.com/p/steelandconcrete/source/checkout).
  * Убедиться что python и nasm присутствуют в PATH
  * Открыть проект Vc9S&C.sln и работать! Э... тут уже 50 проектов... Основной проект - GameClient и GameServer
  * Для запуска проекта из среды необходимо указать рабочую директорию (по непонятной причине эта информация не сохраняется в файле проекта):
Откройте Project->Properties. В комбобоксе Configuration выберите AllConfigurations. В поле Configuration properties->Debuging->Working directory пропишите: bin