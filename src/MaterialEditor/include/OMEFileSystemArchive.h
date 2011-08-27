#ifndef OMEFILESYSTEMARCHIVE_H
#define OMEFILESYSTEMARCHIVE_H
#include <OgreFileSystem.h>

class OMEFileSystemArchive : public Ogre::FileSystemArchive
{
public:
    OMEFileSystemArchive(const Ogre::String& name, const Ogre::String& archType ): Ogre::FileSystemArchive(name, archType){}
    Ogre::DataStreamPtr open(const Ogre::String& filename, bool readOnly = true) const;
};

class OMEFileSystemArchiveFactory : public Ogre::FileSystemArchiveFactory
{
public:
    const Ogre::String& getType() const;
    Ogre::Archive *createInstance( const Ogre::String& name )
    {
        return OGRE_NEW OMEFileSystemArchive(name, "OMEFileSystem");
    }
};

#endif // OMEFILESYSTEMARCHIVE_H
