prefix=@CMAKE_INSTALL_PREFIX@
exec_prefix=@CMAKE_INSTALL_PREFIX@
libdir=${exec_prefix}/lib
includedir=${exec_prefix}/include

Name: wxOgre
Description: A library to create applications mixing Ogre and wxWidgets.
Version: @WXOGRE_VERSION@
Libs: -L${libdir} -lwx_ogre
Cflags: -I${includedir}
