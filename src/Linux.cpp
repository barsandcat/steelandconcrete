#include <pch.h>
#include <ClientApp.h>
#include <ClientLog.h>

bool FileExists(const Ogre::String aFilename)
{
    struct stat fileInfo;
    return 0 == stat(aFilename.c_str(), &fileInfo);
}

void CheckConfigFile(const Ogre::String aHomeDir)
{
    Ogre::String configDir = aHomeDir + "/.steelandconcrete";
    if (!FileExists(configDir))
    {
        mkdir(configDir.c_str(), S_IRUSR | S_IWUSR | S_IXUSR);
    }
    Ogre::String configFile = configDir + "/steelandconcrete.cfg";
    if (!FileExists(configFile))
    {
        std::ofstream of(configFile.c_str());
        if (!of)
            throw std::runtime_error("Не могу создать конфигурационный файл!");

        of << "[OpenGL Rendering Subsystem]" << std::endl;
        of << "FSAA=0" << std::endl;
        of << "Full Screen=No" << std::endl;
        of << "RTT Preferred Mode=FBO" << std::endl;
        of << "Video Mode=1024 x 768" << std::endl;
        of << "[Resources]" << std::endl;
        of << "FileSystem=res/OgreCore" << std::endl;
        of << "FileSystem=res/audio" << std::endl;
        of << "FileSystem=res/textures" << std::endl;
        of << "FileSystem=res/scripts" << std::endl;
        of << "FileSystem=res/models" << std::endl;
        of << "FileSystem=res/bgui" << std::endl;
        of << "FileSystem=res/quickgui" << std::endl;
        of.close();
    }
}


int main(int argc, char **argv)
{
    // Create application object
    try
    {
        Ogre::String home = getenv("HOME");
        CheckConfigFile(home);
        ClientApp app(home + "/.steelandconcrete/steelandconcrete.cfg");
        GetLog() << "ClientApp ready";
        app.MainLoop();
        GetLog() << "Main loop ended";
        chdir("cpd");
    }
    catch (std::exception& e)
    {
        std::cerr << "An exception has occured: " << e.what();
        return 1;
    }
    catch (...)
    {
        std::cerr << "Exception!";
        return 2;
    }
    return 0;
}
