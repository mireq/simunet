project(simunet)

# minimalna verzia cmake
cmake_minimum_required(VERSION 2.6.0)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_FLAGS_RELEASE "-O2 -Wall -fexceptions -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG   "-O2 -g3 -ggdb -Wall -fexceptions -DDEBUG")

# ak kompilujeme s debug
if(CMAKE_BUILD_TYPE MATCHES Debug)
	message(STATUS "Configured to compile including full debug information in the executables")
else(CMAKE_BUILD_TYPE MATCHES Debug)
	message(STATUS "Configured to compile including debug information in the executables")
	set(CMAKE_BUILD_TYPE Release)
endif(CMAKE_BUILD_TYPE MATCHES Debug)

# vyzadujeme Qt4
find_package(Qt4 REQUIRED)

set (simunet_VERSION_MAJOR 0)
set (simunet_VERSION_MINOR 1)

configure_file (
  "${PROJECT_SOURCE_DIR}/src/config.h.in"
  "${PROJECT_BINARY_DIR}/src/config.h"
  )

set(QT_USE_QTOPENGL 1)

#include(${QT_USE_FILE})

if (NOT QT_QTOPENGL_FOUND)
	message(FATAL_ERROR "No QT_QTOPENGL")
endif (NOT QT_QTOPENGL_FOUND)

include_directories("${PROJECT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/src" "${CMAKE_CURRENT_BINARY_DIR}/src" "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_BINARY_DIR}" ${QT_INCLUDES})
add_definitions("${QT_DEFINITIONS}")

# zdrojove kody
set(simunet_SRCS
	src/simunet.cpp
	src/AboutDlg.cpp
	src/MainWindow.cpp
	src/Scene.cpp
	src/SecondaryWindow.cpp)

# hlavickove subory ktore musi spracovat moc
set(simunet_MOC_HDRS
	src/AboutDlg.h
	src/MainWindow.h
	src/Scene.h
	src/SecondaryWindow.h)

# dialogy
set(simunet_UIS
	src/AboutDlg.ui)

# zdroje
set(simunet_QRC
	gfx/resources.qrc)

# preklady
set(simunet_TRANS
	locale/simunet_sk_SK.ts)

# prekompilovane hlavicky
set(simunet_PCH
	src/pch.h)

qt4_wrap_cpp(simunet_MOC_SRCS ${simunet_MOC_HDRS})
qt4_wrap_ui(simunet_UIS_SRCS ${simunet_UIS})
qt4_add_resources(simunet_QRC_SRCS ${simunet_QRC})
qt4_add_translation(lrelease_outputs ${simunet_TRANS})

add_executable(simunet ${simunet_SRCS} ${simunet_MOC_SRCS} ${simunet_UIS_SRCS}
${simunet_QRC_SRCS} ${lrelease_outputs})
target_link_libraries(simunet ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTOPENGL_LIBRARY})

# podpora pch z
# projektu rosegarden
option(WANT_PCH "Whether to use pre-compiled headers" OFF)

set(USE_PCH FALSE)
if (WANT_PCH)
	find_package(PCHSupport)
	if (PCHSupport_FOUND)
		set(USE_PCH TRUE)
		add_definitions(-DUSE_PCH)
		message(STATUS "Enabling precompiled headers for GCC ${gcc_compiler_version}")
	endif (PCHSupport_FOUND)
endif (WANT_PCH)

# odkomentovat pre podrobne vypisy
# SET(CMAKE_VERBOSE_MAKEFILE ON)

if (USE_PCH)
	add_precompiled_header(simunet_pch src/pch.h)
	add_dependencies(simunet simunet_pch)
endif (USE_PCH)