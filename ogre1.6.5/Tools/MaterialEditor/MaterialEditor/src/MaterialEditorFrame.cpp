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
#include "MaterialEditorFrame.h"

#include <wx/bitmap.h>
#include <wx/notebook.h>
#include <wx/treectrl.h>
#include <wx/wxscintilla.h>
#include <wx/wizard.h>
#include <wx/aui/auibook.h>
#include <wx/aui/framemanager.h>
#include <wx/propgrid/manager.h>
#include <wx/propgrid/advprops.h>
#include <wx/imaglist.h>
#include <wx/wfstream.h>
#include <wx/sstream.h>

#include "OgreCamera.h"
#include "OgreColourValue.h"
#include "OgreConfigFile.h"
#include "OgreMaterial.h"
#include "OgreMaterialManager.h"
#include "OgreRoot.h"
#include "OgreString.h"
#include "OgreStringConverter.h"
#include "OgreVector3.h"

#include "CgEditor.h"
#include "DocPanel.h"
#include "EditorEventArgs.h"
#include "EditorManager.h"
#include "IconManager.h"
#include "LogPanel.h"
#include "MaterialPropertyGridPage.h"
#include "MaterialScriptEditor.h"
#include "PropertiesPanel.h"
#include "TechniquePropertyGridPage.h"
#include "PassPropertyGridPage.h"
#include <Workspace.h>
#include <wx/ogre/ogre.h>


const long ID_FILE_MENU_OPEN = wxNewId();
const long ID_FILE_MENU_SAVE = wxNewId();
const long ID_FILE_MENU_SAVE_AS = wxNewId();
const long ID_FILE_MENU_EXIT = wxNewId();

const long ID_EDIT_MENU_UNDO = wxNewId();
const long ID_EDIT_MENU_REDO = wxNewId();
const long ID_EDIT_MENU_CUT = wxNewId();
const long ID_EDIT_MENU_COPY = wxNewId();
const long ID_EDIT_MENU_PASTE = wxNewId();

const long ID_RESOURCE_TREE = wxNewId();
const long ID_FILE_TREE = wxNewId();
const long ID_RENDER_TIMER = wxNewId();

// Image list
const int WORKSPACE_IMAGE = 0;
const int GROUP_IMAGE = 1;
// Archive types
const int UNKNOWN_ARCHIVE_IMAGE = 2;
const int FILE_SYSTEM_IMAGE = 3;
const int ZIP_IMAGE = 4;
// Resource types
const int UNKNOWN_RESOURCE_IMAGE = 5;
const int COMPOSITE_IMAGE = 6;
const int MATERIAL_SCRIPT_IMAGE = 7;
const int MESH_FILE_IMAGE = 8;
const int ASM_PROGRAMM_IMAGE = 9;
const int HL_PROGRAMM_IMAGE = 10;
const int TEXTURE_IMAGE = 11;
const int SKELETON_IMAGE = 12;
const int FONT_IMAGE = 13;
// Materials script
const int MATERIAL_IMAGE = 14;
const int TECHNIQUE_IMAGE = 15;
const int PASS_IMAGE = 16;
const int TEXTURE_UNIT_IMAGE = 17;
const int MESH_IMAGE = 18;

const char* DISPLAY_NAME = "Display";

Ogre::Log::Stream GetLog()
{
    return  Ogre::LogManager::getSingleton().stream();
}

