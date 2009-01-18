project(simunet)

# minimalna verzia cmake
cmake_minimum_required(VERSION 2.4)
cmake_policy(SET CMP0003 NEW)

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

# vyzadujeme python
find_package(PythonLibs REQUIRED)
include_directories(${PYTHON_INCLUDE_PATH})


set (simunet_VERSION_MAJOR 0)
set (simunet_VERSION_MINOR 1)

configure_file (
  "${PROJECT_SOURCE_DIR}/src/config.h.in"
  "${PROJECT_BINARY_DIR}/src/config.h"
  )

set(QT_USE_QTOPENGL 1)
include(${QT_USE_FILE})

#include(${QT_USE_FILE})

if (NOT QT_QTOPENGL_FOUND)
	message(FATAL_ERROR "No QT_QTOPENGL")
endif (NOT QT_QTOPENGL_FOUND)

include_directories(
	"${PROJECT_BINARY_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}/src"
	"${CMAKE_CURRENT_BINARY_DIR}/src"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/gui"
	"${CMAKE_CURRENT_BINARY_DIR}/src/gui"
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${CMAKE_CURRENT_BINARY_DIR}"
	${QT_INCLUDES})
add_definitions(${QT_DEFINITIONS})

# zdrojove kody
set(simunet_SRCS
	src/simunet.cpp
	src/gui/AboutDlg.cpp
	src/gui/CfgPerformance.cpp
	src/gui/ConfigPanel.cpp
	src/gui/ConfigureDlg.cpp
	src/gui/MainWindow.cpp
	src/gui/Scene.cpp
	src/gui/SecondaryWindow.cpp
	src/gui/SNIcon.cpp
	src/gui/SNTitleWidget.cpp)

# hlavickove subory ktore musi spracovat moc
set(simunet_MOC_HDRS
	src/gui/AboutDlg.h
	src/gui/CfgPerformance.h
	src/gui/ConfigPanel.h
	src/gui/ConfigureDlg.h
	src/gui/MainWindow.h
	src/gui/Scene.h
	src/gui/SecondaryWindow.h
	src/gui/SNTitleWidget.h)

set(simunet_HDRS
	src/gui/SNIcon.h)

# dialogy
set(simunet_UIS
	src/gui/AboutDlg.ui)

# zdroje
set(simunet_QRC
	gfx/resources.qrc)

# preklady
set(simunet_TRANS
	locale/simunet_sk_SK.ts)

# prekompilovane hlavicky
set(simunet_PCH
	src/pch.h)

set(core_SRCS
	src/core/PyCPPObject.cpp
	src/core/SNConfig.cpp
	src/core/SNDevice.cpp
	src/core/SNExceptions.cpp
	src/core/SNSimulate.cpp
	src/core/SNSimulateHelper.cpp)

set(core_HDRS
	src/core/PyCPPObject.h
	src/core/SNConfig.h
	src/core/SNDevice.h
	src/core/SNExceptions.h
	src/core/SNSimulate.h
	src/core/SNSimulateHelper.h)

qt4_wrap_cpp(simunet_MOC_SIMULATE_HELPER src/core/SNSimulateHelper.h)
qt4_wrap_cpp(simunet_MOC_SRCS ${simunet_MOC_HDRS})
qt4_wrap_ui(simunet_UIS_SRCS ${simunet_UIS})
qt4_add_resources(simunet_QRC_SRCS ${simunet_QRC})
qt4_add_translation(lrelease_outputs ${simunet_TRANS})

add_library(smlib STATIC
	${core_SRCS}
	${core_HDRS}
	${simunet_MOC_SIMULATE_HELPER}
)

add_executable(simunet
	${simunet_SRCS}
	${simunet_MOC_SRCS}
	${simunet_UIS_SRCS}
	${simunet_QRC_SRCS}
	${simunet_MOC_HDRS}
	${simunet_HDRS}
	${lrelease_outputs}
	${core_HDRS}
)
target_link_libraries(simunet smlib ${PYTHON_LIBRARIES} ${QT_LIBRARIES})

install (TARGETS simunet DESTINATION bin)

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

install(TARGETS simunet
	RUNTIME
	DESTINATION bin
	COMPONENT app)

install(FILES ${core_HDRS}
	DESTINATION include/simunet
	COMPONENT includes)

# cpack
SET(CMAKE_VERSION "${CMAKE_CACHE_MAJOR_VERSION}.${CMAKE_CACHE_MINOR_VERSION}.${CMAKE_CACHE_RELEASE_VERSION}")
if("${CMAKE_VERSION}" STRGREATER "2.4")
	option(USE_CPACK "Use CPack" ON)
	if(USE_CPACK)
		set(CPACK_PACKAGE_NAME "SimuNet")
		set(CPACK_PACKAGE_VENDOR "simunet.eu")
		set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Network simulator")
		set(CPACK_PACKAGE_VERSION "0.0.1")
		set(CPACK_PACKAGE_VERSION_MAJOR "0")
		set(CPACK_PACKAGE_VERSION_MINOR "0")
		set(CPACK_PACKAGE_VERSION_PATCH "1")
#set(CPACK_PACKAGE_INSTALL_DIRECTORY "dir")
		set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
		set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
		set(CPACK_COMPONENT_INCLUDES_DISPLAY_NAME "Includes")
		set(CPACK_COMPONENT_INCLUDES_DESCRIPTION
		  "Header files")

		set(CPACK_COMPONENTS_ALL ${CPACK_COMPONENTS_ALL} app includes)
		set(CPACK_COMPONENT_APP_HIDDEN ON)
		set(CPACK_COMPONENT_APP_REQUIRED ON)
		include(CPack)
	endif(USE_CPACK)
endif("${CMAKE_VERSION}" STRGREATER "2.4")

enable_testing()
add_subdirectory(tests)
