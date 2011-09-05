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

Ogre::DataStreamPtr OMEFileSystemArchive::open(const Ogre::String& filename, bool readOnly) const
{
    Ogre::String full_path = concatenate_path(mName, filename);

    // Use filesystem to determine size
    // (quicker than streaming to the end and back)
    struct stat tagStat;
    int ret = stat(full_path.c_str(), &tagStat);
    assert(ret == 0 && "Problem getting file size" );
    (void)ret;  // Silence warning

    // Always open in binary mode
    // Also, always include reading
    std::ios::openmode mode = std::ios::in | std::ios::binary;
    std::istream* baseStream = 0;
    std::ifstream* roStream = 0;
    std::fstream* rwStream = 0;

    if (!readOnly && isReadOnly())
    {
        mode |= std::ios::out;
        rwStream = OGRE_NEW_T(std::fstream, Ogre::MEMCATEGORY_GENERAL)();
        rwStream->open(full_path.c_str(), mode);
        baseStream = rwStream;
    }
    else
    {
        roStream = OGRE_NEW_T(std::ifstream, Ogre::MEMCATEGORY_GENERAL)();
        roStream->open(full_path.c_str(), mode);
        baseStream = roStream;
    }


    // Should check ensure open succeeded, in case fail for some reason.
    if (baseStream->fail())
    {
        OGRE_DELETE_T(roStream, basic_ifstream, Ogre::MEMCATEGORY_GENERAL);
        OGRE_DELETE_T(rwStream, basic_fstream, Ogre::MEMCATEGORY_GENERAL);
        OGRE_EXCEPT(Ogre::Exception::ERR_FILE_NOT_FOUND,
                    "Cannot open file: " + filename,
                    "FileSystemArchive::open");
    }

    /// Construct return stream, tell it to delete on destroy
    Ogre::FileStreamDataStream* stream = 0;
    if (rwStream)
    {
        // use the writeable stream
        stream = OGRE_NEW Ogre::FileStreamDataStream(filename,
                                               rwStream, (size_t)tagStat.st_size, true);
    }
    else
    {
        // read-only stream
        Ogre::String newName = mName + ":" + filename;
        stream = OGRE_NEW Ogre::FileStreamDataStream(newName,
                                               roStream, (size_t)tagStat.st_size, true);
    }
    return Ogre::DataStreamPtr(stream);
}


const Ogre::String& OMEFileSystemArchiveFactory::getType(void) const
{
    static Ogre::String name = "OMEFileSystem";
    return name;
}
