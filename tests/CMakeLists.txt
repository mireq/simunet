include_directories("${PROJECT_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")

add_executable(moduleInitTest moduleInit/moduleInit.cpp shared.cpp)

target_link_libraries(moduleInitTest smlib ${PYTHON_LIBRARIES})

add_test(ModuleInit moduleInitTest testmodule)
add_test(ModuleInit2 moduleInitTest testmodule testmodule)