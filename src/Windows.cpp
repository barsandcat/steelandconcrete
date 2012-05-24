#include "pch.h"
#include "ClientApp.h"

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
    Ogre::String ConfigFile = ConfigDir + "\\steelandconcrete1.cfg";
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
        of.close();
    }
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

int main(int argc, char **argv)
{
    google::InstallFailureSignalHandler();
    google::InitGoogleLogging(argv[0]);
    google::ParseCommandLineFlags(&argc, &argv, true);

    // Create application object
    try
    {
        Ogre::String home = getenv("APPDATA");
        CheckConfigFile(home);
        ClientApp app(home + "\\steelandconcrete\\steelandconcrete1.cfg");
        Ogre::LogManager::getSingletonPtr()->logMessage("App ready");
        app.MainLoop();
        Ogre::LogManager::getSingletonPtr()->logMessage("Main loop ended");
        google::ShutdownLogging();
        return 0;
    }
    catch (std::exception& e)
    {
        MessageBox(NULL, e.what(), "Steelandconcrete exception!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
        google::ShutdownLogging();
        return 1;
    }
    catch (...)
    {
        MessageBox(NULL, "Unknown excepiton", "Steelandconcrete occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
        google::ShutdownLogging();
        return 2;
    }

}
