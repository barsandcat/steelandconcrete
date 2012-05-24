#include <pch.h>
#include <ClientApp.h>

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
        LOG(ERROR) << "Not launced";
    }
    else // parent
    {
        LOG(INFO) << "Launched";
    }
}


int main(int argc, char **argv)
{
    google::InstallFailureSignalHandler();
    google::InitGoogleLogging(argv[0]);
    google::ParseCommandLineFlags(&argc, &argv, true);

    int res = -1;
    // Create application object
    try
    {
        Ogre::String home = getenv("HOME");
        CheckConfigFile(home);
        ClientApp app(home + "/.steelandconcrete/steelandconcrete1.cfg", argc, argv);
        LOG(INFO) << "ClientApp ready";
        app.MainLoop();
        LOG(INFO) << "Main loop ended";
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
    // this is to save gmon.out
    return chdir("cpd");
}
