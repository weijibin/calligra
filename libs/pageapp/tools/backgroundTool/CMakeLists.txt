include_directories( ../.. )

set(kopabackgroundtool_PART_SRCS
    Plugin.cpp
    KoPABackgroundToolFactory.cpp
    KoPAMasterPageDocker.cpp
)


add_library(kopabackgroundtool MODULE ${kopabackgroundtool_PART_SRCS})
calligra_pageapptool_desktop_to_json(kopabackgroundtool kopabackgroundtool.desktop)

target_link_libraries(
    kopabackgroundtool
    kopageapp
)

install(TARGETS kopabackgroundtool  DESTINATION ${PLUGIN_INSTALL_DIR}/calligra/pageapptools)


########### install files ###############

if (SHOULD_BUILD_DEVEL_HEADERS)
install(FILES
    KoPABackgroundTool.h
    DESTINATION
    ${INCLUDE_INSTALL_DIR}/calligra COMPONENT Devel
)
endif()

ecm_install_icons( ICONS
    22-actions-backgroundtool.png
    DESTINATION ${DATA_INSTALL_DIR}/calligra/icons
    THEME hicolor
)
