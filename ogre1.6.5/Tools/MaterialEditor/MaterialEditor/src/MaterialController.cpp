/*
-------------------------------------------------------------------------
This source file is a part of OGRE
(Object-oriented Graphics Rendering Engine)

For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2006 Torus Knot Software Ltd
Also see acknowledgements in Readme.html

This library is free software; you can redistribute it and/or modify it
under the terms of the GNU Lesser General Public License (LGPL) as
published by the Free Software Foundation; either version 2.1 of the
License, or (at your option) any later version.

This library is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this library; if not, write to the Free Software Foundation,
Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA or go to
http://www.gnu.org/copyleft/lesser.txt
-------------------------------------------------------------------------
*/
#include "MaterialController.h"

#include "OgreColourValue.h"
#include "OgreTechnique.h"

MaterialController::MaterialController()
: mMaterialPtr(NULL)
{
}

MaterialController::MaterialController(MaterialPtr materialPtr)
: mMaterialPtr(materialPtr)
{
}

MaterialController::~MaterialController()
{
}

MaterialPtr MaterialController::getMaterial() const
{
	return mMaterialPtr;
}

void MaterialController::setMaterial(MaterialPtr mp)
{
	mMaterialPtr = mp;
}

Ogre::Technique* MaterialController::getTechniqueController(const String& name)
{
	Ogre::Technique* tc;
	TechniqueControllerList::iterator it;
	for(it = mTechniqueControllers.begin(); it != mTechniqueControllers.end(); ++it)
	{
		tc = (*it);
		if(tc->getName() == name) return tc;
	}

	return NULL;
}

const TechniqueControllerList* MaterialController::getTechniqueControllers() const
{
	return &mTechniqueControllers;
}

Ogre::Technique* MaterialController::createTechnique(const String& name)
{
	Technique* t = mMaterialPtr->createTechnique();
	t->setName(name);

	return t;
}
