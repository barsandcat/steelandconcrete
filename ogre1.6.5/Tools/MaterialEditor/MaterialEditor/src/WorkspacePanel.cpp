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
#include "WorkspacePanel.h"

#include <boost/bind.hpp>

#include <wx/bitmap.h>
#include <wx/button.h>
#include <wx/imaglist.h>
#include <wx/menu.h>
#include <wx/sizer.h>
#include <wx/aui/auibook.h>

#include "OgreMaterial.h"
#include "OgreMaterialManager.h"
#include "OgreMaterialSerializer.h"
#include "OgrePass.h"
#include "OgreTechnique.h"

#include "EditorManager.h"
#include "EventArgs.h"
#include "IconManager.h"
#include "MaterialScriptEditor.h"
#include "MaterialWizard.h"
#include "PassWizard.h"
#include "Project.h"
#include "ProjectWizard.h"
#include "TechniqueWizard.h"
#include "Workspace.h"
#include "WorkspaceEventArgs.h"

#define WORKSPACE_IMAGE 0
#define PROJECT_IMAGE 1
#define MATERIAL_SCRIPT_IMAGE 2
#define MATERIAL_IMAGE 3
#define TECHNIQUE_IMAGE 4
#define PASS_IMAGE 5

boost::signal< void (Ogre::MaterialPtr) > WorkspacePanel::mMaterialSelectedSignal;
boost::signal< void (Ogre::Technique*) > WorkspacePanel::mTechniqueSelectedSignal;
boost::signal< void (Ogre::Pass*) > WorkspacePanel::mPassSelectedSignal;

const long ID_TREE_CTRL = wxNewId();
const long ID_MENU_NEW = wxNewId();
const long ID_MENU_NEW_PROJECT = wxNewId();
const long ID_MENU_NEW_MATERIAL_SCRIPT = wxNewId();
const long ID_MENU_NEW_MATERIAL = wxNewId();
const long ID_MENU_NEW_TECHNIQUE = wxNewId();
const long ID_MENU_NEW_PASS = wxNewId();
const long ID_MENU_ADD_MATERIAL = wxNewId();
const long ID_MENU_EDIT = wxNewId();
const long ID_MENU_PASS_ENABLED = wxNewId();
const long ID_MENU_DELETE = wxNewId();

BEGIN_EVENT_TABLE(WorkspacePanel, wxPanel)
	EVT_TREE_ITEM_RIGHT_CLICK(ID_TREE_CTRL, WorkspacePanel::OnRightClick)
	EVT_TREE_ITEM_ACTIVATED(ID_TREE_CTRL, WorkspacePanel::OnActivate)
	EVT_TREE_SEL_CHANGED(ID_TREE_CTRL, WorkspacePanel::OnSelectionChanged)
	EVT_TREE_END_LABEL_EDIT(ID_TREE_CTRL, WorkspacePanel::LabelChanged)
	EVT_MENU(ID_MENU_NEW_PROJECT, WorkspacePanel::OnNewProject)
	EVT_MENU(ID_MENU_NEW_MATERIAL_SCRIPT, WorkspacePanel::OnNewMaterialScript)
	EVT_MENU(ID_MENU_NEW_MATERIAL, WorkspacePanel::OnNewMaterial)
	EVT_MENU(ID_MENU_NEW_TECHNIQUE, WorkspacePanel::OnNewTechnique)
	EVT_MENU(ID_MENU_NEW_PASS, WorkspacePanel::OnNewPass)
	EVT_MENU(ID_MENU_ADD_MATERIAL, WorkspacePanel::OnAddMaterial)
	EVT_MENU(ID_MENU_EDIT, WorkspacePanel::OnEdit)
	EVT_UPDATE_UI(ID_MENU_NEW_MATERIAL, WorkspacePanel::OnUpdateMaterialMenuItem)
	EVT_UPDATE_UI(ID_MENU_NEW_TECHNIQUE, WorkspacePanel::OnUpdateTechniqueMenuItem)
	EVT_UPDATE_UI(ID_MENU_NEW_PASS, WorkspacePanel::OnUpdatePassMenuItem)
