# lua-kafka: kafka for lua
# Jian Chen <admin@chensoft.com>
# http://chensoft.com
cmake_minimum_required(VERSION 3.1)
project(kafka VERSION 1.8.2 LANGUAGES C)

add_library(kafka MODULE "")

# environment
set_target_properties(kafka PROPERTIES C_STANDARD 99 C_STANDARD_REQUIRED ON)
set_target_properties(kafka PROPERTIES PREFIX "")

# source codes
file(GLOB PROJ_INC src/*.h)
file(GLOB PROJ_SRC src/*.c)

# third party
find_package(Lua REQUIRED)
find_library(RDKAFKA rdkafka REQUIRED)

# generate lib
target_include_directories(kafka PRIVATE ${LUA_INCLUDE_DIR})
target_sources(kafka PRIVATE ${PROJ_INC} ${PROJ_SRC})
target_link_libraries(kafka rdkafka ${LUA_LIBRARIES})

# install lib
install(TARGETS kafka DESTINATION "${CMAKE_INSTALL_PREFIX}/lib")