BEGIN_EVENT_TABLE(MaterialEditorFrame, wxFrame)
    // File Menu
    EVT_MENU (ID_FILE_MENU_OPEN,		 MaterialEditorFrame::OnFileOpen)
    EVT_MENU (ID_FILE_MENU_SAVE,		 MaterialEditorFrame::OnFileSave)
    EVT_MENU (ID_FILE_MENU_SAVE_AS,		 MaterialEditorFrame::OnFileSaveAs)
    EVT_MENU (ID_FILE_MENU_EXIT,		 MaterialEditorFrame::OnFileExit)
    // Edit Menu
    EVT_MENU (ID_EDIT_MENU_UNDO,  MaterialEditorFrame::OnEditUndo)
    EVT_MENU (ID_EDIT_MENU_REDO,  MaterialEditorFrame::OnEditRedo)
    EVT_MENU (ID_EDIT_MENU_CUT,	  MaterialEditorFrame::OnEditCut)
    EVT_MENU (ID_EDIT_MENU_COPY,  MaterialEditorFrame::OnEditCopy)
    EVT_MENU (ID_EDIT_MENU_PASTE, MaterialEditorFrame::OnEditPaste)
    // Resource tree
    EVT_TREE_SEL_CHANGED(ID_FILE_TREE, MaterialEditorFrame::OnFileSelected)
    EVT_TREE_SEL_CHANGED(ID_RESOURCE_TREE, MaterialEditorFrame::OnResourceSelected)

    EVT_TIMER(ID_RENDER_TIMER, MaterialEditorFrame::OnRenderTimer)
END_EVENT_TABLE()

MaterialEditorFrame::MaterialEditorFrame(wxWindow* parent) :
    wxFrame(parent, - 1, wxT("Ogre Material Editor"), wxDefaultPosition, wxSize(800, 600), wxDEFAULT_FRAME_STYLE),
    mMenuBar(0),
    mFileMenu(0),
    mEditMenu(0),
    mWindowMenu(0),
    mHelpMenu(0),
    mAuiNotebook(0),
    mInformationNotebook(0),
    mFileTree(0),
    mPropertiesPanel(0),
    mLogPanel(0),
    mDocPanel(0),
    mOgreControl(0),
    mForward(0),
    mBackward(0),
    mRight(0),
    mLeft(0)
{
    createAuiManager();
    createMenuBar();

    CreateToolBar();
    CreateStatusBar();

    /*
    ** We have to create the OgrePanel first
    ** since some of the other panels rely on Ogre.
    */
    createAuiNotebookPane();
    createOgrePane();
    createInformationPane();
    createManagementPane();
    createPropertiesPane();

    mAuiManager.Update();
    /*     if (wxFileExists(wxT("aui.cfg")))
     *     {
     *         wxFileInputStream stream(wxT("aui.cfg"));
     *         if (stream.Ok())
     *         {
     *             int cnt = stream.GetLength()/sizeof(wxChar);
     *             wxChar * tmp = new wxChar[cnt+1];
     *             stream.Read(tmp, stream.GetLength());
     *             tmp[cnt] = wxChar(0);
     *             wxString perspective(tmp);
     *             delete [] tmp;
     *             mAuiManager.LoadPerspective(perspective);
     *         }
     *     }
     */

    mRenderTimer = new wxTimer(this, ID_RENDER_TIMER);
    mRenderTimer->Start(20);
}

MaterialEditorFrame::~MaterialEditorFrame()
{
    mLogPanel->detachLog(Ogre::LogManager::getSingleton().getDefaultLog());

    mAuiManager.UnInit();

    mRenderTimer->Stop();
    delete mRenderTimer;
}

void MaterialEditorFrame::OnRenderTimer(wxTimerEvent& event)
{
    mOgreControl->TranslateCamera(mRight - mLeft, 0, mBackward - mForward);
    //mOgreControl->TranslateCamera(1, 0, 0);
    mOgreControl->Update();
}

