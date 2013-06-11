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
#ifndef _MATERIALEDITORFRAME_H_
#define _MATERIALEDITORFRAME_H_

#include <wx/wx.h>
#include <wx/treectrl.h>

#include "OgreRenderSystem.h"
#include "OgreRoot.h"

#include <wx/aui/framemanager.h>


class wxAuiNotebook;
class wxNotebook;
class wxPropertyGridManager;
class wxTreeCtrl;

namespace
{
class RenderSystem;
class Root;
}

class DocPanel;
class EventArgs;
class LogPanel;
class PropertiesPanel;
class OgreControl;

typedef std::map<Ogre::String, Ogre::MaterialPtr> MaterialMap;
typedef std::map<Ogre::String, MaterialMap> ScriptMap;
typedef std::map<Ogre::String, ScriptMap> ArchiveMap;
typedef std::map<Ogre::String, ArchiveMap> GroupMap;

class MaterialEditorFrame : public wxFrame
{
public:
    MaterialEditorFrame(wxWindow* parent = NULL);
    ~MaterialEditorFrame();
    void CreateScene();
protected:
    void createAuiManager(void);
    void createAuiNotebookPane(void);
    void createManagementPane(void);
    void createInformationPane(void);
    void createPropertiesPane();
    void createOgrePane(void);

    void createMenuBar(void);
    void createFileMenu(void);
    void createEditMenu(void);
    void createWindowMenu(void);
    void createHelpMenu(void);

    void OnActivate(wxActivateEvent& event);

    void OnActiveEditorChanged(EventArgs& args);

    void OnNewMaterial(wxCommandEvent& event);
    void OnFileOpen(wxCommandEvent& event);
    void OnFileSave(wxCommandEvent& event);
    void OnAddFileSystem(wxCommandEvent& event);
    void OnFileClose(wxCommandEvent& event);
    void OnFileExit(wxCommandEvent& event);
    void OnEditUndo(wxCommandEvent& event);
    void OnEditRedo(wxCommandEvent& event);
    void OnEditCut(wxCommandEvent& event);
    void OnEditCopy(wxCommandEvent& event);
    void OnEditPaste(wxCommandEvent& event);

    void OnResourceSelected(wxTreeEvent& event);
    void OnFileSelected(wxTreeEvent& event);
    void OnRenderTimer(wxTimerEvent& event);
private:
    void FillObjectTree(Ogre::Entity* aEntity);
    void FillMaterialObjectTree(Ogre::MaterialPtr aMaterial);

    void AddMeshToObjectTree(const wxTreeItemId aParentNodeId, Ogre::MeshPtr aMesh);
    void AddSkeletonToObjectTree(const wxTreeItemId aParentNodeId, Ogre::SkeletonPtr aSkeleton);
    void AddSubMeshToObjectTree(const wxTreeItemId aParentNodeId, Ogre::SubMesh* aSubMesh);
    void AddMaterialToObjectTree(const wxTreeItemId aParentNodeId, Ogre::MaterialPtr aMaterial);
    void AddTechiqueToObjectTree(const wxTreeItemId aParentNodeId, Ogre::Technique* aTechnique);
    void AddPassToObjectTree(const wxTreeItemId aParentNodeId, Ogre::Pass* aPass);
    void FillMaterialMap();
    void FillResourceTree();
    void AddArchiveToResourceTree(wxTreeItemId aGroupId, Ogre::String aGroupName, Ogre::Archive* aArchive);
    void GetTuPassTecMatNames(const wxTreeItemId aSelectedItemId,
                                               Ogre::String &aTuName,
                                               Ogre::String &aPassName,
                                               Ogre::String &aTecName,
                                               Ogre::String &aMatName) const;
    const MaterialMap GetScriptMaterials(Ogre::String aGroup, Ogre::String aArchive,
                                   Ogre::String aFile) const;
    const wxTreeItemId CreateObjectTreeItem(const wxTreeItemId aParent,
                                            Ogre::String aName,
                                            int aImage);
    wxMenuBar* mMenuBar;
    wxMenu* mFileMenu;
    wxMenu* mEditMenu;
    wxMenu* mWindowMenu;
    wxMenu* mHelpMenu;

    wxAuiManager mAuiManager;

    wxAuiNotebook* mAuiNotebook;
    wxAuiNotebook* mInformationNotebook;

    wxTreeCtrl* mResourceTree;
    wxTreeCtrl* mObjectTree;
    PropertiesPanel* mPropertiesPanel;

    Ogre::SceneManager* m_sm;

    LogPanel* mLogPanel;
    DocPanel* mDocPanel;
    OgreControl* mOgreControl;

    GroupMap mMaterialMap;

    wxTimer* mRenderTimer;

    DECLARE_EVENT_TABLE();
};

#endif // _MATERIALEDITORFRAME_H_
