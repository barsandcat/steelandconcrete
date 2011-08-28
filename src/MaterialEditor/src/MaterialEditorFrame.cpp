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
#include <OgreControl.h>


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

enum ResourceBrowserImages
{
// Image list
WORKSPACE,
RESOURCE_GROUP,
UNKNOWN_ARCHIVE,
FILE_SYSTEM_ARCHIVE,
ZIP_ARCHIVE,
UNKNOWN_RESOURCE,
COMPOSITE_RESOURCE,
MATERIAL_SCRIPT_RESOURCE,
MATERIAL_RESOURCE,
MESH_RESOURCE,
ASM_RESOURCE,
HL_RESOURCE,
TEXTURE_RESOURCE,
SKELETON_RESOURCE,
FONT_RESOURCE,
RESOURCE_BROWSER_IMAGES_COUNT
};

// Materials script
enum ObjectViewImages
{
MATERIAL,
TECHNIQUE,
PASS,
TEXTURE,
MESH,
OBJECT_VIEW_IMAGES_COUNT
};

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
    mResourceTree(0),
    mPropertiesPanel(0),
    mLogPanel(0),
    mDocPanel(0),
    mOgreControl(0)
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
    const int image = mObjectTree->GetItemImage(curr);
    switch (image)
    {
    case TEXTURE:
        aTuName = Ogre::String(mObjectTree->GetItemText(curr).mb_str());
        curr = mObjectTree->GetItemParent(curr);
    case PASS:
        aPassName = Ogre::String(mObjectTree->GetItemText(curr).mb_str());
        curr = mObjectTree->GetItemParent(curr);
    case TECHNIQUE:
        aTecName = Ogre::String(mObjectTree->GetItemText(curr).mb_str());
        curr = mObjectTree->GetItemParent(curr);
    case MATERIAL:
        aMatName = Ogre::String(mObjectTree->GetItemText(curr).mb_str());
        break;
    default:
        assert(0 && "GetTuPassTecMatNames can handle only materials!");
    }
}

void MaterialEditorFrame::OnResourceSelected(wxTreeEvent& event)
{
    if (mObjectTree->GetRootItem() == event.GetItem())
        return;

    const int image = mObjectTree->GetItemImage(event.GetItem());
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
    case MATERIAL:
        mPropertiesPanel->MaterialSelected(mat);
        break;
    case TECHNIQUE:
        tec = mat->getTechnique(tecName);
        mPropertiesPanel->TechniqueSelected(tec);
        break;
    case PASS:
        tec = mat->getTechnique(tecName);
        pass = tec->getPass(passName);
        mPropertiesPanel->PassSelected(pass);
        break;
    case TEXTURE:
        tec = mat->getTechnique(tecName);
        pass = tec->getPass(passName);
        tu = pass->getTextureUnitState(tuName);
        //mPropertiesPanel->TextureUnitStateSelected(tu);
        break;
    }

}

void MaterialEditorFrame::OnFileSelected(wxTreeEvent& event)
{
    // Update display entity
    Ogre::SceneNode* node = m_sm->getSceneNode("DisplayNode");
    if (m_sm->hasEntity(DISPLAY_NAME))
    {
        Ogre::Entity* ent = m_sm->getEntity(DISPLAY_NAME);
        node->detachObject(ent);
        m_sm->destroyEntity(ent);
    }

    Ogre::Entity* newEntity(NULL);
    wxString selectedNodeName = mResourceTree->GetItemText(event.GetItem());

    switch (mResourceTree->GetItemImage(event.GetItem()))
    {
    case MATERIAL_RESOURCE:
    {
        newEntity = m_sm->createEntity(DISPLAY_NAME, Ogre::SceneManager::PT_CUBE);
        Ogre::String matName(selectedNodeName.mb_str());
        newEntity->setMaterialName(matName);
        break;
    }
    case MESH_RESOURCE:
        newEntity = m_sm->createEntity(DISPLAY_NAME, Ogre::String(selectedNodeName.mb_str()));
        break;
    default:
        return;
    }
    node->attachObject(newEntity);

    FillObjectTree(newEntity);
}

