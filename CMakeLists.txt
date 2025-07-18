cmake_minimum_required(VERSION 3.12)
#-------------------------------------------------------------------------------------------
# I'm going to use vcpk in most cases for our install of 3rd party libs
# this is going to check the environment variable for CMAKE_TOOLCHAIN_FILE and this must point to where
# vcpkg.cmake is in the University this is set in your .bash_profile to
# export CMAKE_TOOLCHAIN_FILE=/public/devel/2020/vcpkg/scripts/buildsystems/vcpkg.cmake
# to build see the NGL instructions 
# Windows :- mkdir build; cd build ; cmake -DCMAKE_PREFIX_PATH=~/NGL/ .. ; cmake --build . 
# Linux / Mac mkdir build; cd build; cmake -DCMAKE_PREFIX_PATH~/NGL/ .. ; make
#-------------------------------------------------------------------------------------------
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE AND DEFINED ENV{CMAKE_TOOLCHAIN_FILE})
   set(CMAKE_TOOLCHAIN_FILE $ENV{CMAKE_TOOLCHAIN_FILE})
endif()

# Name of the project
project(DefferedShader)
# This is the name of the Exe change this and it will change everywhere
set(TargetName DefferedShader)
# This will include the file NGLConfig.cmake, you need to add the location to this either using
# -DCMAKE_PREFIX_PATH=~/NGL or as a system environment variable. 
find_package(NGL CONFIG REQUIRED)
# Instruct CMake to run moc automatically when needed (Qt projects only)
set(CMAKE_AUTOMOC ON)
# find Qt libs
# find Qt libs first we check for Version 6
find_package(Qt6 COMPONENTS OpenGL Widgets QUIET )
if ( Qt6_FOUND )
    message("Found Qt6 Using that")
else()
    message("Found Qt5 Using that")
    find_package(Qt5 COMPONENTS OpenGL Widgets REQUIRED)
endif()

# use C++ 17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)
# Set the name of the executable we want to build
add_executable(${TargetName})

# Add NGL include path
include_directories(include $ENV{HOME}/NGL/include)
target_sources(${TargetName} PRIVATE ${PROJECT_SOURCE_DIR}/src/main.cpp
			${PROJECT_SOURCE_DIR}/src/NGLScene.cpp
			${PROJECT_SOURCE_DIR}/src/main.cpp
			${PROJECT_SOURCE_DIR}/src/NGLSceneMouseControls.cpp
			${PROJECT_SOURCE_DIR}/src/FirstPersonCamera.cpp
			${PROJECT_SOURCE_DIR}/include/NGLScene.h
            ${PROJECT_SOURCE_DIR}/include/Mesh.h
            ${PROJECT_SOURCE_DIR}/include/FirstPersonCamera.h

)

target_link_libraries(${TargetName} PRIVATE Qt::Widgets Qt::OpenGL NGL)

add_custom_target(${TargetName}CopyShaders ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/shaders
    $<TARGET_FILE_DIR:${TargetName}>/shaders

        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/textures
        $<TARGET_FILE_DIR:${TargetName}>/textures

        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/models
        $<TARGET_FILE_DIR:${TargetName}>/models
)


find_package(GTest CONFIG REQUIRED)
include(GoogleTest)
enable_testing()
add_executable(DSTests
        tests/DSTests.cpp)
target_include_directories(DSTests PRIVATE include)

target_sources(DSTests PRIVATE ${CMAKE_SOURCE_DIR}/tests/DSTests.cpp
        ${CMAKE_SOURCE_DIR}/src/NGLScene.cpp
        ${CMAKE_SOURCE_DIR}/src/FirstPersonCamera.cpp
        ${CMAKE_SOURCE_DIR}/src/NGLSceneMouseControls.cpp
        ${CMAKE_SOURCE_DIR}/include/NGLScene.h
        ${CMAKE_SOURCE_DIR}/include/Mesh.h
        ${CMAKE_SOURCE_DIR}/include/FirstPersonCamera.h

)
target_link_libraries(DSTests PRIVATE GTest::gtest GTest::gtest_main )
target_link_libraries(DSTests PRIVATE Qt::Widgets Qt::OpenGL NGL)


gtest_discover_tests(DSTests)