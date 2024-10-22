/*
 * Copyright (C) 2007-2008 Martin Pieuchot
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 */

#include "wx/ogre/prerequisites.h"
#include "wx/ogre/rendersystem.h"
#include "wx/ogre/utils.h"

#ifdef OGRE_STATIC_LIB
#include "OgreGLPlugin.h"
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_LINUX
 #ifndef OGRE_PLUGINDIR
 #define OGRE_PLUGINDIR "/usr/local/lib/OGRE"
 #endif
#else
 #define OGRE_PLUGINDIR ""
#endif

IMPLEMENT_OGRE_SINGLETON(wxOgreRenderSystem)

//------------------------------------------------------------------------------
wxOgreRenderSystem::wxOgreRenderSystem()
{
#ifndef WXOGRE_DEBUG
	m_root = new Ogre::Root("", "", "");
#else
	m_root = new Ogre::Root("", "", "wxOgreDebug.log");
#endif
}
//------------------------------------------------------------------------------
wxOgreRenderSystem::wxOgreRenderSystem(const Ogre::String& plugins,
                           const Ogre::String& config, const Ogre::String& log)
{
	// Create a new Ogre ROOT
    // Root("plugins.cfg","config.cfg","ogre.log");
	m_root = new Ogre::Root(plugins, config, log);
}
//------------------------------------------------------------------------------
wxOgreRenderSystem::~wxOgreRenderSystem()
{
    delete m_root;
    m_root = 0;
}
//------------------------------------------------------------------------------
void wxOgreRenderSystem::LoadPlugin(const Ogre::String& plugin)
{
    try {
#ifdef OGRE_STATIC_LIB
	// Gl renedr system
    Ogre::Plugin* mGLPlugin = new Ogre::GLPlugin();
    m_root->installPlugin(mGLPlugin);
#else
#if WXOGRE_DEBUG and OGRE_PLATFORM == OGRE_PLATFORM_WINDOWS
		m_root->loadPlugin(OGRE_PLUGINDIR + plugin + "_d");
#else
		m_root->loadPlugin(OGRE_PLUGINDIR + plugin);
#endif
#endif
	} catch (Ogre::Exception& e) {
        wxOgreExceptionBox(e);
    }
}
//------------------------------------------------------------------------------
void wxOgreRenderSystem::SelectOgreRenderSystem(const Ogre::String& render)
{
    const Ogre::RenderSystemList& renderList = m_root->getAvailableRenderers();
    // check through all available renderers, if there is one the
    // string is "render"
    for (Ogre::RenderSystemList::const_iterator it = renderList.begin(); it != renderList.end(); ++it) {
        Ogre::RenderSystem* renderSys = *it;
        if (std::string (renderSys->getName()) == render) {
            m_root->setRenderSystem(renderSys);
            break;
        }
    }

}
//------------------------------------------------------------------------------
void wxOgreRenderSystem::Initialise()
{
    m_root->initialise(false);
}
//------------------------------------------------------------------------------
bool wxOgreRenderSystem::HasCapability(const Ogre::Capabilities& c)
{
    return m_root->getRenderSystem()->getCapabilities()->hasCapability(c);
}
//------------------------------------------------------------------------------
