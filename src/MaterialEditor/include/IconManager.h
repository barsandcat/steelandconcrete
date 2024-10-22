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
#ifndef _ICONMANAGER_H_
#define _ICONMANAGER_H_

#include "wx/bitmap.h"
#include "OgreSingleton.h"

#include "../xpm/close.xpm"
#include "../xpm/copy.xpm"
#include "../xpm/cut.xpm"
#include "../xpm/material.xpm"
#include "../xpm/material_script.xpm"
#include "../xpm/ogre.xpm"
#include "../xpm/pass.xpm"
#include "../xpm/paste.xpm"
#include "../xpm/program_script.xpm"
#include "../xpm/project.xpm"
#include "../xpm/project_new.xpm"
#include "../xpm/project_save.xpm"
#include "../xpm/save.xpm"
#include "../xpm/save_as.xpm"
#include "../xpm/shader.xpm"
#include "../xpm/technique.xpm"
#include "../xpm/workspace.xpm"

#include "../xpm/application-x-font-pcf.xpm"
#include "../xpm/application-x-kpovmodeler.xpm"
#include "../xpm/image-x-generic.xpm"
#include "../xpm/folder.xpm"
#include "../xpm/utilities-file-archiver-3.xpm"
#include "../xpm/unknown.xpm"

class IconManager : public Ogre::Singleton<IconManager>
{
public:
	IconManager();
	~IconManager();

	enum IconType
	{
		CLOSE,
		COPY,
		CUT,
		MATERIAL,
		MATERIAL_SCRIPT,
		OGRE,
		PASS,
		PASTE,
		PROGRAM_SCRIPT,
		PROJECT,
		PROJECT_NEW,
		PROJECT_SAVE,
		SAVE,
		SAVE_AS,
		SHADER,
		TECHNIQUE,
		WORKSPACE,
		FONT,
		MESH,
		FILE_SYSTEM,
		TEXTURE,
		ZIP,
		UNKNOWN,
	};

	const wxBitmap& getIcon(IconType type) const;

	static IconManager& getSingleton(void);
	static IconManager* getSingletonPtr(void);

private:
	typedef std::map<IconType, wxBitmap> IconMap;
	IconMap mIconMap;
};
#endif
