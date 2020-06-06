cmake_minimum_required(VERSION 3.7.0)
project(cpp-json)

option(CPPJSON_WITH_DEVELOPMENT_DEPENDANCIES " need conan package manager" ON)

########### dependencies from cmake #########
if(TETRIS_WITH_DEVELOPMENT_DEPENDANCIES)
    include(${CMAKE_BINARY_DIR}/conan_paths.cmake)
endif()

########### modern C++ flags######################
set (CMAKE_CXX_STANDARD             17)
set (CMAKE_CXX_STANDARD_REQUIRED    ON)
set (CMAKE_CXX_EXTENSIONS           OFF)



########### Tetris Library ###################

add_library(${PROJECT_NAME} 
    src/json/json.cpp)
add_library(${PROJECT_NAME}::${PROJECT_NAME}  ALIAS ${PROJECT_NAME} )
target_include_directories(${PROJECT_NAME}  PUBLIC src)



######## test ########
if(CPPJSON_WITH_DEVELOPMENT_DEPENDANCIES)
    find_package(Catch2 REQUIRED)

    add_executable(test_cpp_json
                    test/main_catch.cpp
                    test/test_acceptance.cpp
                    test/Testables.h)
    target_compile_definitions(test_cpp_json PUBLIC UNITEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}/test")
    target_link_libraries(test_cpp_json ${PROJECT_NAME}::${PROJECT_NAME} Catch2::Catch2 )


    ######### test coverage with kcov and ctest ######################

    find_program(KCOV kcov)
    message("kcov: ${KCOV}")
    if( KCOV )
        enable_testing()

        add_test(
            NAME json_coverage 
            COMMAND ${KCOV} --exclude-pattern=/usr/ --include-path=${CMAKE_CURRENT_LIST_DIR}/src  cov $<TARGET_FILE:test_cpp_json>  
            )
    endif()
endif()