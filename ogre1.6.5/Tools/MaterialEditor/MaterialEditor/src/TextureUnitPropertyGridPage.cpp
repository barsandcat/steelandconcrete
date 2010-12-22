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
#include "TextureUnitPropertyGridPage.h"

#include <boost/bind.hpp>

#include <wx/propgrid/advprops.h>

#include "OgreCommon.h"
#include "OgreTextureUnitState.h"

#include "TextureUnitController.h"

using namespace Ogre;

BEGIN_EVENT_TABLE(TextureUnitPropertyGridPage, wxPropertyGridPage)
EVT_PG_CHANGED(-1, TextureUnitPropertyGridPage::propertyChanged)
END_EVENT_TABLE()

TextureUnitPropertyGridPage::TextureUnitPropertyGridPage(TextureUnitController* controller)
: mController(controller)
{
}

TextureUnitPropertyGridPage::~TextureUnitPropertyGridPage()
{
}

void TextureUnitPropertyGridPage::populate()
{
	const TextureUnitState* tus = mController->getTextureUnit();

	mTextureNameId = Append(new wxStringProperty(wxT("Texture Name"), wxPG_LABEL, wxString(tus->getTextureName().c_str(), wxConvUTF8)));
	mCurrentFrameId = Append(new wxIntProperty(wxT("Current Frame"), wxPG_LABEL, tus->getCurrentFrame()));

	wxPGChoices btChoices;
	btChoices.Add(wxT("Fragment"), TextureUnitState::BT_FRAGMENT);
	btChoices.Add(wxT("Vertex"), TextureUnitState::BT_VERTEX);
	mBindingTypeId = Append(new wxEnumProperty(wxT("Binding Type"), wxPG_LABEL, btChoices, TextureUnitState::BT_FRAGMENT));

	wxPGChoices ctChoices;
	ctChoices.Add(wxT("Named"), TextureUnitState::CONTENT_NAMED);
	ctChoices.Add(wxT("Shadow"), TextureUnitState::CONTENT_SHADOW);
	mContentTypeId = Append(new wxEnumProperty(wxT("Content Type"), wxPG_LABEL, ctChoices, TextureUnitState::CONTENT_NAMED));

	wxPGChoices pfChoices;
	pfChoices.Add(wxT("Unknown"), PF_UNKNOWN);
	pfChoices.Add(wxT("L8"), PF_L8);
	pfChoices.Add(wxT("BYTE_L"), PF_BYTE_L);
	pfChoices.Add(wxT("L16"), PF_L16);
	pfChoices.Add(wxT("SHORT_L"), PF_SHORT_L);
	pfChoices.Add(wxT("A8"), PF_A8);
	pfChoices.Add(wxT("BYTE_A"), PF_BYTE_A);
	pfChoices.Add(wxT("A4L4"), PF_A4L4);
	pfChoices.Add(wxT("BYTE_LA"), PF_BYTE_LA);
	pfChoices.Add(wxT("R5G6B5"), PF_R5G6B5);
	pfChoices.Add(wxT("B5G6R5"), PF_B5G6R5);
	pfChoices.Add(wxT("R3G3B2"), PF_R3G3B2);
	pfChoices.Add(wxT("A4R4G4B4"), PF_A4R4G4B4);
	pfChoices.Add(wxT("A1R5G5B5"), PF_A1R5G5B5);
	pfChoices.Add(wxT("R8G8B8"), PF_R8G8B8);
	pfChoices.Add(wxT("B8G8R8"), PF_B8G8R8);
	pfChoices.Add(wxT("A8R8G8B8"), PF_A8R8G8B8);
	pfChoices.Add(wxT("A8B8G8R8"), PF_A8B8G8R8);
	pfChoices.Add(wxT("B8G8R8A8"), PF_B8G8R8A8);
	pfChoices.Add(wxT("R8G8B8A8"), PF_R8G8B8A8);
	pfChoices.Add(wxT("X8R8G8B8"), PF_X8R8G8B8);
	pfChoices.Add(wxT("X8B8G8R8"), PF_X8B8G8R8);
	pfChoices.Add(wxT("BYTE_RGB"), PF_BYTE_RGB);
	pfChoices.Add(wxT("BYTE_BGR"), PF_BYTE_BGR);
	pfChoices.Add(wxT("BYTE_BGRA"), PF_BYTE_BGRA);
	pfChoices.Add(wxT("BYTE_RGBA"), PF_BYTE_RGBA);
	pfChoices.Add(wxT("A2R10G10B10"), PF_A2R10G10B10);
	pfChoices.Add(wxT("A2B10G10R10"), PF_A2B10G10R10);
	pfChoices.Add(wxT("DXT1"), PF_DXT1);
	pfChoices.Add(wxT("DXT2"), PF_DXT2);
	pfChoices.Add(wxT("DXT3"), PF_DXT3);
	pfChoices.Add(wxT("DXT4"), PF_DXT4);
	pfChoices.Add(wxT("DXT5"), PF_DXT5);
	pfChoices.Add(wxT("FLOAT16_R"), PF_FLOAT16_R);
	pfChoices.Add(wxT("FLOAT16_RGB"), PF_FLOAT16_RGB);
	pfChoices.Add(wxT("FLOAT16_RGBA"), PF_FLOAT16_RGBA);
	pfChoices.Add(wxT("FLOAT32_R"), PF_FLOAT32_R);
	pfChoices.Add(wxT("FLOAT32_RGB"), PF_FLOAT32_RGB);
	pfChoices.Add(wxT("FLOAT32_RGBA"), PF_FLOAT32_RGBA);
	pfChoices.Add(wxT("FLOAT16_GR"), PF_FLOAT16_GR);
	pfChoices.Add(wxT("FLOAT32_GR"), PF_FLOAT32_GR);
	pfChoices.Add(wxT("PF_DEPTH"), PF_DEPTH);
	pfChoices.Add(wxT("SHORT_RGBA"), PF_SHORT_RGBA);
	pfChoices.Add(wxT("SHORT_GR"), PF_SHORT_GR);
	pfChoices.Add(wxT("SHORT_RGB"), PF_SHORT_RGB);
	pfChoices.Add(wxT("PF_COUNT"), PF_COUNT);
	mPixelFormatId = Append(new wxEnumProperty(wxT("Desired Format"), wxPG_LABEL, pfChoices, PF_UNKNOWN));

	mNumMipMapsId = Append(new wxIntProperty(wxT("Mip Maps")));
}

void TextureUnitPropertyGridPage::propertyChanged(wxPropertyGridEvent& event)
{
	wxPGId id = event.GetProperty();

}