END_EVENT_TABLE()

WorkspacePanel::WorkspacePanel(wxWindow* parent,
			   wxWindowID id /* = wxID_ANY */,
			   const wxPoint& pos /* = wxDefaultPosition */,
			   const wxSize& size /* = wxDefaultSize */,
			   long style /* = wxTAB_TRAVERSAL | wxNO_BORDER */,
			   const wxString& name /* = wxT("Workspace Panel")) */)
			   : mImageList(NULL), wxPanel(parent, id, pos, size, style, name)
{
	createPanel();

	Workspace::getSingletonPtr()->subscribe(Workspace::ProjectAdded, boost::bind(&WorkspacePanel::projectAdded, this, _1));
}

WorkspacePanel::~WorkspacePanel()
{
}

void WorkspacePanel::createPanel()
{
	mSizer = new wxFlexGridSizer(1, 1, 0, 0);
	mSizer->AddGrowableCol(0);
	mSizer->AddGrowableRow(0);
	mSizer->SetFlexibleDirection(wxVERTICAL);
	mSizer->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	mTreeCtrl = new wxTreeCtrl(this, ID_TREE_CTRL, wxDefaultPosition, wxDefaultSize,
                            wxNO_BORDER | wxTR_EDIT_LABELS | wxTR_FULL_ROW_HIGHLIGHT |
                            wxTR_HAS_BUTTONS | wxTR_SINGLE);
	mTreeCtrl->AssignImageList(getImageList());
	mRootId = mTreeCtrl->AddRoot(wxT("Workspace"), WORKSPACE_IMAGE);

	mSizer->Add(mTreeCtrl, 0, wxALL | wxEXPAND, 0);

	SetSizer(mSizer);
	Layout();
}

wxImageList* WorkspacePanel::getImageList()
{
	if(mImageList == NULL)
	{
		mImageList = new wxImageList(16, 16, true, 6);
		mImageList->Add(IconManager::getSingleton().getIcon(IconManager::WORKSPACE));
		mImageList->Add(IconManager::getSingleton().getIcon(IconManager::PROJECT));
		mImageList->Add(IconManager::getSingleton().getIcon(IconManager::MATERIAL_SCRIPT));
		mImageList->Add(IconManager::getSingleton().getIcon(IconManager::MATERIAL));
		mImageList->Add(IconManager::getSingleton().getIcon(IconManager::TECHNIQUE));
		mImageList->Add(IconManager::getSingleton().getIcon(IconManager::PASS));
	}

	return mImageList;
}

void WorkspacePanel::showContextMenu(wxPoint point, wxTreeItemId id)
{
	wxMenu contextMenu;
	appendNewMenu(&contextMenu);
	contextMenu.AppendSeparator();
	if(isProject(id))
	{
	    appendProjectMenuItems(&contextMenu);
	}
	contextMenu.Append(ID_MENU_EDIT, wxT("Edit"));
	PopupMenu(&contextMenu, point);
}

void WorkspacePanel::appendNewMenu(wxMenu* menu)
{
	wxMenu* newMenu = new wxMenu();
	newMenu->Append(ID_MENU_NEW_PROJECT, wxT("Project"));
	newMenu->Append(ID_MENU_NEW_MATERIAL_SCRIPT, wxT("Material Script"));
	newMenu->Append(ID_MENU_NEW_MATERIAL, wxT("Material"));
	newMenu->Append(ID_MENU_NEW_TECHNIQUE, wxT("Technique"));
	newMenu->Append(ID_MENU_NEW_PASS, wxT("Pass"));

	menu->AppendSubMenu(newMenu, wxT("New"));
}

void WorkspacePanel::appendProjectMenuItems(wxMenu* menu)
{
	menu->Append(ID_MENU_ADD_MATERIAL, wxT("Add Material"));
}

Project* WorkspacePanel::getProject(wxTreeItemId id)
{
	for(ProjectIdMap::iterator it = mProjectIdMap.begin(); it != mProjectIdMap.end(); ++it)
	{
		if(it->second == id) return it->first;
	}

	return NULL;
}