void MaterialEditorFrame::createAuiManager()
{
    mAuiManager.SetFlags(wxAUI_MGR_DEFAULT | wxAUI_MGR_ALLOW_ACTIVE_PANE | wxAUI_MGR_TRANSPARENT_DRAG);
    mAuiManager.SetManagedWindow(this);

    wxAuiDockArt* art = mAuiManager.GetArtProvider();
    art->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 1);
    art->SetMetric(wxAUI_DOCKART_SASH_SIZE, 4);
    art->SetMetric(wxAUI_DOCKART_CAPTION_SIZE, 17);
    art->SetColour(wxAUI_DOCKART_ACTIVE_CAPTION_COLOUR, wxColour(49, 106, 197));
    art->SetColour(wxAUI_DOCKART_ACTIVE_CAPTION_GRADIENT_COLOUR, wxColour(90, 135, 208));
    art->SetColour(wxAUI_DOCKART_ACTIVE_CAPTION_TEXT_COLOUR, wxColour(255, 255, 255));
    art->SetColour(wxAUI_DOCKART_INACTIVE_CAPTION_COLOUR, wxColour(200, 198, 183));
    art->SetColour(wxAUI_DOCKART_INACTIVE_CAPTION_GRADIENT_COLOUR, wxColour(228, 226, 209));
    art->SetColour(wxAUI_DOCKART_INACTIVE_CAPTION_TEXT_COLOUR, wxColour(0, 0, 0));

    mAuiManager.Update();
}

void MaterialEditorFrame::createAuiNotebookPane()
{
    mAuiNotebook = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_DEFAULT_STYLE | wxNO_BORDER);

    // Create EditorManager singleton
    new EditorManager(mAuiNotebook);
    wxAuiPaneInfo info;
    info.Caption(wxT("Scripts"));
    info.MaximizeButton(true);
    info.Bottom();
    info.CloseButton(false);

    mAuiManager.AddPane(mAuiNotebook, info);
}

void MaterialEditorFrame::GetTuPassTecMatNames(const wxTreeItemId aSelectedItemId,
        Ogre::String &aTuName,
        Ogre::String &aPassName,
        Ogre::String &aTecName,
        Ogre::String &aMatName) const
{
    wxTreeItemId curr = aSelectedItemId;
    const int image = mScriptTree->GetItemImage(curr);
    switch (image)
    {
    case TEXTURE_UNIT_IMAGE:
        aTuName = Ogre::String(mScriptTree->GetItemText(curr).mb_str());
        curr = mScriptTree->GetItemParent(curr);
    case PASS_IMAGE:
        aPassName = Ogre::String(mScriptTree->GetItemText(curr).mb_str());
        curr = mScriptTree->GetItemParent(curr);
    case TECHNIQUE_IMAGE:
        aTecName = Ogre::String(mScriptTree->GetItemText(curr).mb_str());
        curr = mScriptTree->GetItemParent(curr);
    case MATERIAL_IMAGE:
        aMatName = Ogre::String(mScriptTree->GetItemText(curr).mb_str());
        break;
    default:
        assert(0 && "GetTuPassTecMatNames can handle only materials!");
    }
}

void MaterialEditorFrame::OnResourceSelected(wxTreeEvent& event)
{
    if (mScriptTree->GetRootItem() == event.GetItem())
        return;

    const int image = mScriptTree->GetItemImage(event.GetItem());
    switch (image)
    {
    case MATERIAL_IMAGE:
    case TECHNIQUE_IMAGE:
    case PASS_IMAGE:
    case TEXTURE_UNIT_IMAGE:
    {
        Ogre::String tuName;
        Ogre::String passName;
        Ogre::String tecName;
        Ogre::String matName;

        GetTuPassTecMatNames(event.GetItem(), tuName, passName, tecName, matName);

        Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(matName);
        Ogre::Technique* tec = NULL;
        Ogre::Pass* pass = NULL;
        Ogre::TextureUnitState* tu = NULL;

        switch (image)
        {
        case MATERIAL_IMAGE:
            mPropertiesPanel->MaterialSelected(mat);
            break;
        case TECHNIQUE_IMAGE:
            tec = mat->getTechnique(tecName);
            mPropertiesPanel->TechniqueSelected(tec);
            break;
        case PASS_IMAGE:
            tec = mat->getTechnique(tecName);
            pass = tec->getPass(passName);
            mPropertiesPanel->PassSelected(pass);
            break;
        case TEXTURE_UNIT_IMAGE:
            tec = mat->getTechnique(tecName);
            pass = tec->getPass(passName);
            tu = pass->getTextureUnitState(tuName);
            //mPropertiesPanel->TextureUnitStateSelected(tu);
            break;
        }

        UpdateDisplay("", matName);
        break;
    }
    case MESH_IMAGE:
    {
        Ogre::String meshName(mScriptTree->GetItemText(event.GetItem()).mb_str());
        UpdateDisplay(meshName, "");
        break;
    }
    }

}