void MaterialEditorFrame::FillObjectTree(Ogre::Entity* aEntity)
{
    // Build inspector tree
    mObjectTree->DeleteAllItems();

    Ogre::MeshPtr mesh = aEntity->getMesh();
    wxTreeItemId root = mObjectTree->AddRoot(wxString(mesh->getName().c_str(), wxConvUTF8), MESH);
    for (int i = 0; i < aEntity->getNumSubEntities(); ++i)
    {
        Ogre::SubEntity* subEntity = aEntity->getSubEntity(i);
        Ogre::SubMesh* subMesh = subEntity->getSubMesh();
        Ogre::String matName = subMesh->getMaterialName();
        if (matName.empty())
        {
            matName = subEntity->getMaterialName();
        }

        Ogre::MaterialPtr material = Ogre::MaterialManager::getSingleton().getByName(matName);
        wxString wxMatName(matName.c_str(), wxConvUTF8);
        const wxTreeItemId materialId = mObjectTree->AppendItem(root, wxMatName, MATERIAL);

        Ogre::Material::TechniqueIterator matIt = material->getTechniqueIterator();
        while (matIt.hasMoreElements())
        {
            Ogre::Technique* techique = matIt.getNext();
            wxString techniqueName(techique->getName().c_str(), wxConvUTF8);
            const wxTreeItemId techiqueId = mObjectTree->AppendItem(materialId, techniqueName, TECHNIQUE);
            Ogre::Technique::PassIterator passIt = techique->getPassIterator();
            while (passIt.hasMoreElements())
            {
                Ogre::Pass* pass = passIt.getNext();
                wxString passName(pass->getName().c_str(), wxConvUTF8);
                const wxTreeItemId passId = mObjectTree->AppendItem(techiqueId, passName, PASS);
                Ogre::Pass::TextureUnitStateIterator texIt = pass->getTextureUnitStateIterator();
                while (texIt.hasMoreElements())
                {
                    Ogre::TextureUnitState* tu = texIt.getNext();
                    wxString tuName(tu->getName().c_str(), wxConvUTF8);
                    const wxTreeItemId texId = mObjectTree->AppendItem(passId, tuName, TEXTURE);
                }
            }
        }
    }

    mObjectTree->SelectItem(root, true);
}

wxImageList* CreateResourceBrowserImageList()
{
    wxImageList* mImageList = new wxImageList(16, 16, true, RESOURCE_BROWSER_IMAGES_COUNT);
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
    mImageList->Add(IconManager::getSingleton().getIcon(IconManager::MATERIAL));// MATERIAL_IMAGE = 6;
    mImageList->Add(IconManager::getSingleton().getIcon(IconManager::MESH));// MESH_IMAGE = 7;
    mImageList->Add(IconManager::getSingleton().getIcon(IconManager::PROGRAM_SCRIPT));// ASM_PROGRAMM_IMAGE = 8;
    mImageList->Add(IconManager::getSingleton().getIcon(IconManager::PROGRAM_SCRIPT));// HL_PROGRAMM_IMAGE = 9;
    mImageList->Add(IconManager::getSingleton().getIcon(IconManager::TEXTURE));// TEXTURE_IMAGE = 10;
    mImageList->Add(IconManager::getSingleton().getIcon(IconManager::TECHNIQUE));// SKELETON_IMAGE = 11;
    mImageList->Add(IconManager::getSingleton().getIcon(IconManager::FONT));// FONT_IMAGE = 12;
    return mImageList;
}

wxImageList* CreateObjectViewImageList()
{
    wxImageList* mImageList = new wxImageList(16, 16, true, OBJECT_VIEW_IMAGES_COUNT);
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
        mObjectTree = new wxTreeCtrl(this, ID_RESOURCE_TREE, wxDefaultPosition, wxDefaultSize,
                                     wxNO_BORDER | wxTR_EDIT_LABELS | wxTR_FULL_ROW_HIGHLIGHT |
                                     wxTR_HAS_BUTTONS | wxTR_SINGLE);
        mObjectTree->AssignImageList(CreateObjectViewImageList());

        wxAuiPaneInfo info;
        info.Caption(wxT("Object view"));
        info.MaximizeButton(true);
        info.CloseButton(false);
        info.BestSize(256, 512);
        info.Left();
        info.Layer(1);

        mAuiManager.AddPane(mObjectTree, info);
    }

    {

        mResourceTree = new wxTreeCtrl(this, ID_FILE_TREE, wxDefaultPosition, wxDefaultSize,
                                   wxNO_BORDER | wxTR_EDIT_LABELS | wxTR_FULL_ROW_HIGHLIGHT | wxTR_HAS_BUTTONS | wxTR_SINGLE);

        mResourceTree->AssignImageList(CreateResourceBrowserImageList());

        wxAuiPaneInfo info;
        info.Caption(wxT("Resource browser"));
        info.MaximizeButton(true);
        info.CloseButton(false);
        info.BestSize(256, 512);
        info.Left();
        info.Layer(1);

        mAuiManager.AddPane(mResourceTree, info);
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
    m_sm = mOgreControl->CreateSceneManager();

    Ogre::Entity* ent = m_sm->createEntity(DISPLAY_NAME, Ogre::SceneManager::PT_CUBE);
    Ogre::SceneNode* no = m_sm->getRootSceneNode()->createChildSceneNode("DisplayNode");

    no->setPosition(0, 0, 0);
    no->attachObject(ent);

    struct stat stFileInfo;
    const char* resources = "resources.cfg";
    if (!stat(resources, &stFileInfo))
    {
        Workspace::OpenConfigFile(resources);
        FillMaterialMap();
        FillResourceTree();
    }
}

void MaterialEditorFrame::createOgrePane()
{
    mOgreControl = new OgreControl(this, wxID_ANY, wxDefaultPosition, this->GetClientSize());

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

void MaterialEditorFrame::FillMaterialMap()
{
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
            mMaterialMap[group][archive][file][material->getName()] = material;
        }
    }
}

