include_directories("${PROJECT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")

option(BUILD_BENCHMARKS "Build Benchmarks" OFF)

add_executable(moduleInitTest moduleInit/moduleInit.cpp shared.cpp)
add_executable(comunicationTest comunication/comunication.cpp shared.cpp)
add_executable(snbloateditTest snbloatedit/snbloatedit.cpp)
add_executable(snbloatlistTest snbloatlist/snbloatlist.cpp)
if(BUILD_BENCHMARKS)
	qt4_add_resources(simunet_QRC_SRC ../resources/resources.qrc)
	add_executable(performanceTest performance/performance.cpp ${simunet_QRC_SRC})
endif(BUILD_BENCHMARKS)

target_link_libraries(moduleInitTest smlib ${PYTHON_LIBRARIES} ${QT_LIBRARIES})
target_link_libraries(comunicationTest smlib ${PYTHON_LIBRARIES} ${QT_LIBRARIES})
target_link_libraries(snbloateditTest smlib ${OPENSSL_LIBRARIES})
target_link_libraries(snbloatlistTest smlib ${OPENSSL_LIBRARIES})
if(BUILD_BENCHMARKS)
	target_link_libraries(performanceTest smlib ${PYTHON_LIBRARIES} ${QT_LIBRARIES})
endif(BUILD_BENCHMARKS)


add_test(ModuleInit moduleInitTest testmodule)
set_tests_properties(ModuleInit PROPERTIES PASS_REGULAR_EXPRESSION "init\nframe1\nframe2\ndel 1")

add_test(ModuleInit2 moduleInitTest testmodule testmodule)
set_tests_properties(ModuleInit2 PROPERTIES PASS_REGULAR_EXPRESSION "init\nframe1\nframe2\ndel 1\ninit\nframe1\nframe2\ndel 2")

add_test(comunication comunicationTest testmodule testmodule)
set_tests_properties(comunication PROPERTIES PASS_REGULAR_EXPRESSION "init\nframe\nid:1\ndata:emarf\ntelnet:line:c\ntelnet:1:enil:cmd\nreset\ncfg:config\ndump:config\nhr:url:name:value\nhtmlrecv\nabc\ndel 1\ninit\nframe\nid:2\ndata:emarf\ntelnet:line:c\ntelnet:2:enil:cmd\nreset\ncfg:config\ndump:config\nhr:url:name:value\nhtmlrecv\nabc\ndel 2")
