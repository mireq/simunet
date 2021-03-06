project(simunet)

# minimalna verzia cmake
cmake_minimum_required(VERSION 2.4)
cmake_policy(SET CMP0003 NEW)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

set(CMAKE_CXX_FLAGS_RELEASE "-O2 -Wall -fexceptions -DNDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG   "-O2 -g3 -ggdb -Wall -Wextra -fexceptions -DDEBUG")

# ak kompilujeme s debug
if(CMAKE_BUILD_TYPE MATCHES Debug)
	message(STATUS "Configured to compile including full debug information in the executables")
	add_definitions(-DQT_DEBUG)
else(CMAKE_BUILD_TYPE MATCHES Debug)
	message(STATUS "Configured to compile including debug information in the executables")
	set(CMAKE_BUILD_TYPE Release)
	add_definitions(-DQT_NO_DEBUG)
endif(CMAKE_BUILD_TYPE MATCHES Debug)

# vyzadujeme Qt4
find_package(Qt4 REQUIRED)

# vyzadujeme OpenSSL
find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

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
set(QT_USE_QTWEBKIT 1)
set(QT_USE_QTNETWORK 1)
include(${QT_USE_FILE})

#include(${QT_USE_FILE})

if (NOT QT_QTOPENGL_FOUND)
	message(FATAL_ERROR "No QT_QTWEBKIT")
endif (NOT QT_QTOPENGL_FOUND)

if (NOT QT_QTWEBKIT_FOUND)
	message(FATAL_ERROR "No QT_QTWEBKIT")
endif (NOT QT_QTWEBKIT_FOUND)

set (SUBDIRS
	src
	src/gui
	src/gui/diagram
	src/gui/models
	src/gui/noqobject
	src/gui/noqobject/diagram
	src/gui/widgets
)

include_directories(
	"${PROJECT_BINARY_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${CMAKE_CURRENT_BINARY_DIR}"
	${QT_INCLUDES})

foreach(SUBDIR ${SUBDIRS})
	include_directories(
		"${CMAKE_CURRENT_SOURCE_DIR}/${SUBDIR}"
		"${CMAKE_CURRENT_BINARY_DIR}/${SUBDIR}"
	)
endforeach(SUBDIR)

set(simunet_HDRS
	src/config.h
	src/pch.h
	src/simunet.h
	src/sntypes.h
	src/gui/noqobject/SNAbstractDevicesScene.h
	src/gui/noqobject/SNIcon.h
	src/gui/noqobject/diagram/SNDiagramDevice.h
	src/gui/noqobject/diagram/SNDiagramItem.h
	src/gui/noqobject/diagram/SNDiagramLineSegment.h
)

set(simunet_MOC_HDRS
	src/gui/diagram/SNDevicesDiagramScene.h
	src/gui/diagram/SNDiagramLine.h
	src/gui/diagram/SNDiagramView.h
	src/gui/dialogs/AboutDlg.h
	src/gui/dialogs/CfgApperance.h
	src/gui/dialogs/CfgApperance_p.h
	src/gui/dialogs/CfgPerformance.h
	src/gui/dialogs/ConfigureDlg.h
	src/gui/dialogs/DeviceSettingsDlg.h
	src/gui/dialogs/MainWindow.h
	src/gui/dialogs/SecondaryWindow.h
	src/gui/models/SNDevicesListModel.h
	src/gui/widgets/SNColorSelectWidget.h
	src/gui/widgets/SNColorSelectWidget_p.h
	src/gui/widgets/SNConfigPanel.h
	src/gui/widgets/SNFontSelect.h
	src/gui/widgets/SNScene.h
	src/gui/widgets/SNSplash.h
	src/gui/widgets/SNTelnetWidget.h
	src/gui/widgets/SNTitleWidget.h
	src/gui/widgets/SNWebConfigPage.h
	src/gui/widgets/SNWebConfigWidget.h
)

set(simunet_SRCS
	src/simunet.cpp
	src/gui/diagram/SNDevicesDiagramScene.cpp
	src/gui/diagram/SNDiagramLine.cpp
	src/gui/diagram/SNDiagramView.cpp
	src/gui/dialogs/AboutDlg.cpp
	src/gui/dialogs/CfgApperance.cpp
	src/gui/dialogs/CfgPerformance.cpp
	src/gui/dialogs/ConfigureDlg.cpp
	src/gui/dialogs/DeviceSettingsDlg.cpp
	src/gui/dialogs/MainWindow.cpp
	src/gui/dialogs/SecondaryWindow.cpp
	src/gui/models/SNDevicesListModel.cpp
	src/gui/noqobject/SNAbstractDevicesScene.cpp
	src/gui/noqobject/diagram/SNDiagramDevice.cpp
	src/gui/noqobject/diagram/SNDiagramItem.cpp
	src/gui/noqobject/diagram/SNDiagramLineSegment.cpp
	src/gui/noqobject/SNIcon.cpp
	src/gui/widgets/SNColorSelectWidget.cpp
	src/gui/widgets/SNConfigPanel.cpp
	src/gui/widgets/SNFontSelect.cpp
	src/gui/widgets/SNScene.cpp
	src/gui/widgets/SNSplash.cpp
	src/gui/widgets/SNTelnetWidget.cpp
	src/gui/widgets/SNTitleWidget.cpp
	src/gui/widgets/SNWebConfigPage.cpp
	src/gui/widgets/SNWebConfigWidget.cpp
)

# dialogy
set(simunet_UIS
	src/gui/dialogs/AboutDlg.ui)

#ADD_DEPENDENCIES(${LIB} updateEOglext)
# zdroje
set(simunet_QRC
	resources/resources.qrc)

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


add_subdirectory(src/core)
add_executable(simunet
	${simunet_HDRS}
	${simunet_MOC_HDRS}
	${simunet_SRCS}
	${simunet_MOC_SRCS}
	${simunet_UIS_SRCS}
	${simunet_QRC_SRCS}
	${lrelease_outputs}
	${smlib_HDRS}
)

target_link_libraries(simunet smlib ${PYTHON_LIBRARIES} ${QT_LIBRARIES} ${OPENSSL_LIBRARIES})

install (TARGETS simunet DESTINATION bin)

# podpora pch
# podrobnosti v adresari cmake, subor README
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
	add_precompiled_header(simunet src/pch.h)
endif (USE_PCH)

install(TARGETS simunet
	RUNTIME
	DESTINATION bin
	COMPONENT app)


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