void MaterialEditorFrame::FillResourceTree()
{
    mResourceTree->DeleteAllItems();
    wxString workspaceName(Workspace::GetFileName().c_str(), wxConvUTF8);
    wxTreeItemId mRootId = mResourceTree->AddRoot(workspaceName, WORKSPACE);
    Ogre::ResourceGroupManager& rgm = Ogre::ResourceGroupManager::getSingleton();
    Ogre::StringVector groups = rgm.getResourceGroups();
    for (Ogre::StringVector::iterator groupIt = groups.begin(); groupIt != groups.end(); ++groupIt)
    {
        wxString groupName(groupIt->c_str(), wxConvUTF8);
        wxTreeItemId groupId = mResourceTree->AppendItem(mRootId, groupName, RESOURCE_GROUP);
        mResourceTree->SelectItem(groupId, true); //This is some kind of hack for windows

        const Ogre::ResourceGroupManager::LocationList& locations = rgm.getResourceLocationList(*groupIt);
        Ogre::ResourceGroupManager::LocationList::const_iterator archiveIt = locations.begin();
        for (; archiveIt != locations.end(); ++archiveIt)
        {
            Ogre::ResourceGroupManager::ResourceLocation* location = *archiveIt;
            Ogre::Archive* archive = location->archive;
            AddArchiveToResourceTree(groupId, *groupIt, archive);
        }
    }
}

void MaterialEditorFrame::AddArchiveToResourceTree(wxTreeItemId aGroupId, Ogre::String aGroupName, Ogre::Archive* aArchive)
{
    wxString archiveName(aArchive->getName().c_str(), wxConvUTF8);
    wxTreeItemId archiveId = mResourceTree->AppendItem(aGroupId, archiveName, FILE_SYSTEM_ARCHIVE);

    Ogre::StringVectorPtr fileList;

    fileList = aArchive->find("*.material");
    for (Ogre::StringVector::iterator fileNameIt = fileList->begin(); fileNameIt != fileList->end(); ++fileNameIt)
    {
        wxString materialScriptName(fileNameIt->c_str(), wxConvUTF8);
        wxTreeItemId materialScriptId = mResourceTree->AppendItem(archiveId, materialScriptName, MATERIAL_SCRIPT_RESOURCE);
        const MaterialMap &materials = mMaterialMap.find(aGroupName)->second.find(aArchive->getName())->second.find(fileNameIt->c_str())->second;
        for (MaterialMap::const_iterator it = materials.begin(); it != materials.end(); ++it)
        {
            mResourceTree->AppendItem(materialScriptId, wxString(it->first.c_str(), wxConvUTF8), MATERIAL_RESOURCE);
        }
    }

    fileList = aArchive->find("*.png");
    for (Ogre::StringVector::iterator fileNameIt = fileList->begin(); fileNameIt != fileList->end(); ++fileNameIt)
    {
        wxString name(fileNameIt->c_str(), wxConvUTF8);
        mResourceTree->AppendItem(archiveId, name, TEXTURE_RESOURCE);
    }

    fileList = aArchive->find("*.jpg");
    for (Ogre::StringVector::iterator fileNameIt = fileList->begin(); fileNameIt != fileList->end(); ++fileNameIt)
    {
        wxString name(fileNameIt->c_str(), wxConvUTF8);
        mResourceTree->AppendItem(archiveId, name, TEXTURE_RESOURCE);
    }

    fileList = aArchive->find("*.bmp");
    for (Ogre::StringVector::iterator fileNameIt = fileList->begin(); fileNameIt != fileList->end(); ++fileNameIt)
    {
        wxString name(fileNameIt->c_str(), wxConvUTF8);
        mResourceTree->AppendItem(archiveId, name, TEXTURE_RESOURCE);
    }

    fileList = aArchive->find("*.mesh");
    for (Ogre::StringVector::iterator fileNameIt = fileList->begin(); fileNameIt != fileList->end(); ++fileNameIt)
    {
        wxString name(fileNameIt->c_str(), wxConvUTF8);
        mResourceTree->AppendItem(archiveId, name, MESH_RESOURCE);
    }

    fileList = aArchive->find("*.skeleton");
    for (Ogre::StringVector::iterator fileNameIt = fileList->begin(); fileNameIt != fileList->end(); ++fileNameIt)
    {
        wxString name(fileNameIt->c_str(), wxConvUTF8);
        mResourceTree->AppendItem(archiveId, name, SKELETON_RESOURCE);
    }

    fileList = aArchive->find("*.program");
    for (Ogre::StringVector::iterator fileNameIt = fileList->begin(); fileNameIt != fileList->end(); ++fileNameIt)
    {
        wxString name(fileNameIt->c_str(), wxConvUTF8);
        mResourceTree->AppendItem(archiveId, name, HL_RESOURCE);
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
        FillMaterialMap();
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
