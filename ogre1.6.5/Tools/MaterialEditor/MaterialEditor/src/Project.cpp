/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2006 Torus Knot Software Ltd
Also see acknowledgements in Readme.html

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.

You may alternatively use this source under the terms of a specific version of
the OGRE Unrestricted License provided you have obtained such a license from
Torus Knot Software Ltd.
-----------------------------------------------------------------------------
*/
#include "Project.h"

#include "OgreEntity.h"
#include "OgreLight.h"
#include "OgreMaterial.h"
#include "OgreMaterialManager.h"
#include "OgreSceneManager.h"
#include <OgreDataStream.h>

Project::Project(const wxString& name) :mName(name)
{
    Ogre::String fileName = Ogre::String(name.mb_str());
    std::ifstream *origStream = OGRE_NEW_T(std::ifstream, Ogre::MEMCATEGORY_GENERAL)(fileName.c_str(), std::ios::in | std::ios::binary);
    
    Ogre::LogManager::getSingleton().logMessage("File " + fileName + " ok, now parse it");
    Ogre::DataStreamPtr stream(OGRE_NEW Ogre::FileStreamDataStream(fileName, origStream, true));
    Ogre::MaterialManager::getSingleton().parseScript(stream, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
    // Now get materials
    Ogre::ResourceManager::ResourceMapIterator it = Ogre::MaterialManager::getSingleton().getResourceIterator();
    while (it.hasMoreElements())
    {
        Ogre::MaterialPtr material = it.getNext();
        if (material->getOrigin() == fileName)
        {
            Ogre::LogManager::getSingleton().logMessage("Found material " + material->getName());
            mMaterialControllers.push_back(material);
        }
    }
}

Project::~Project()
{
	mMaterialControllers.clear();
}

const wxString& Project::getName() const
{
	return mName;
}

void Project::addMaterial(Ogre::MaterialPtr materialPtr)
{
	mMaterialControllers.push_back(materialPtr);
}

Ogre::MaterialPtr Project::createMaterial(const Ogre::String& name)
{
	// TODO: Projects should probably have their own resource groups instead of using the default
	Ogre::MaterialPtr materialPtr = (Ogre::MaterialPtr)Ogre::MaterialManager::getSingletonPtr()->create(name, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	mMaterialControllers.push_back(materialPtr);
	return materialPtr;
}

Ogre::MaterialPtr Project::getMaterialController(const Ogre::String& name)
{
	Ogre::MaterialPtr mc;
	MaterialControllerList::iterator it;
	for(it = mMaterialControllers.begin(); it != mMaterialControllers.end(); ++it)
	{
		mc = (*it);
		if(mc->getName() == name) return mc;
	}

	return Ogre::MaterialPtr();
}

const MaterialControllerList& Project::getMaterials() const
{
	return mMaterialControllers;
}

