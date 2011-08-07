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

#ifndef _WX_OGRE_PREREQUISITES_H_
#define _WX_OGRE_PREREQUISITES_H_

#include <wx/wx.h>
#include <Ogre.h>

#define S(s) STRINGIFY(s)
#define STRINGIFY(s) #s

#if defined( _DEBUG ) || defined( DEBUG )
#   define WXOGRE_DEBUG 1
#else
#   define WXOGRE_DEBUG 0
#endif

#define WXOGRE_VERSION_MAJOR 0 
#define WXOGRE_VERSION_MINOR 1 

#define WXOGRE_STR_VER S(WXOGRE_VERSION_MAJOR) "." S(WXOGRE_VERSION_MINOR) 


#define WXOGRE_OGRE_VER ( (OGRE_VERSION_MAJOR*100) +  \
                          (OGRE_VERSION_MINOR *10) +  \
                          (OGRE_VERSION_PATCH)     )

#define WXOGRE_OGRE_STR_VER S(OGRE_VERSION_MAJOR) "." \
                            S(OGRE_VERSION_MINOR) "." \
                            S(OGRE_VERSION_PATCH)


#endif