void MaterialEditorFrame::UpdateDisplay(Ogre::String aMeshName, Ogre::String aMaterialName)
{
    // Update display
    Ogre::SceneNode* node = m_sm->getSceneNode("DisplayNode");
    Ogre::Entity* ent = m_sm->getEntity("Display");
    node->detachObject(ent);
    m_sm->destroyEntity(ent);

    if (!aMeshName.empty())
    {
        ent = m_sm->createEntity(DISPLAY_NAME, aMeshName);
    }
    else
    {
        ent = m_sm->createEntity(DISPLAY_NAME, Ogre::SceneManager::PT_CUBE);
    }

    if (!aMaterialName.empty())
    {
        Ogre::MaterialPtr mat = Ogre::MaterialManager::getSingleton().getByName(aMaterialName);
        ent->setMaterial(mat);
    }

    node->attachObject(ent);
}

const MaterialMap* MaterialEditorFrame::GetMaterialMap(const wxTreeItemId& aId) const
{
    Ogre::String file(mFileTree->GetItemText(aId).mb_str());

    const wxTreeItemId archiveId = mFileTree->GetItemParent(aId);
    Ogre::String archive(mFileTree->GetItemText(archiveId).mb_str());

    const wxTreeItemId groupId = mFileTree->GetItemParent(archiveId);
    Ogre::String group(mFileTree->GetItemText(groupId).mb_str());

    return &mGroupMap.at(group).at(archive).at(file);
}


void MaterialEditorFrame::OnFileSelected(wxTreeEvent& event)
{
    mScriptTree->DeleteAllItems();
    wxString selectedNodeName = mFileTree->GetItemText(event.GetItem());
    wxTreeItemId root = mScriptTree->AddRoot(selectedNodeName);

    switch (mFileTree->GetItemImage(event.GetItem()))
    {
    case MATERIAL_SCRIPT_IMAGE:
    {
        const MaterialMap* materials = GetMaterialMap(event.GetItem());
        for (MaterialMap::const_iterator it = materials->begin(); it != materials->end(); ++it)
        {
            Ogre::MaterialPtr material = it->second;
            const wxTreeItemId materialId = mScriptTree->AppendItem(root, wxString(material->getName().c_str(), wxConvUTF8), MATERIAL_IMAGE);
            if (it == materials->begin())
            {
                mScriptTree->SelectItem(materialId, true);
            }

            Ogre::Material::TechniqueIterator matIt = material->getTechniqueIterator();
            while (matIt.hasMoreElements())
            {
                Ogre::Technique* techique = matIt.getNext();
                const wxTreeItemId techiqueId = mScriptTree->AppendItem(materialId, wxString(techique->getName().c_str(), wxConvUTF8), TECHNIQUE_IMAGE);
                Ogre::Technique::PassIterator passIt = techique->getPassIterator();
                while (passIt.hasMoreElements())
                {
                    Ogre::Pass* pass = passIt.getNext();
                    const wxTreeItemId passId = mScriptTree->AppendItem(techiqueId, wxString(pass->getName().c_str(), wxConvUTF8), PASS_IMAGE);
                    Ogre::Pass::TextureUnitStateIterator texIt = pass->getTextureUnitStateIterator();
                    while (texIt.hasMoreElements())
                    {
                        Ogre::TextureUnitState* tu = texIt.getNext();
                        const wxTreeItemId texId = mScriptTree->AppendItem(passId, wxString(tu->getName().c_str(), wxConvUTF8), TEXTURE_UNIT_IMAGE);
                    }
                }
            }
        }
        break;
    }
    case MESH_FILE_IMAGE:
    {
        const wxTreeItemId meshId = mScriptTree->AppendItem(root, selectedNodeName, MESH_IMAGE);
        mScriptTree->SelectItem(meshId, true);
        break;
    }
    }
}


