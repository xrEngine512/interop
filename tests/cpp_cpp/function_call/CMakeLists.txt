cmake_minimum_required(VERSION 3.4)

add_subdirectory(modules)
configure_file(configuration.json configuration.json COPYONLY)
add_test(cpp_cpp.function_call ${TESTS_NODE} -c configuration.json)
