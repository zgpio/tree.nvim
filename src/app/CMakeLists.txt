include_directories(..)

if(WIN32)
	enable_language(RC)
elseif(APPLE)
	# set(MACOSX_BUNDLE_INFO_PLIST ${PROJECT_SOURCE_DIR}/cmake/MacOSXBundleInfo.plist.in)
else()
endif()

include(GNUInstallDirs)

set(SRC_FILE column.cpp tree.cpp app.cpp)
if(WIN32)
    list(APPEND SRC_FILE wcwidth.cpp)
endif()

add_library(tree-server ${SRC_FILE})

target_link_libraries(tree-server neovim-qt)

if(NOT APPLE)
	set_property(SOURCE app.cpp PROPERTY COMPILE_DEFINITIONS
		)
endif()

add_executable(tree-nvim WIN32 MACOSX_BUNDLE main.cpp
	${NEOVIM_RCC_SOURCES}
	${RES_FILE})

target_link_libraries(tree-nvim ${MSGPACK_LIBRARIES} tree-server)

install(DIRECTORY runtime/ DESTINATION ${CMAKE_INSTALL_PREFIX})
install(TARGETS tree-nvim DESTINATION ${CMAKE_INSTALL_BINDIR})

if(WIN32 AND NOT CMAKE_CROSSCOMPILING)
endif()
