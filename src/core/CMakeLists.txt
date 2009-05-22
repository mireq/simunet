project(smlib)

include_directories("${PROJECT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")

set(smlib_HDRS
	PyCPPObject.h
	SNAccessors.h
	SNBloat.h
	SNBloatList.h
	SNConfig.h
	SNDevice.h
	SNExceptions.h
	SNHwPort.h
	SNLog.h
	map/SNDevTreeItem.h
	map/SNDevTreeNode.h
	map/SNMapControlPointItem.h
	map/SNMapDeviceItem.h
	map/SNMap.h
	map/SNMapItem.h
	map/SNMapLineItem.h
	map/SNPoint3f.h
)

set(smlib_MOC_HDRS
	SNDynamicSettings.h
	SNGuiSettings.h
	SNSimulate.h
	SNSimulateHelper.h
)

set(smlib_SRCS
	PyCPPObject.cpp
	SNAccessors.cpp
	SNBloat.cpp
	SNBloatList.cpp
	SNConfig.cpp
	SNDevice.cpp
	SNDynamicSettings.cpp
	SNExceptions.cpp
	SNGuiSettings.cpp
	SNHwPort.cpp
	SNLog.cpp
	SNSimulate.cpp
	SNSimulateHelper.cpp
	map/SNDevTreeItem.cpp
	map/SNDevTreeNode.cpp
	map/SNMapControlPointItem.cpp
	map/SNMap.cpp
	map/SNMapDeviceItem.cpp
	map/SNMapItem.cpp
	map/SNMapLineItem.cpp
	map/SNPoint3f.cpp
)

qt4_wrap_cpp(smlib_MOC_SRCS ${smlib_MOC_HDRS})

add_library(smlib STATIC
	${smlib_HDRS}
	${smlib_MOC_HDRS}
	${smlib_SRCS}
	${smlib_MOC_SRCS}
)

install(FILES ${smlib_HDRS}
	DESTINATION include/simunet
	COMPONENT includes)
