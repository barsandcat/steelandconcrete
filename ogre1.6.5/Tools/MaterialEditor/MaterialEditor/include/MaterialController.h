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
#ifndef _MATERIALCONTROLLER_H_
#define _MATERIALCONTROLLER_H_

#include <list>
#include "OgreMaterial.h"

namespace Ogre
{
	class Technique;
}


using namespace Ogre;

typedef std::list<Ogre::Technique*> TechniqueControllerList;

class MaterialController
{
public:

	MaterialController();
	MaterialController(MaterialPtr material);
	virtual ~MaterialController();

	MaterialPtr getMaterial() const;
	void setMaterial(MaterialPtr mp);

	Ogre::Technique* getTechniqueController(const String& name);
	const TechniqueControllerList* getTechniqueControllers() const;

	Ogre::Technique* createTechnique(const String& name);

protected:
	MaterialPtr mMaterialPtr;

	TechniqueControllerList mTechniqueControllers;
};

#endif // _MATERIALCONTROLLER_H_

