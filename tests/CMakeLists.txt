include_directories("${PROJECT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(moduleInitTest moduleInit/moduleInit.cpp shared.cpp)

target_link_libraries(moduleInitTest smlib ${PYTHON_LIBRARIES} ${QT_LIBRARIES})

add_test(ModuleInit moduleInitTest testmodule)
set_tests_properties(ModuleInit PROPERTIES PASS_REGULAR_EXPRESSION "init\nframe1\nframe2\ndel 1")

add_test(ModuleInit2 moduleInitTest testmodule testmodule)
set_tests_properties(ModuleInit2 PROPERTIES PASS_REGULAR_EXPRESSION "init\nframe1\nframe2\ndel 1\ninit\nframe1\nframe2\ndel 2")