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
    Ogre::String configFile = configDir + "/steelandconcrete1.cfg";
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

void LaunchServer()
{
    pid_t pID = fork();
    if (pID == 0) // child
    {
        // Code only executed by child process
        const char* const cmd[] = { "steelandconcrete_d_server", 0 };
        execv("steelandconcrete_d_server", const_cast<char* const*>(cmd));

    }
    else if (pID < 0) // failed to fork
    {
        GetLog() << "Not launced";
    }
    else // parent
    {
        GetLog() << "Launched";
    }
}


int main(int argc, char **argv)
{
    int res = -1;
    // Create application object
    try
    {
        Ogre::String home = getenv("HOME");
        CheckConfigFile(home);
        ClientApp app(home + "/.steelandconcrete/steelandconcrete1.cfg");
        GetLog() << "ClientApp ready";
        app.MainLoop();
        GetLog() << "Main loop ended";
        res = 0;
    }
    catch (std::exception& e)
    {
        std::cerr << "An exception has occured: " << e.what();
        res = 1;
    }
    catch (...)
    {
        std::cerr << "Exception!";
        res = 2;
    }
    int cdres = chdir("cpd");// this is to save gmon.out
    return res;
}
