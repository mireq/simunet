
add_executable(moduleInitTest moduleInit/moduleInit.cpp)

target_link_libraries(moduleInitTest ${PYTHON_LIBRARIES} smlib)

add_test(ModuleInit moduleInitTest testmodule)
add_test(ModuleInit2 moduleInitTest testmodule testmodule)