wxImageList* CreateImageList()
{
    wxImageList* mImageList = new wxImageList(16, 16, true, 17);
    mImageList->Add(IconManager::getSingleton().getIcon(IconManager::WORKSPACE));// WORKSPACE_IMAGE = 0;
    mImageList->Add(IconManager::getSingleton().getIcon(IconManager::PROJECT));// GROUP
    // Archive types
    mImageList->Add(IconManager::getSingleton().getIcon(IconManager::UNKNOWN));// UNKNOW_ARCHIVE_IMAGE = 1;
    mImageList->Add(IconManager::getSingleton().getIcon(IconManager::FILE_SYSTEM));// FILESYTEM_IMAGE = 2;
    mImageList->Add(IconManager::getSingleton().getIcon(IconManager::ZIP));// ZIP_IMAGE = 3;
    // Resource types
    mImageList->Add(IconManager::getSingleton().getIcon(IconManager::UNKNOWN));// UNKNOW_RESOURCE_IMAGE = 4;
    mImageList->Add(IconManager::getSingleton().getIcon(IconManager::PROJECT));// COMPOSITE_IMAGE = 5;
    mImageList->Add(IconManager::getSingleton().getIcon(IconManager::MATERIAL_SCRIPT));// MATERIAL_IMAGE = 6;
    mImageList->Add(IconManager::getSingleton().getIcon(IconManager::MESH));// MESH_IMAGE = 7;
    mImageList->Add(IconManager::getSingleton().getIcon(IconManager::PROGRAM_SCRIPT));// ASM_PROGRAMM_IMAGE = 8;
    mImageList->Add(IconManager::getSingleton().getIcon(IconManager::PROGRAM_SCRIPT));// HL_PROGRAMM_IMAGE = 9;
    mImageList->Add(IconManager::getSingleton().getIcon(IconManager::TEXTURE));// TEXTURE_IMAGE = 10;
    mImageList->Add(IconManager::getSingleton().getIcon(IconManager::TECHNIQUE));// SKELETON_IMAGE = 11;
    mImageList->Add(IconManager::getSingleton().getIcon(IconManager::FONT));// FONT_IMAGE = 12;

    mImageList->Add(IconManager::getSingleton().getIcon(IconManager::MATERIAL));
    mImageList->Add(IconManager::getSingleton().getIcon(IconManager::TECHNIQUE));
    mImageList->Add(IconManager::getSingleton().getIcon(IconManager::PASS));
    mImageList->Add(IconManager::getSingleton().getIcon(IconManager::TEXTURE));
    mImageList->Add(IconManager::getSingleton().getIcon(IconManager::MESH));
    return mImageList;
}

void MaterialEditorFrame::createManagementPane()
{
    {
        mScriptTree = new wxTreeCtrl(this, ID_RESOURCE_TREE, wxDefaultPosition, wxDefaultSize,
                                     wxNO_BORDER | wxTR_EDIT_LABELS | wxTR_FULL_ROW_HIGHLIGHT |
                                     wxTR_HAS_BUTTONS | wxTR_SINGLE);
        mScriptTree->AssignImageList(CreateImageList());

        wxAuiPaneInfo info;
        info.Caption(wxT("Script browser"));
        info.MaximizeButton(true);
        info.CloseButton(false);
        info.BestSize(256, 512);
        info.Left();
        info.Layer(1);

        mAuiManager.AddPane(mScriptTree, info);
    }

    {

        mFileTree = new wxTreeCtrl(this, ID_FILE_TREE, wxDefaultPosition, wxDefaultSize,
                                   wxNO_BORDER | wxTR_EDIT_LABELS | wxTR_FULL_ROW_HIGHLIGHT | wxTR_HAS_BUTTONS | wxTR_SINGLE);

        mFileTree->AssignImageList(CreateImageList());

        wxAuiPaneInfo info;
        info.Caption(wxT("Resource browser"));
        info.MaximizeButton(true);
        info.CloseButton(false);
        info.BestSize(256, 512);
        info.Left();
        info.Layer(1);

        mAuiManager.AddPane(mFileTree, info);
    }


}

