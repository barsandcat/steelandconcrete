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
#ifndef _MATERIALPROPERTYGRIDPAGE_H_
#define _MATERIALPROPERTYGRIDPAGE_H_

#include <wx/wx.h>
#include <wx/propgrid/manager.h>
#include <OgreMaterial.h>

class MaterialPropertyGridPage : public wxPropertyGridPage
{
public:
	MaterialPropertyGridPage(Ogre::MaterialPtr aMaterial);
	virtual ~MaterialPropertyGridPage();
	virtual void populate();

protected:
	virtual void propertyChange(wxPropertyGridEvent& event);

	Ogre::MaterialPtr mMaterial;

	wxPGId mReceiveShadowsId;
	wxPGId mTransparencyShadowsId;

	DECLARE_EVENT_TABLE();
};

#endif // _MATERIALPROPERTYGRIDPAGE_H_