Ogre::MaterialPtr WorkspacePanel::getMaterial(wxTreeItemId id)
{
	for(MaterialIdMap::iterator it = mMaterialIdMap.begin(); it != mMaterialIdMap.end(); ++it)
	{
		if(it->second == id) return it->first;
	}

	return Ogre::MaterialPtr();
}

Ogre::Technique* WorkspacePanel::getTechnique(wxTreeItemId id)
{
	for(TechniqueIdMap::iterator it = mTechniqueIdMap.begin(); it != mTechniqueIdMap.end(); ++it)
	{
		if(it->second == id) return it->first;
	}

	return NULL;
}

Ogre::Pass* WorkspacePanel::getPass(wxTreeItemId id)
{
	for(PassIdMap::iterator it = mPassIdMap.begin(); it != mPassIdMap.end(); ++it)
	{
		if(it->second == id) return it->first;
	}

	return NULL;
}

bool WorkspacePanel::isWorkspace(wxTreeItemId id)
{
	return mRootId == id;
}

bool WorkspacePanel::isProject(wxTreeItemId id)
{
	return getProject(id) != NULL;
}

bool WorkspacePanel::isMaterial(wxTreeItemId id)
{
	return !getMaterial(id).isNull();
}

bool WorkspacePanel::isTechnique(wxTreeItemId id)
{
	return getTechnique(id) != NULL;
}

bool WorkspacePanel::isPass(wxTreeItemId id)
{
	return getPass(id) != NULL;
}

void WorkspacePanel::OnRightClick(wxTreeEvent& event)
{
	showContextMenu(event.GetPoint(), event.GetItem());
}

void WorkspacePanel::OnActivate(wxTreeEvent& event)
{
	OnEdit(event);
}

void WorkspacePanel::OnSelectionChanged(wxTreeEvent& event)
{
	wxTreeItemId id = event.GetItem();
	if(isProject(id))
	{
	}
	else if(isMaterial(id))
	{
		mMaterialSelectedSignal(getMaterial(id));
	}
	else if(isTechnique(id))
	{
	    mTechniqueSelectedSignal(getTechnique(id));
	}
	else if(isPass(id))
	{
	    mPassSelectedSignal(getPass(id));
	}
	// else its the workspace so just leave the list empty as if nothing were selected
}

