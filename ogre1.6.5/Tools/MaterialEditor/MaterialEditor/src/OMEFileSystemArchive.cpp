#include "OMEFileSystemArchive.h"
#include <OgreException.h>

#include <OgrePrerequisites.h>

static bool is_absolute_path(const char* path)
{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
    if (isalpha(path[0]) && path[1] == ':')
        return true;
#endif
    return path[0] == '/' || path[0] == '\\';
}
//-----------------------------------------------------------------------
static Ogre::String concatenate_path(const Ogre::String& base, const Ogre::String& name)
{
    if (base.empty() || is_absolute_path(name.c_str()))
        return name;
    else
        return base + '/' + name;
}

Ogre::DataStreamPtr OMEFileSystemArchive::open(const Ogre::String& filename) const
{
    Ogre::String full_path = concatenate_path(mName, filename);

    // Use filesystem to determine size
    // (quicker than streaming to the end and back)
    struct stat tagStat;
    int ret = stat(full_path.c_str(), &tagStat);
    assert(ret == 0 && "Problem getting file size" );

    // Always open in binary mode
    std::ifstream *origStream = OGRE_NEW_T(std::ifstream, Ogre::MEMCATEGORY_GENERAL)();
    origStream->open(full_path.c_str(), std::ios::in | std::ios::binary);

    // Should check ensure open succeeded, in case fail for some reason.
    if (origStream->fail())
    {
        OGRE_DELETE_T(origStream, basic_ifstream, Ogre::MEMCATEGORY_GENERAL);
        OGRE_EXCEPT(Ogre::Exception::ERR_FILE_NOT_FOUND,
                    "Cannot open file: " + filename,
                    "FileSystemArchive::open");
    }

    /// Construct return stream, tell it to delete on destroy
    Ogre::FileStreamDataStream* stream = OGRE_NEW Ogre::FileStreamDataStream(mName + ":" + filename,
                                         origStream, tagStat.st_size, true);
    return Ogre::DataStreamPtr(stream);
}


const Ogre::String& OMEFileSystemArchiveFactory::getType(void) const
{
    static Ogre::String name = "OMEFileSystem";
    return name;
}
