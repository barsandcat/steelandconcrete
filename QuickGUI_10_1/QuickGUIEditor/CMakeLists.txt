include_directories(include ${CMAKE_SOURCE_DIR}/QuickGUI/include)

aux_source_directory(src QuickGUIEditor_SRC)

add_executable(QuickGUIEditor ${QuickGUIEditor_SRC})
target_link_libraries(QuickGUIEditor QuickGUI ${OGRE_LIBRARY} ${OIS_LIBRARY})
