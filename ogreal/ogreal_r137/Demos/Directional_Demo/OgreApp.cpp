#include "OgreApp.h"

OgreApp::OgreApp()
{
    root = new Root;

    ConfigFile cf;
    cf.load("resources.cfg");

    // Go through all sections & settings in the file
    ConfigFile::SectionIterator seci = cf.getSectionIterator();

    String secName, typeName, archName;
    while (seci.hasMoreElements())
    {
        secName = seci.peekNextKey();
        ConfigFile::SettingsMultiMap *settings = seci.getNext();
        ConfigFile::SettingsMultiMap::iterator i;
        for (i = settings->begin(); i != settings->end(); ++i)
        {
            typeName = i->first;
            archName = i->second;
            ResourceGroupManager::getSingleton().addResourceLocation(
                archName, typeName, secName);
        }
    }

	ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	
	// Restore the old config
	if(!root->restoreConfig())
	{
		// If there is no existing config, bring up the config dialog
		if(!root->showConfigDialog())
		{
			// If the user cancels the dialog, close the app
			exit(0);
		}
	}

    win = root->initialise(true, "Ogre Framework");

	sceneMgr = root->createSceneManager(ST_GENERIC, "MainSceneManager");

    camera = sceneMgr->createCamera("SimpleCamera");
    viewPort = root->getAutoCreatedWindow()->addViewport(camera);
    viewPort->setBackgroundColour(ColourValue(0,0,0));
    camera->setAspectRatio(Real(viewPort->getActualWidth()) / Real(viewPort->getActualHeight()));
    camera->setNearClipDistance(1.0);

	soundManager = new OgreAL::SoundManager();

	createScene();

    listener = new DeviceListener(win, camera, sceneMgr);
    root->addFrameListener(listener);
}

OgreApp::~OgreApp()
{
    delete listener;
	delete soundManager;
    delete root;
}

void OgreApp::start()
{
    root->startRendering();
}

void OgreApp::createScene()
{
	sceneMgr->setSkyBox(true, "Sky", 5, 8, 4000);
    sceneMgr->setAmbientLight(ColourValue(0.5f, 0.5f, 0.5f));
	//sceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_ADDITIVE);

	node = sceneMgr->getRootSceneNode()->createChildSceneNode("TowerNode");
	ent = sceneMgr->createEntity("Tower", "Tower.mesh");
	node->attachObject(ent);

	node = sceneMgr->getRootSceneNode()->createChildSceneNode("HornNode");
	ent = sceneMgr->createEntity("Horn", "Horn.mesh");
	node->attachObject(ent);
	node->setPosition(0, 23, 0);

	OgreAL::Sound *sound = soundManager->createSound("Siren", "Siren.wav", true);
	sound->setGain(3.0);
	sound->setInnerConeAngle(10);
	sound->setOuterConeAngle(180);
	sound->setOuterConeGain(0.2);
	node->attachObject(sound);

	node = sceneMgr->getRootSceneNode()->createChildSceneNode("CameraNode");
	node->setPosition(0, 6, 100);
	node = node->createChildSceneNode("PitchNode");
	node->attachObject(camera);
	node->attachObject(soundManager->getListener());

	// Create a ground plane
    Plane plane(Vector3::UNIT_Y, 0);
    MeshManager::getSingleton().createPlane("ground",
       ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, plane,
       15000,15000,20,20,true,1,5,5,Vector3::UNIT_Z);
    ent = sceneMgr->createEntity("GroundEntity", "ground");
    sceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(ent);
    ent->setMaterialName("Ground");
    ent->setCastShadows(false);

	light = sceneMgr->createLight("sun");
	light->setType(Light::LT_DIRECTIONAL);
	light->setDirection(-1,-1,-1);

	OverlayManager& overlayMgr = OverlayManager::getSingleton();
	Ogre::OverlayContainer* panel = static_cast<OverlayContainer*>(
	   overlayMgr.createOverlayElement("Panel", "PanelName"));
	panel->setMetricsMode(Ogre::GMM_PIXELS);
	panel->setPosition(10, 10);
	panel->setDimensions(100, 100);

	TextAreaOverlayElement* textArea = static_cast<TextAreaOverlayElement*>(
	   overlayMgr.createOverlayElement("TextArea", "TextAreaName"));
	textArea->setMetricsMode(Ogre::GMM_PIXELS);
	textArea->setPosition(0, 0);
	textArea->setDimensions(100, 100);
	textArea->setCharHeight(16);
	textArea->setFontName("Arial");
	textArea->setCaption("Hello, World!");

	Ogre::Overlay* overlay = overlayMgr.create("AverageFps");
	overlay->add2D(panel);
	panel->addChild(textArea);
	overlay->show();
}