void WorkspacePanel::OnNewProject(wxCommandEvent& event)
{
	ProjectWizard* wizard = new ProjectWizard();
	wizard->Create(this, wxID_ANY, wxT("New Project"), wxNullBitmap, wxDefaultPosition, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
	wizard->RunWizard(wizard->getProjectPage()); // This seems unnatural, seems there must be a better way to deal with wizards

	wizard->Destroy();

	delete wizard;
}

void WorkspacePanel::OnNewMaterialScript(wxCommandEvent& event)
{
	wxTreeItemId id = mTreeCtrl->GetSelection();
	mTreeCtrl->AppendItem(id, wxT("Material Script"), MATERIAL_SCRIPT_IMAGE);
}

void WorkspacePanel::OnNewMaterial(wxCommandEvent& event)
{
	wxTreeItemId id = mTreeCtrl->GetSelection();

	MaterialWizard* wizard = new MaterialWizard();
	wizard->mMaterialAddedSignal.connect(boost::bind(&WorkspacePanel::projectMaterialAdded, this, _1, _2));
	wizard->Create(this, wxID_ANY, wxT("New Project"), wxNullBitmap, wxDefaultPosition, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
	wizard->getMaterialPage()->setProject(getProject(id));
	wizard->RunWizard(wizard->getMaterialPage()); // This seems unnatural, seems there must be a better way to deal with wizards

	wizard->Destroy();

	delete wizard;
}

void WorkspacePanel::OnNewTechnique(wxCommandEvent& event)
{
	Project* project = NULL;
	Ogre::MaterialPtr material;

	wxTreeItemId selId = mTreeCtrl->GetSelection();
	if(isProject(selId))
	{
		project = getProject(selId);
	}
	else if(isMaterial(selId))
	{
		wxTreeItemId projectId = mTreeCtrl->GetItemParent(selId);
		project = getProject(projectId);

		material = getMaterial(selId);
	}

	TechniqueWizard* wizard = new TechniqueWizard();
	wizard->Create(this, wxID_ANY, wxT("New Technique"), wxNullBitmap, wxDefaultPosition, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
	wizard->mTechniqueAddedSignal.connect(boost::bind(&WorkspacePanel::TechniqueAdded, this, _1, _2));
	wizard->getTechniquePage()->setProject(project);
	wizard->getTechniquePage()->setMaterial(material);
	wizard->RunWizard(wizard->getTechniquePage()); // This seems unnatural, seems there must be a better way to deal with wizards

	wizard->Destroy();

	delete wizard;
}

void WorkspacePanel::OnNewPass(wxCommandEvent& event)
{
	Project* project = NULL;
	Ogre::MaterialPtr material;
	Ogre::Technique* technique = NULL;

	wxTreeItemId selId = mTreeCtrl->GetSelection();
	if(isProject(selId))
	{
		project = getProject(selId);
	}
	else if(isMaterial(selId))
	{
		wxTreeItemId projectId = mTreeCtrl->GetItemParent(selId);
		project = getProject(projectId);

		material = getMaterial(selId);
	}
	else if(isTechnique(selId))
	{
		wxTreeItemId materialId = mTreeCtrl->GetItemParent(selId);
		material = getMaterial(materialId);

		wxTreeItemId projectId = mTreeCtrl->GetItemParent(materialId);
		project = getProject(projectId);

		technique = getTechnique(selId);
	}

	PassWizard* wizard = new PassWizard();
	wizard->mPassAddedSignal.connect(boost::bind(&WorkspacePanel::TechniquePassAdded, this, _1, _2));
	wizard->Create(this, wxID_ANY, wxT("New Pass"), wxNullBitmap, wxDefaultPosition, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
	wizard->getPassPage()->setProject(project);
	wizard->getPassPage()->setMaterial(material);
	wizard->getPassPage()->setTechnique(technique);
	wizard->RunWizard(wizard->getPassPage());

	wizard->Destroy();

	delete wizard;
}

void WorkspacePanel::OnAddMaterial(wxCommandEvent& event)
{
	wxFileDialog * openDialog = new wxFileDialog(this, wxT("Add a Material"), wxEmptyString, wxEmptyString,
		wxT("Material Files (*.material)|*.material|All Files (*.*)|*.*"));

	if(openDialog->ShowModal() == wxID_OK)
	{
		wxString path = openDialog->GetPath();
		if(path.EndsWith(wxT(".material")))
		{
			wxTreeItemId selId = mTreeCtrl->GetSelection();
			if(isProject(selId))
			{
				Project* project = getProject(selId);

				MaterialScriptEditor* editor = new MaterialScriptEditor(EditorManager::getSingletonPtr()->getEditorNotebook());
				editor->loadFile(path);
				int index = (int)path.find_last_of('\\');
				if(index == -1) index = (int)path.find_last_of('/');
				editor->setName((index != -1) ? path.substr(index + 1, path.Length()) : path);

				EditorManager::getSingletonPtr()->openEditor(editor);

				Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName("GrassMaterial");
				project->addMaterial(mat);
				projectMaterialAdded(project, mat);
			}
		}
	}
}

void WorkspacePanel::OnEdit(wxCommandEvent& event)
{
	wxTreeItemId selId = mTreeCtrl->GetSelection();
	if(isMaterial(selId))
	{
		Ogre::MaterialPtr mc = getMaterial(selId);

		EditorManager* editorManager = EditorManager::getSingletonPtr();
		EditorBase* editor = editorManager->findEditor(wxString(mc->getName().c_str(), wxConvUTF8));
		EditorBase* editorMat = editorManager->findEditor(wxString((mc->getName() + ".material").c_str(), wxConvUTF8));
		if(editor != NULL)
		{
			editorManager->setActiveEditor(editor);
		}
		else if(editorMat != NULL)
		{
			editorManager->setActiveEditor(editorMat);
		}
		else
		{
			Ogre::MaterialSerializer* ms = new Ogre::MaterialSerializer();
			ms->queueForExport(mc, true);
			String script = ms->getQueuedAsString();

			MaterialScriptEditor* materialEditor = new MaterialScriptEditor(editorManager->getEditorNotebook(), wxID_ANY);
			Ogre::String name = mc->getName();
			name += ".material";
			materialEditor->setName(wxString(name.c_str(), wxConvUTF8));
			materialEditor->SetText(wxString(script.c_str(), wxConvUTF8));

			editorManager->openEditor(materialEditor);
		}
	}
}

void WorkspacePanel::OnUpdateMaterialMenuItem(wxUpdateUIEvent& event)
{
	event.Enable(!Workspace::getSingletonPtr()->getProjects()->empty());
}

void WorkspacePanel::OnUpdateTechniqueMenuItem(wxUpdateUIEvent& event)
{
	bool enable = false;
	const ProjectList* projects = Workspace::getSingletonPtr()->getProjects();

	ProjectList::const_iterator it;
	for(it = projects->begin(); it != projects->end(); ++it)
	{
		if(!(*it)->getMaterials()->empty())
		{
			enable = true;
			break;
		}
	}

	event.Enable(enable);
}

void WorkspacePanel::OnUpdatePassMenuItem(wxUpdateUIEvent& event)
{
	bool enable = false;
	const ProjectList* projects = Workspace::getSingletonPtr()->getProjects();

	ProjectList::const_iterator pit;
	for(pit = projects->begin(); pit != projects->end(); ++pit)
	{
		const MaterialControllerList* materials = (*pit)->getMaterials();
		MaterialControllerList::const_iterator mit;
		for(mit = materials->begin(); mit != materials->end(); ++mit)
		{
			if((*mit)->getNumTechniques() > 0)
			{
				enable = true;
				break;
			}
		}
	}

	event.Enable(enable);
}

void WorkspacePanel::LabelChanged(wxTreeEvent& event)
{
    wxTreeItemId id = event.GetItem();
    if (isTechnique(id))
    {
        Ogre::Technique* tc = getTechnique(id);
        tc->setName(Ogre::String(event.GetLabel().mb_str()));
    }
}

void WorkspacePanel::projectAdded(EventArgs& args)
{
	WorkspaceEventArgs wea = dynamic_cast<WorkspaceEventArgs&>(args);
	Project* project = wea.getProject();

	wxTreeItemId id = mTreeCtrl->AppendItem(mRootId, project->getName().c_str(), PROJECT_IMAGE);
	mTreeCtrl->SelectItem(id, true);

	mProjectIdMap[project] = id;
}


void WorkspacePanel::projectMaterialAdded(Project* project, Ogre::MaterialPtr material)
{
	wxTreeItemId projectId = mProjectIdMap[project];
	wxTreeItemId id = mTreeCtrl->AppendItem(projectId, wxString(material->getName().c_str(), wxConvUTF8), MATERIAL_IMAGE);
	mTreeCtrl->SelectItem(id, true);

	mMaterialIdMap[material] = id;
}

void WorkspacePanel::TechniqueAdded(Ogre::MaterialPtr mc, Ogre::Technique* tc)
{
	wxTreeItemId materialId = mMaterialIdMap[mc];
	wxTreeItemId id = mTreeCtrl->AppendItem(materialId, wxString(tc->getName().c_str(), wxConvUTF8), TECHNIQUE_IMAGE);
	mTreeCtrl->SelectItem(id, true);

	mTechniqueIdMap[tc] = id;
}

void WorkspacePanel::TechniquePassAdded(Ogre::Technique* tc, Ogre::Pass* pc)
{
	wxTreeItemId techniqueId = mTechniqueIdMap[tc];
	wxTreeItemId id = mTreeCtrl->AppendItem(techniqueId, wxString(pc->getName().c_str(), wxConvUTF8), PASS_IMAGE);
	mTreeCtrl->SelectItem(id, true);

	mPassIdMap[pc] = id;
}