void MaterialEditorFrame::createInformationPane()
{
    mInformationNotebook = new wxAuiNotebook(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP | wxAUI_NB_TAB_SPLIT | wxAUI_NB_TAB_MOVE | wxAUI_NB_SCROLL_BUTTONS | wxNO_BORDER);

    mLogPanel = new LogPanel(mInformationNotebook);
    mInformationNotebook->AddPage(mLogPanel, wxT("Log"));
    mLogPanel->attachLog(Ogre::LogManager::getSingleton().getDefaultLog());

    mDocPanel = new DocPanel(mInformationNotebook);
    mInformationNotebook->AddPage(mDocPanel, wxT("Documentation"));

    wxAuiPaneInfo info;
    info.Caption(wxT("Information"));
    info.MaximizeButton(true);
    info.CloseButton(false);
    info.BestSize(256, 128);
    info.Bottom();

    mAuiManager.AddPane(mInformationNotebook, info);
}

void MaterialEditorFrame::createPropertiesPane()
{
    mPropertiesPanel = new PropertiesPanel(this);

    wxAuiPaneInfo info;
    info.Caption(wxT("Properties"));
    info.MaximizeButton(true);
    info.CloseButton(false);
    info.BestSize(256, 512);
    info.Right();
    info.Layer(1);

    mAuiManager.AddPane(mPropertiesPanel, info);
}

void MaterialEditorFrame::CreateScene()
{
    m_sm = mOgreControl->CreateSceneManager(Ogre::ST_GENERIC);

    Ogre::Entity* ent = m_sm->createEntity(DISPLAY_NAME, Ogre::SceneManager::PT_CUBE);
    Ogre::SceneNode* no = m_sm->getRootSceneNode()->createChildSceneNode("DisplayNode");

    no->setPosition(0, 0, -200);
    no->attachObject(ent);

    struct stat stFileInfo;
    const char* resources = "resources.cfg";
    if (!stat(resources, &stFileInfo))
    {
        Workspace::OpenConfigFile(resources);
        FillResourceTree();
    }
}

void MaterialEditorFrame::createOgrePane()
{
    mOgreControl = new wxOgreControl(this, wxID_ANY, wxDefaultPosition, this->GetClientSize());

    wxAuiPaneInfo info;
    info.Caption(wxT("Render"));
    info.MaximizeButton(true);
    info.Center();
    info.CloseButton(false);

    mAuiManager.AddPane(mOgreControl, info);
}

void MaterialEditorFrame::createMenuBar()
{
    mMenuBar = new wxMenuBar();

    createFileMenu();
    createEditMenu();
    createWindowMenu();
    createHelpMenu();

    SetMenuBar(mMenuBar);
}

void MaterialEditorFrame::createFileMenu()
{
    mFileMenu = new wxMenu();
    wxMenuItem* menuItem = NULL;

    // New sub menu
    menuItem = new wxMenuItem(mFileMenu, ID_FILE_MENU_OPEN, wxT("&Open"));
    mFileMenu->Append(menuItem);

    menuItem = new wxMenuItem(mFileMenu, ID_FILE_MENU_SAVE, wxT("&Save"));
    menuItem->SetBitmap(IconManager::getSingleton().getIcon(IconManager::SAVE));
    mFileMenu->Append(menuItem);

    menuItem = new wxMenuItem(mFileMenu, ID_FILE_MENU_SAVE_AS, wxT("Save &As..."));
    menuItem->SetBitmap(IconManager::getSingleton().getIcon(IconManager::SAVE_AS));
    mFileMenu->Append(menuItem);

    mFileMenu->AppendSeparator();

    menuItem = new wxMenuItem(mFileMenu, ID_FILE_MENU_EXIT, wxT("E&xit"));
    mFileMenu->Append(menuItem);
    mFileMenu->UpdateUI();

    mMenuBar->Append(mFileMenu, wxT("&File"));
}

