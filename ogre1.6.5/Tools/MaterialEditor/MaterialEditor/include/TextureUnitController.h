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
#ifndef _TEXTUREUNITCONTROLLER_H_
#define _TEXTUREUNITCONTROLLER_H_

#include "OgreTexture.h"
#include "OgreTextureUnitState.h"

class PassController;

using namespace Ogre;

class TextureUnitController
{
public:
	TextureUnitController();
	TextureUnitController(TextureUnitState* textureUnit);
	TextureUnitController(PassController* parent, TextureUnitState* textureUnit);
	virtual ~TextureUnitController();

	TextureUnitState* getTextureUnit() const;
	PassController* getParentController() const;
protected:

	PassController* mParentController;
	TextureUnitState* mTextureUnit;
};

#endif // _TEXTUREUNITCONTROLLER_H_


