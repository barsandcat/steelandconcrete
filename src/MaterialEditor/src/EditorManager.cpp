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
#include "EditorManager.h"

#include <boost/bind.hpp>

#include <wx/aui/auibook.h>

#include "Editor.h"
#include "EditorEventArgs.h"

template<> EditorManager* Ogre::Singleton<EditorManager>::msSingleton = 0;

EditorManager& EditorManager::getSingleton(void)
{
	assert( msSingleton );  return ( *msSingleton );
}

EditorManager* EditorManager::getSingletonPtr(void)
{
	return msSingleton;
}

EditorManager::EditorManager(wxAuiNotebook* notebook)
: mEditorNotebook(notebook), mActiveEditor(NULL)
{
	registerEvents();

	if(mEditorNotebook != NULL)
	{
		Connect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED, wxAuiNotebookEventHandler(EditorManager::OnPageChanged));
		Connect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE, wxAuiNotebookEventHandler(EditorManager::OnPageClosed));

		mEditorNotebook->PushEventHandler(this);
	}
}

EditorManager::~EditorManager()
{

}

wxAuiNotebook* EditorManager::getEditorNotebook() const
{
	return mEditorNotebook;
}

void EditorManager::setEditorNotebook(wxAuiNotebook* notebook)
{
	if(mEditorNotebook != NULL)
	{
		Disconnect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED, wxAuiNotebookEventHandler(EditorManager::OnPageChanged));
		Disconnect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE, wxAuiNotebookEventHandler(EditorManager::OnPageClosed));

		mEditorNotebook->RemoveEventHandler(this);
	}

	mEditorNotebook = notebook;

	if(mEditorNotebook != NULL)
	{
		Connect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CHANGED, wxAuiNotebookEventHandler(EditorManager::OnPageChanged));
		Connect(wxEVT_COMMAND_AUINOTEBOOK_PAGE_CLOSE, wxAuiNotebookEventHandler(EditorManager::OnPageClosed));

		mEditorNotebook->PushEventHandler(this);
	}
}

void EditorManager::openEditor(EditorBase* editor)
{
	assert(mEditorNotebook != NULL);

	mEditors.push_back(editor);
	mEditorNotebook->AddPage(editor->getControl(), editor->getName(), true);

	mEditorIndexMap[editor] = mEditorNotebook->GetPageIndex(editor->getControl());

	editor->subscribe(EditorBase::NameChanged, boost::bind(&EditorManager::nameChanged, this, _1));

	setActiveEditor(editor);
}

void EditorManager::closeEditor(EditorBase* editor)
{
	assert(mEditorNotebook != NULL);

	if(mEditorIndexMap.find(editor) != mEditorIndexMap.end())
	{
		if(mActiveEditor == editor)
		{
			mActiveEditor->deactivate();
			mActiveEditor = NULL;
		}

		int index = mEditorIndexMap[editor];
		mEditorNotebook->RemovePage(index);
	}
}

EditorBase* EditorManager::findEditor(const wxString& name)
{
	EditorList::iterator it;
	EditorBase* editor = NULL;
	for(it = mEditors.begin(); it != mEditors.end(); ++it)
	{
		editor = (*it);
		if(editor->getName() == name) return editor;
	}

	return NULL;
}

EditorBase* EditorManager::getActiveEditor() const
{
	return mActiveEditor;
}

void EditorManager::setActiveEditor(EditorBase* editor)
{
	if(mActiveEditor == editor) return;

	if(mActiveEditor != NULL) mActiveEditor->deactivate();

	mActiveEditor = editor;

	if(mActiveEditor != NULL)
	{
		mActiveEditor->activate();

		if(mEditorNotebook != NULL && (mEditorIndexMap.find(mActiveEditor) != mEditorIndexMap.end()))
		{
			mEditorNotebook->SetSelection(mEditorIndexMap[mActiveEditor]);
		}
	}

	fireEvent(ActiveEditorChanged, EditorEventArgs(mActiveEditor));
}

const EditorList& EditorManager::getEditors() const
{
	return mEditors;
}

void EditorManager::registerEvents()
{
	registerEvent(ActiveEditorChanged);
}

void EditorManager::nameChanged(EventArgs& args)
{
	EditorEventArgs eea = dynamic_cast<EditorEventArgs&>(args);
	EditorBase* editor = eea.getEditor();
	if(mEditorIndexMap.find(editor) != mEditorIndexMap.end())
	{
		int index = mEditorIndexMap[editor];
		mEditorNotebook->SetPageText(index, editor->getName());
	}
}

void EditorManager::OnPageChanged(wxAuiNotebookEvent& event)
{
	int index = event.GetSelection();

	if(mActiveEditor != NULL)
	{
		if(mEditorIndexMap.find(mActiveEditor) != mEditorIndexMap.end())
		{
			int oldIndex = mEditorIndexMap[mActiveEditor];

			if(index != oldIndex)
			{
				mActiveEditor->deactivate();

				EditorIndexMap::iterator it;
				for(it = mEditorIndexMap.begin(); it != mEditorIndexMap.end(); ++it)
				{
					if(it->second == index)
					{
						setActiveEditor(it->first);
						break;
					}
				}
			}
		}
	}
	else
	{
		EditorIndexMap::iterator it;
		for(it = mEditorIndexMap.begin(); it != mEditorIndexMap.end(); ++it)
		{
			if(it->second == index)
			{
				setActiveEditor(it->first);
			}
		}
	}
}

void EditorManager::OnPageClosed(wxAuiNotebookEvent& event)
{
	int index = event.GetSelection();

	EditorBase* editor = NULL;
	EditorIndexMap::iterator it;
	for(it = mEditorIndexMap.begin(); it != mEditorIndexMap.end(); ++it)
	{
		if(it->second == index)
		{
			editor = it->first;
			editor->deactivate();
			mEditorIndexMap.erase(it);
			break;
		}
	}

	if(editor != NULL)
	{
		if(editor == mActiveEditor)
			mActiveEditor = NULL;

		EditorList::iterator lit;
		for(lit = mEditors.begin(); lit != mEditors.end(); ++lit)
		{
			if((*lit) == editor)
			{
				mEditors.erase(lit);
				break;
			}
		}
	}

	// Is this handled by OnPageChanged?
	int selIndex = event.GetSelection();
	for(it = mEditorIndexMap.begin(); it != mEditorIndexMap.end(); ++it)
	{
		if(it->second == index)
		{
			setActiveEditor(it->first);
			break;
		}
	}
}

