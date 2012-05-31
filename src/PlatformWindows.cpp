#include <pch.h>

#include <Platform.h>


DEFINE_string(data_dir, "res", "Game data directory");

Ogre::String GetFlagsFilePath()
{
    Ogre::String home = getenv("APPDATA");
    return home + "\\steelandconcrete\\steelandconcrete.flags";
}

void LaunchServer()
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);
    ZeroMemory( &pi, sizeof(pi) );


    CreateProcess("steelandconcrete_server.exe", NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);

    CloseHandle( pi.hProcess );
    CloseHandle( pi.hThread );
}



