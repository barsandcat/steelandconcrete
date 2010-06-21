include_directories(include)

option(QUICKGUI_STATIC FALSE)
if (QUICKGUI_STATIC)
	set(QUICKGUI_LIB_TYPE STATIC)
else()
	set(QUICKGUI_LIB_TYPE SHARED)
endif()

file(GLOB QUICKGUI_SRC "src/*.cpp")
file(GLOB QUICKGUI_INCLUDE "include/*.h")

add_library(QuickGUI ${QUICKGUI_LIB_TYPE} ${QUICKGUI_SRC})
target_link_libraries(QuickGUI ${OGRE_LIBRARY})

install(TARGETS QuickGUI LIBRARY DESTINATION lib)
install(FILES ${QUICKGUI_INCLUDE} DESTINATION include/QuickGUI)
file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/QuickGUI.h "#include <QuickGUI/QuickGUI.h>\n")
install(FILES QuickGUI.h DESTINATION include)