void MaterialEditorFrame::createEditMenu()
{
    mEditMenu = new wxMenu();
    mEditMenu->Append(ID_EDIT_MENU_UNDO, wxT("Undo"));
    mEditMenu->Append(ID_EDIT_MENU_REDO, wxT("Redo"));
    mEditMenu->AppendSeparator();
    mEditMenu->Append(ID_EDIT_MENU_CUT, wxT("Cut"));
    mEditMenu->Append(ID_EDIT_MENU_COPY, wxT("Copy"));
    mEditMenu->Append(ID_EDIT_MENU_PASTE, wxT("Paste"));

    mMenuBar->Append(mEditMenu, wxT("&Edit"));
}

void MaterialEditorFrame::createWindowMenu()
{
    mWindowMenu = new wxMenu();
    mMenuBar->Append(mWindowMenu, wxT("&Window"));
}

void MaterialEditorFrame::createHelpMenu()
{
    mHelpMenu = new wxMenu();
    mMenuBar->Append(mHelpMenu, wxT("&Help"));
}

void MaterialEditorFrame::FillResourceTree()
{
    mFileTree->DeleteAllItems();
    wxTreeItemId mRootId = mFileTree->AddRoot(wxString(Workspace::GetFileName().c_str(), wxConvUTF8), WORKSPACE_IMAGE);
    Ogre::ResourceGroupManager& rgm = Ogre::ResourceGroupManager::getSingleton();
    Ogre::StringVector groups = rgm.getResourceGroups();
    for (Ogre::StringVector::iterator groupIt = groups.begin(); groupIt != groups.end(); ++groupIt)
    {
        wxTreeItemId groupId = mFileTree->AppendItem(mRootId, wxString(groupIt->c_str(), wxConvUTF8), GROUP_IMAGE);
        mFileTree->SelectItem(groupId, true); //This is some kind of hack for windows
        Ogre::FileInfoListPtr fileInfoList = rgm.listResourceFileInfo(*groupIt, false);

        // Collect archives. In Ogre 1.7, this will be removed, and archives queried directly
        std::set< Ogre::Archive* > archives;
        for (Ogre::FileInfoList::iterator fileIt = fileInfoList->begin(); fileIt != fileInfoList->end(); ++fileIt)
        {
            archives.insert(fileIt->archive);
        }

        for (std::set< Ogre::Archive* >::iterator archiveIt = archives.begin(); archiveIt != archives.end(); ++archiveIt)
        {
            Ogre::Archive* archive = *archiveIt;
            wxTreeItemId archiveId = mFileTree->AppendItem(groupId, wxString(archive->getName().c_str(), wxConvUTF8), FILE_SYSTEM_IMAGE);

            Ogre::StringVectorPtr fileList;
            // Materials
            fileList = archive->find("*.material");
            for (Ogre::StringVector::iterator fileNameIt = fileList->begin(); fileNameIt != fileList->end(); ++fileNameIt)
            {
                wxTreeItemId id = mFileTree->AppendItem(archiveId, wxString(fileNameIt->c_str(), wxConvUTF8), MATERIAL_SCRIPT_IMAGE);
            }

            // Meshes
            fileList = archive->find("*.mesh");
            for (Ogre::StringVector::iterator fileNameIt = fileList->begin(); fileNameIt != fileList->end(); ++fileNameIt)
            {
                mFileTree->AppendItem(archiveId, wxString(fileNameIt->c_str(), wxConvUTF8), MESH_FILE_IMAGE);
            }

            // Programs
            fileList = archive->find("*.program");
            for (Ogre::StringVector::iterator fileNameIt = fileList->begin(); fileNameIt != fileList->end(); ++fileNameIt)
            {
                mFileTree->AppendItem(archiveId, wxString(fileNameIt->c_str(), wxConvUTF8), HL_PROGRAMM_IMAGE);
            }
        }
    }

    // Now get materials
    Ogre::ResourceManager::ResourceMapIterator it = Ogre::MaterialManager::getSingleton().getResourceIterator();
    while (it.hasMoreElements())
    {
        Ogre::MaterialPtr material = it.getNext();
        const Ogre::String& origin = material->getOrigin();
        if (!origin.empty())
        {
            const Ogre::String& group = material->getGroup();
            const size_t pos = origin.rfind(":");
            Ogre::String archive = origin.substr(0, pos);

            Ogre::String file = origin.substr(pos + 1, origin.size() - pos);
            GetLog() << group + ":" + archive + ":" + file + ":" + material->getName();
            mGroupMap[group][archive][file][material->getName()] = material;
        }
    }

}

