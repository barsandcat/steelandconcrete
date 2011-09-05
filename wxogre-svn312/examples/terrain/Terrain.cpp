
#include "wx/ogre/ogre.h"

Ogre::RaySceneQuery* p_ray = 0;

class TerrainControl : public wxOgreControl
{
public:

    TerrainControl(wxWindow* parent, const wxSize& size)
        : wxOgreControl(parent, wxID_ANY, wxDefaultPosition, size) {}

    virtual void Update()
    {
        /* Clamp to terrain */
        static Ogre::Ray updateRay;

        updateRay.setOrigin(m_cam->getPosition());
        updateRay.setDirection(Ogre::Vector3::NEGATIVE_UNIT_Y);

        p_ray->setRay(updateRay);

        Ogre::RaySceneQueryResult& qryResult = p_ray->execute();
        Ogre::RaySceneQueryResult::iterator i = qryResult.begin();

        if (i != qryResult.end() && i->worldFragment) {
            Ogre::Vector3 pos = m_cam->getPosition();

            m_cam->setPosition(pos.x,
                               i->worldFragment->singleIntersection.y + 10, 
                               pos.z);
        }

        wxOgreControl::Update();
    }
};


class TerrainApp : public wxOgreApp
{
public:

    bool OnInit()
    {
        /* Initialize Ogre render system */
        m_rsys->LoadPlugin("RenderSystem_GL");
        m_rsys->LoadPlugin("Plugin_OctreeSceneManager");
        m_rsys->LoadPlugin("Plugin_ParticleFX");
        m_rsys->SelectOgreRenderSystem("OpenGL Rendering Subsystem");
        m_rsys->Initialise();

        m_frame = new wxFrame(0, wxID_ANY, _("wxTerrain"), wxDefaultPosition,
                              wxSize(600,400));

        m_control = new TerrainControl(m_frame, m_frame->GetClientSize());

        m_frame->Show();

        /* 
         * Resources must be initialised when, at least,
         * one window has been created.
         */
        m_res->LoadResourceFile("resources.cfg");
        m_res->InitialiseAllResources();

        CreateScene();

        return true;
    }

private:

    void CreateScene()
    {
        m_sm = m_control->CreateSceneManager("TerrainSceneManager");

        Ogre::Camera* cam = m_control->GetCamera();
 
        Ogre::Plane waterPlane;

        /* Set ambient light */
        m_sm->setAmbientLight(Ogre::ColourValue(0.5, 0.5, 0.5));

        /* Create a light */
        Ogre::Light* l = m_sm->createLight("MainLight");

        /*
         * Accept default settings: point light, white diffuse,
         * just set position.
         * NB I could attach the light to a SceneNode if I wanted
         * it to move automatically with other objects, but I don't
         */
        l->setPosition(20, 80, 50);

        /* Fog
         * NB it's VERY important to set this before calling
         * setWorldGeometry because the vertex program picked
         * will be different
         */
        Ogre::ColourValue fadeColour(0.93, 0.86, 0.76);
        m_sm->setFog(Ogre::FOG_LINEAR, fadeColour, .001, 500, 1000);
        m_control->GetViewport()->setBackgroundColour(fadeColour);

        m_sm->setWorldGeometry("terrain.cfg");

        /* Infinite far plane? */
        if (m_rsys->HasCapability(Ogre::RSC_INFINITE_FAR_PLANE))
            cam->setFarClipDistance(0);

        
        Ogre::Plane plane;  // Define the required skyplane
        plane.d = 5000;     // 5000 world units from the camera
        plane.normal = - Ogre::Vector3::UNIT_Y;// Above the camera, facing down

        /* Set a nice viewpoint */
        cam->setPosition(707, 2500, 528);
        cam->setOrientation(Ogre::Quaternion(-0.3486, 0.0122, 0.9365, 0.0329));
        cam->setNearClipDistance(5);


        Ogre::Vector3 pos = cam->getPosition();
        p_ray = m_sm->createRayQuery(Ogre::Ray(pos, Ogre::Vector3::NEGATIVE_UNIT_Y));

        m_control->Refresh();
    }

    Ogre::SceneManager*  m_sm;

    wxFrame*         m_frame;
    TerrainControl*  m_control;
};

IMPLEMENT_APP(TerrainApp)
