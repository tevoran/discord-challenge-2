cmake_minimum_required(VERSION 3.16)
project("dc2")

#BASICALLY RELEASE MODE
#SET(CMAKE_CXX_FLAGS  "-m64 -Ofast -flto -march=native -funroll-loops")

#DEBUGMODE
#SET(CMAKE_CXX_FLAGS "-g -ggdb -fsanitize=address")
SET(CMAKE_CXX_FLAGS "-g")

#PROFILERMODE
#SET(CMAKE_CXX_FLAGS "-g -pg")

#SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

include(FindPkgConfig)
find_library(SDL2 SDL2)
find_package(SDL2 REQUIRED)
find_library(SDL2_ttf SDL2_ttf)
find_library(SDL2_mixer SDL2_mixer)
find_library(GLEW GLEW)
find_library(GL GL)


#creating the library
include_directories(vendor/TGJGE/src/)
include_directories(vendor/TGJGE/vendor/stb/)
include_directories(vendor/TGJGE/vendor/glm/)
file(GLOB_RECURSE "engine" SRC vendor/TGJGE/src/*.c)
add_library(tgjge STATIC ${engine})

target_link_libraries(tgjge LINK_PRIVATE "${SDL2}")
target_link_libraries(tgjge LINK_PRIVATE "${SDL2_ttf}")
target_link_libraries(tgjge LINK_PRIVATE "${SDL2_mixer}")
target_link_libraries(tgjge LINK_PRIVATE m)
target_link_libraries(tgjge LINK_PRIVATE "${GLEW}")
target_link_libraries(tgjge LINK_PRIVATE "${GL}")

#creating library test
include_directories(src/)
file(GLOB_RECURSE "dc2" SRC src/*.cpp)
add_executable(dc2 ${dc2})
target_link_libraries(dc2 LINK_PRIVATE tgjge)
file(COPY vendor/TGJGE/shaders DESTINATION .)
file(COPY vendor/TGJGE/assets DESTINATION .)