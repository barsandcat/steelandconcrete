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
#ifndef _TEXTUREUNITPROPERTYGRIDPAGE_H_
#define _TEXTUREUNITPROPERTYGRIDPAGE_H_

#include <wx/wx.h>
#include <wx/propgrid/manager.h>
namespace Ogre
{
	class TextureUnitState;
}

class TextureUnitPropertyGridPage : public wxPropertyGridPage
{
public:
	TextureUnitPropertyGridPage(Ogre::TextureUnitState* controller);
	virtual ~TextureUnitPropertyGridPage();

	virtual void populate();

	
protected:
	virtual void propertyChanged(wxPropertyGridEvent& event);

	Ogre::TextureUnitState* mController;

	wxPGId mTextureNameId;
	wxPGId mCubicTextureNameId;
	wxPGId mAnimatedTextureNameId;
	wxPGId mTextureDimensionsId;
	wxPGId mCurrentFrameId;
	wxPGId mFrameTextureNameId;
	wxPGId mBindingTypeId;
	wxPGId mContentTypeId;
	wxPGId mPixelFormatId;
	wxPGId mNumMipMapsId;
	wxPGId mAlphaId;
	wxPGId mTextureCoordSetId;
	wxPGId mTextureTransformId;
	wxPGId mTextureScrollId;
	wxPGId mTextureUScrollId;
	wxPGId mTextureVScrollId;
	wxPGId mTextureUScaleId;
	wxPGId mTextureVScaleId;
	wxPGId mTextureRotateId;
	wxPGId mTextureAddressingModeId;
	wxPGId mTextureBorderColourId;
	wxPGId mColourOperationExId;
	wxPGId mColourOpMultipassFallbackId;
	wxPGId mAlphaOperationId;
	wxPGId mEnvironmentMapId;
	wxPGId mScrollAnimationId;
	wxPGId mRotateAnimationId;
	wxPGId mTransformAnimationId;
	wxPGId mProjectiveTexturingId;
	wxPGId mTextureFilteringId;
	wxPGId mTextureAnisotropyId;
	wxPGId mTextureMipMapBiasId;
	wxPGId mNameId;
	wxPGId mTextureNameAliasId;

	DECLARE_EVENT_TABLE();
};

#endif // _TEXTUREUNITPROPERTYGRIDPAGE_H_
