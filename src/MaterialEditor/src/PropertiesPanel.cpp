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
#include "PropertiesPanel.h"

#include <boost/any.hpp>
#include <boost/bind.hpp>

#include <wx/button.h>
#include <wx/dcclient.h>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/toolbar.h>
#include <wx/propgrid/manager.h>
#include <wx/propgrid/advprops.h>

#include <OgreMaterial.h>
#include <OgreMaterialManager.h>
#include <OgrePass.h>
#include <OgreTechnique.h>

#include <MaterialPropertyGridPage.h>
#include <PassPropertyGridPage.h>
#include <TechniquePropertyGridPage.h>
#include "Workspace.h"

BEGIN_EVENT_TABLE(PropertiesPanel, wxPanel)
END_EVENT_TABLE()

PropertiesPanel::PropertiesPanel(wxWindow* parent,
                                 wxWindowID id /* = wxID_ANY */,
                                 const wxPoint& pos /* = wxDefaultPosition */,
                                 const wxSize& size /* = wxDefaultSize */,
                                 long style /* = wxTAB_TRAVERSAL | wxNO_BORDER */,
                                 const wxString& name /* = wxT("Workspace Panel")) */)
    : wxPanel(parent, id, pos, size, style, name)
{
    mGridSizer = new wxGridSizer(1, 1, 0, 0);

    mPropertyGrid = new wxPropertyGridManager(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
            wxPG_BOLD_MODIFIED | wxPG_SPLITTER_AUTO_CENTER | wxPG_DESCRIPTION | wxPGMAN_DEFAULT_STYLE);

    mGridSizer->Add(mPropertyGrid, 0, wxALL | wxEXPAND, 0);

    SetSizer(mGridSizer);
    Layout();

}

PropertiesPanel::~PropertiesPanel()
{
}

void PropertiesPanel::Clear()
{
    mPropertyGrid->Clear();
}

void PropertiesPanel::MaterialSelected(Ogre::MaterialPtr material)
{
    mPropertyGrid->Clear();
    MaterialPropertyGridPage* page = new MaterialPropertyGridPage(material);

    int index = mPropertyGrid->AddPage(wxEmptyString, wxPG_NULL_BITMAP, page);
    page->populate();

    mPropertyGrid->SelectPage(index);
    mPropertyGrid->Refresh();
}

void PropertiesPanel::TechniqueSelected(Ogre::Technique* tc)
{
    mPropertyGrid->Clear();
    TechniquePropertyGridPage* page = new TechniquePropertyGridPage(tc);

    int index = mPropertyGrid->AddPage(wxEmptyString, wxPG_NULL_BITMAP, page);
    page->populate();


    mPropertyGrid->SelectPage(index);
    mPropertyGrid->Refresh();
}

void PropertiesPanel::PassSelected(Ogre::Pass* pc)
{
    mPropertyGrid->Clear();
    PassPropertyGridPage* page = new PassPropertyGridPage(pc);

    int index = mPropertyGrid->AddPage(wxEmptyString, wxPG_NULL_BITMAP, page);
    page->populate();

    mPropertyGrid->SelectPage(index);
    mPropertyGrid->Refresh();
}


