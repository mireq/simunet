project(simunet)

# minimalna verzia cmake
cmake_minimum_required(VERSION 2.6.0)

# vyzadujeme Qt4
find_package(Qt4 REQUIRED)

set (simunet_VERSION_MAJOR 0)
set (simunet_VERSION_MINOR 1)

configure_file (
  "${PROJECT_SOURCE_DIR}/src/config.h.in"
  "${PROJECT_BINARY_DIR}/src/config.h"
  )

set(QT_USE_QTOPENGL 1)

include(${QT_USE_FILE})

if (NOT QT_QTOPENGL_FOUND)
	message(FATAL_ERROR "No QT_QTOPENGL")
endif (NOT QT_QTOPENGL_FOUND)

include_directories("${PROJECT_BINARY_DIR}" "${CMAKE_CURRENT_BINARY_DIR}" "${QT_INCLUDE_DIR}")
add_definitions("${QT_DEFINITIONS}")

# zdrojove kody
set(simunet_SRCS
	src/simunet.cpp)

# hlavickove subory ktore musi spracovat moc
set(simunet_MOC_HDRS)

# dialogy
set(simunet_UIS)

# zdroje
set(simunet_QRC
	gfx/resources.qrc)

# preklady
set(simunet_TRANS
	locale/simunet_sk_SK.ts)

qt4_wrap_cpp(simunet_MOC_SRCS ${simunet_MOC_HDRS})
qt4_wrap_ui(simunet_UIS_SRCS ${simunet_UIS})
qt4_add_resources(simunet_QRC_SRCS ${simunet_QRC})
qt4_add_translation(lrelease_outputs ${simunet_TRANS})

add_executable(simunet ${simunet_SRCS} ${simunet_MOC_SRCS} ${simunet_UIS_SRCS}
${fatrat_QRC_SRCS} ${lrelease_outputs})
target_link_libraries(simunet ${QT_LIBRARIES} ${QT_ADDITIONAL_LIBS})

# odkomentovat pre podrobne vypisy
# SET(CMAKE_VERBOSE_MAKEFILE ON)

# ak kompilujeme s debug
if(CMAKE_BUILD_TYPE MATCHES Debug)
	add_definitions(-ggdb)
	add_definitions(-DDEBUG)
	add_definitions(-Wall)
endif(CMAKE_BUILD_TYPE MATCHES Debug)

add_definitions(-Wall -O2)