void MaterialEditorFrame::OnFileOpen(wxCommandEvent& event)
{
    wxFileDialog * openDialog = new wxFileDialog(this, wxT("Choose a file to open"), wxEmptyString, wxEmptyString,
            wxT("Resource configuration (*.cfg)|*.cfg|All Files (*.*)|*.*"));

    if(openDialog->ShowModal() == wxID_OK)
    {
        wxString path = openDialog->GetPath();
        Workspace::OpenConfigFile(Ogre::String(path.mb_str()));
        FillResourceTree();
    }
}

void MaterialEditorFrame::OnFileSave(wxCommandEvent& event)
{
    EditorBase* editor = EditorManager::getSingletonPtr()->getActiveEditor();
    if(editor != NULL) editor->save();

    // TODO: Support project & workspace save
}

void MaterialEditorFrame::OnFileSaveAs(wxCommandEvent& event)
{
    EditorBase* editor = EditorManager::getSingletonPtr()->getActiveEditor();
    if(editor != NULL) editor->saveAs();

    // TODO: Support project & workspace saveAs
}

void MaterialEditorFrame::OnFileExit(wxCommandEvent& event)
{
    wxString perspective = mAuiManager.SavePerspective();
    wxFileOutputStream stream(wxT("aui.cfg"));
    wxStringInputStream stringStream(perspective);
    if(stream.Ok())
    {
        stream.Write(stringStream);
    }
    stream.Close();
    Close();
}

void MaterialEditorFrame::OnEditUndo(wxCommandEvent& event)
{
    EditorBase* editor = EditorManager::getSingletonPtr()->getActiveEditor();
    if(editor != NULL) editor->undo();
}

void MaterialEditorFrame::OnEditRedo(wxCommandEvent& event)
{
    EditorBase* editor = EditorManager::getSingletonPtr()->getActiveEditor();
    if(editor != NULL) editor->redo();
}

void MaterialEditorFrame::OnEditCut(wxCommandEvent& event)
{
    EditorBase* editor = EditorManager::getSingletonPtr()->getActiveEditor();
    if(editor != NULL) editor->cut();
}

void MaterialEditorFrame::OnEditCopy(wxCommandEvent& event)
{
    EditorBase* editor = EditorManager::getSingletonPtr()->getActiveEditor();
    if(editor != NULL) editor->copy();
}

void MaterialEditorFrame::OnEditPaste(wxCommandEvent& event)
{
    EditorBase* editor = EditorManager::getSingletonPtr()->getActiveEditor();
    if(editor != NULL) editor->paste();
}

void MaterialEditorFrame::OnKey( wxKeyEvent& event )
{
    Ogre::Real dir = event.GetEventType() == wxEVT_KEY_DOWN ? 100 : 0;
    switch (event.GetKeyCode())
    {
    case 'W':
        mForward = dir;
        break;
    case 'S':
        mBackward = dir;
        break;
    case 'A':
        mLeft = dir;
        break;
    case 'D':
        mRight = dir;
        break;
    }
    event.Skip();
}
