#include "pch.h"
#include "ClientApp.h"

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

bool FileExists(const Ogre::String aFilename)
{
    struct stat fileInfo;
    return 0 == stat(aFilename.c_str(), &fileInfo);
}

void CheckConfigFile(const Ogre::String aHomeDir)
{
    Ogre::String ConfigDir = aHomeDir + "\\steelandconcrete";
    if (!FileExists(ConfigDir))
    {
        CreateDirectory(ConfigDir.c_str(), NULL); 
    }
    Ogre::String ConfigFile = ConfigDir + "\\steelandconcrete.cfg";
    if (!FileExists(ConfigFile))
    {
        std::ofstream of(ConfigFile.c_str());
        if (!of)
			throw std::runtime_error("Не могу создать конфигурационный файл!");

        of << "[OpenGL Rendering Subsystem]" << std::endl;
        of << "Colour Depth=32" << std::endl;
        of << "Display Frequency=N/A" << std::endl;
        of << "FSAA=0" << std::endl;
        of << "Full Screen=No" << std::endl;
        of << "RTT Preferred Mode=FBO" << std::endl;
        of << "VSync=No" << std::endl;
        of << "Video Mode=1024 x 768" << std::endl;
        of << "[Resources]" << std::endl;
        of << "FileSystem=res/OgreCore" << std::endl;
        of << "FileSystem=res/audio" << std::endl;
        of << "FileSystem=res/textures" << std::endl;
        of << "FileSystem=res/scripts" << std::endl;
        of << "FileSystem=res/models" << std::endl;
        of << "FileSystem=res/quickgui" << std::endl;
        of.close();
    }
}

void LaunchServer()
{
}

INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT)
{
    // Create application object
    try
    {
        Ogre::String home = getenv("APPDATA");
        CheckConfigFile(home);
        ClientApp app(home + "\\steelandconcrete\\steelandconcrete.cfg");
        Ogre::LogManager::getSingletonPtr()->logMessage("App ready");
        app.MainLoop();
        Ogre::LogManager::getSingletonPtr()->logMessage("Main loop ended");
        return 0;
    }
    catch (std::exception& e)
    {
        MessageBox(NULL, e.what(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
        return 1;
    }
    catch (...)
    {
        MessageBox(NULL, "Unknown excepiton", "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
        return 2;
    }
}
