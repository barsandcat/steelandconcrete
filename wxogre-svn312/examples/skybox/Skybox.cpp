
#include "wx/ogre/ogre.h"

class Skybox : public wxOgreApp
{
public:

    bool OnInit()
    {
        /* Initialize Ogre render system */
        m_rsys->LoadPlugin("RenderSystem_GL");
        m_rsys->LoadPlugin("Plugin_ParticleFX");
        m_rsys->SelectOgreRenderSystem("OpenGL Rendering Subsystem");
        m_rsys->Initialise();

        m_frame = new wxFrame(0, wxID_ANY, _("wxSkybox"), wxDefaultPosition, wxSize(600,400));
        m_control = new wxOgreControl(m_frame, wxID_ANY, wxDefaultPosition, m_frame->GetClientSize());
        m_frame->Show();
        CreateScene();

        return true;
    }

private:

    void CreateScene()
    {
        Ogre::SceneManager* sm;
        Ogre::Camera* cam;

        /* 
         * Resources must be initialised when, at least,
         * one window has been created.
         */
        m_res->LoadResourceFile("resources.cfg");
        m_res->InitialiseAllResources();

        /* Create a SceneManager */
        sm = m_control->CreateSceneManager(Ogre::ST_GENERIC);

        /* Get the Camera */
        cam = m_control->GetCamera();

        /* Position it at 500 in Z direction */
        cam->setPosition(Ogre::Vector3(0, 0, 500));
        /* Look back along -Z */
        cam->lookAt(Ogre::Vector3(0, 0, -300));
        cam->setNearClipDistance(5);

        /*
         * Now it's the same code as the Ogre's Samples/SkyBox demo.
         */

        // Set ambient light
        sm->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));

        /* Create a skybox */
        sm->setSkyBox(true, "Examples/SpaceSkyBox", 50);

        /* Set ambient light */
        sm->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));

        /* Create a light */
        Ogre::Light* l = sm->createLight("MainLight");

        /*
         * Accept default settings: point light, white diffuse,
         * just set position.
         * NB I could attach the light to a SceneNode if I wanted
         * it to move automatically with other objects, but I don't
         */
        l->setPosition(20, 80, 50);

        /* Also add a nice starship in */
        Ogre::Entity* ent = sm->createEntity("razor", "razor.mesh");

        sm->getRootSceneNode()->attachObject(ent);

        mTrhusters = sm->createParticleSystem("ParticleSys1", 200);

        mTrhusters->setMaterialName("Examples/Flare");
        mTrhusters->setDefaultDimensions(25, 25);

        Ogre::ParticleEmitter *pEmit1 = mTrhusters->addEmitter("Point");
        Ogre::ParticleEmitter *pEmit2 = mTrhusters->addEmitter("Point");

        /* Thruster 1 */
        pEmit1->setAngle(Ogre::Degree(3));
        pEmit1->setTimeToLive(0.2);
        pEmit1->setEmissionRate(70);

        pEmit1->setParticleVelocity(50);

        pEmit1->setDirection(- Ogre::Vector3::UNIT_Z);
        pEmit1->setColour(Ogre::ColourValue::White, Ogre::ColourValue::Red);

        /* Thruster 2 */
        pEmit2->setAngle(Ogre::Degree(3));
        pEmit2->setTimeToLive(0.2);
        pEmit2->setEmissionRate(70);

        pEmit2->setParticleVelocity(50);

        pEmit2->setDirection(- Ogre::Vector3::UNIT_Z);
        pEmit2->setColour(Ogre::ColourValue::White, Ogre::ColourValue::Red);

        /* Set the position of the thrusters */
        pEmit1->setPosition(Ogre::Vector3(5.7f, 0.0f, 0.0f));
        pEmit2->setPosition(Ogre::Vector3(-18.0f, 0.0f, 0.0f));

        sm->getRootSceneNode()->createChildSceneNode(Ogre::Vector3(0.0f, 6.5f, -67.0f))->attachObject(mTrhusters);


        m_control->Refresh();
    }

    Ogre::ParticleSystem *mTrhusters;

    wxFrame* m_frame;
    wxOgreControl* m_control;
};

IMPLEMENT_APP(Skybox)
