SET(LIB_NAME wx_ogre)
SET(LIB_INCDIR wx/ogre)

SET(CMAKE_DEBUG_POSTFIX _d)
SET(HEADER_PATH ${wxOgre_SOURCE_DIR}/include/${LIB_INCDIR})
SET(LIB_PUBLIC_HEADERS
	${HEADER_PATH}/ogre.h
	${HEADER_PATH}/app.h
	${HEADER_PATH}/utils.h
	${HEADER_PATH}/control.h
	${HEADER_PATH}/resources.h
	${HEADER_PATH}/rendersystem.h
	${HEADER_PATH}/prerequisites.h
	${HEADER_PATH}/wxFourWaySplitter.h
)

ADD_LIBRARY(${LIB_NAME}
	${LIB_PUBLIC_HEADERS}
	app.cpp
	utils.cpp
	control.cpp
	resources.cpp
	rendersystem.cpp
	wxFourWaySplitter.cpp
)

TARGET_LINK_LIBRARIES(${LIB_NAME}
	${wxWidgets_LIBRARIES}
	${OGRE_LIBRARIES}
	${GTK2_LIBRARIES}
)

SET_TARGET_PROPERTIES(${LIB_NAME} PROPERTIES
	VERSION ${WXOGRE_VERSION}
)

INCLUDE(ModuleInstall OPTIONAL)
