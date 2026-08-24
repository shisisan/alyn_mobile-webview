project(GameSA)

file(GLOB INCLUDE_FILES
        include/*.h
        include/**/*.h)

file(GLOB SOURCE_FILES
        include/*.cpp
        include/**/*.cpp)

add_library(${PROJECT_NAME} INTERFACE)

target_sources(${PROJECT_NAME} INTERFACE
        ${INCLUDE_FILES}
        ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} INTERFACE
        Memory)

target_include_directories(${PROJECT_NAME} INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/include")
