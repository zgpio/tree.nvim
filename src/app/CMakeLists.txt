set(QTLIBS Qt5::Network)

include_directories(..)

if(WIN32)
	if (USE_STATIC_QT)
		add_definitions(-DUSE_STATIC_QT)
		link_directories(${CMAKE_PREFIX_PATH}/share/qt5/plugins/platforms)
		set(QTLIBS ${QTLIBS} Qt5PlatformCompositorSupport qwindows)
	endif()
	enable_language(RC)
elseif(APPLE)
	# set_source_files_properties(${ICON_PATH} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
	# set(MACOSX_BUNDLE_INFO_PLIST ${PROJECT_SOURCE_DIR}/cmake/MacOSXBundleInfo.plist.in)
else()
endif()

include(GNUInstallDirs)

set(SRC_FILE column.cpp tree.cpp app.cpp)
if(WIN32)
    list(APPEND SRC_FILE wcwidth.cpp)
endif()

add_library(tree-nvim ${SRC_FILE})

target_link_libraries(tree-nvim neovim-qt)

if(NOT APPLE)
	set_property(SOURCE app.cpp PROPERTY COMPILE_DEFINITIONS
		)
endif()

add_executable(nvim-qt WIN32 MACOSX_BUNDLE main.cpp
	${NEOVIM_RCC_SOURCES}
	${RES_FILE}
	${ICON_PATH})

target_link_libraries(nvim-qt ${QTLIBS} ${MSGPACK_LIBRARIES} tree-nvim)

if(APPLE)
#	TODO: Bundle Qt, currently conflicts with brew Qt
#	add_custom_command(TARGET nvim-qt COMMAND macdeployqt
#		ARGS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/nvim-qt.app
#		COMMENT "Bundling Qt libraries")
endif()

install(TARGETS nvim-qt DESTINATION ${CMAKE_INSTALL_BINDIR})

if(WIN32 AND NOT CMAKE_CROSSCOMPILING)
	if(Qt5Core_VERSION VERSION_LESS 5.3)
		message(WARNING "Cannot use windeployqt in Qt 5.2 and earlier")
	elseif(NOT DEFINED USE_STATIC_QT)
		include(WinDeployQt)
		WinDeployQt(TARGET nvim-qt COMPILER_RUNTIME INCLUDE_MODULES ${QTLIBS} EXCLUDE_MODULES webkit webkit2)
		install(DIRECTORY ${PROJECT_BINARY_DIR}/windeployqt/
			DESTINATION ${CMAKE_INSTALL_BINDIR})
	endif()
endif()
