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
#ifndef _PROJECT_H_
#define _PROJECT_H_

#include <list>

#include <boost/signal.hpp>

#include "OgreMaterial.h"
#include "OgreString.h"

#include "wx/string.h"
namespace Ogre
{
	class SceneManager;
}

class Project;

typedef std::list<Ogre::MaterialPtr> MaterialControllerList;

class Project
{
public:

	Project();
	Project(const wxString& name);
	virtual ~Project();

	const wxString& getName() const;

	void addMaterial(Ogre::MaterialPtr materialPtr);
	Ogre::MaterialPtr createMaterial(const Ogre::String& name);

	Ogre::MaterialPtr getMaterialController(const Ogre::String& name);

	const MaterialControllerList* getMaterials() const;



protected:
	wxString mName;
	MaterialControllerList mMaterialControllers;

};

#endif // _PROJECT_H_
