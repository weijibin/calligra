project(stageapp)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../part
    ${CMAKE_CURRENT_BINARY_DIR}/../part
    ${KOMAIN_INCLUDES}
    ${KOPAGEAPP_INCLUDES}
)

set(stage_KDEINIT_SRCS main.cpp )

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/../pics/*-apps-calligrastage.png")
ecm_add_app_icon(stage_KDEINIT_SRCS ICONS ${ICONS_SRCS})

kf5_add_kdeinit_executable(calligrastage ${stage_KDEINIT_SRCS})

if (APPLE)
   set_target_properties(calligrastage PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.template)
   set_target_properties(calligrastage PROPERTIES MACOSX_BUNDLE_GUI_IDENTIFIER "org.kde.calligrastage")
   set_target_properties(calligrastage PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME "Calligra Stage 2")
   install( FILES ${CMAKE_CURRENT_BINARY_DIR}/stage_KDEINIT_SRCS.icns DESTINATION ${BUNDLE_INSTALL_DIR}/calligrastage.app/Contents/Resources)
endif ()

target_link_libraries(kdeinit_calligrastage komain)

install(TARGETS kdeinit_calligrastage ${INSTALL_TARGETS_DEFAULT_ARGS})

target_link_libraries(calligrastage kdeinit_calligrastage)
install(TARGETS calligrastage ${INSTALL_TARGETS_DEFAULT_ARGS})

install( PROGRAMS org.kde.calligrastage.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install( FILES org.kde.calligrastage.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
