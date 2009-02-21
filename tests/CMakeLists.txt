include_directories("${PROJECT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(moduleInitTest moduleInit/moduleInit.cpp shared.cpp)
add_executable(comunicationTest comunication/comunication.cpp shared.cpp)
add_executable(snbloateditTest snbloatedit/snbloatedit.cpp)

target_link_libraries(moduleInitTest smlib ${PYTHON_LIBRARIES} ${QT_LIBRARIES})
target_link_libraries(comunicationTest smlib ${PYTHON_LIBRARIES} ${QT_LIBRARIES})
target_link_libraries(snbloateditTest smlib ${OPENSSL_LIBRARIES})

add_test(ModuleInit moduleInitTest testmodule)
set_tests_properties(ModuleInit PROPERTIES PASS_REGULAR_EXPRESSION "init\nframe1\nframe2\ndel 1")

add_test(ModuleInit2 moduleInitTest testmodule testmodule)
set_tests_properties(ModuleInit2 PROPERTIES PASS_REGULAR_EXPRESSION "init\nframe1\nframe2\ndel 1\ninit\nframe1\nframe2\ndel 2")

add_test(comunication comunicationTest testmodule testmodule)
set_tests_properties(comunication PROPERTIES PASS_REGULAR_EXPRESSION "init\nframe\nid:1\ndata:emarf\ntelnet:line:c\ntelnet:1:enil:cmd\nreset\ncfg:config\ndump:config\nhr:url:name:value\nhtmlrecv\nabc\ndel 1\ninit\nframe\nid:2\ndata:emarf\ntelnet:line:c\ntelnet:2:enil:cmd\nreset\ncfg:config\ndump:config\nhr:url:name:value\nhtmlrecv\nabc\ndel 2")
