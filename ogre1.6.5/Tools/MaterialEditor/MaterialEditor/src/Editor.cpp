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
#include "Editor.h"

#include <wx/control.h>

#include "EditorEventArgs.h"

EditorBase::EditorBase()
: mName(wxT("Editor"))
{
	registerEvents();
}

EditorBase::~EditorBase()
{
}

wxControl* EditorBase::getControl() const
{
	return mControl;
}

void EditorBase::setControl(wxControl* control)
{
	mControl = control;
}

void EditorBase::registerEvents()
{
	registerEvent(NameChanged);
	registerEvent(DirtyStateChanged);
}




const wxString& EditorBase::getName() const
{
	return mName;
}

void EditorBase::setName(const wxString& name)
{
	mName = name;

	fireEvent(NameChanged, EditorEventArgs(this));
}

void EditorBase::activate()
{
	// Do nothing
}

void EditorBase::deactivate()
{
	// Do nothing
}

bool EditorBase::isDirty()
{
	return false;
}

void EditorBase::save()
{
	// Do nothing
}

void EditorBase::saveAs()
{
	// Do nothing
}

bool EditorBase::isSaveAsAllowed()
{
	return false;
}

bool EditorBase::isRedoable()
{
	return false;
}

void EditorBase::redo()
{
	// Do nothing
}

bool EditorBase::isUndoable()
{
	return false;
}

void EditorBase::undo()
{
	// Do nothing
}

bool EditorBase::isCuttable()
{
	return false;
}

void EditorBase::cut()
{
	// Do nothing
}

bool EditorBase::isCopyable()
{
	return false;
}

void EditorBase::copy()
{
	// Do nothing
}

bool EditorBase::isPastable()
{
	return false;
}

void EditorBase::paste()
{
	// Do nothing
}
