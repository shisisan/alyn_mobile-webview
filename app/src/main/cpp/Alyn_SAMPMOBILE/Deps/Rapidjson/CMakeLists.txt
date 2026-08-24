project(rapidjson)

file(GLOB INCLUDE_FILES
        ${PROJECT_NAME}/*.h
        ${PROJECT_NAME}/error/*.h
        ${PROJECT_NAME}/internal/*.h
        ${PROJECT_NAME}/msinttypes/*.h
)

file(GLOB SOURCE_FILES
        ${PROJECT_NAME}/*.cpp
        ${PROJECT_NAME}/error/*.cpp
        ${PROJECT_NAME}/internal/*.cpp
        ${PROJECT_NAME}/msinttypes/*.cpp
)

add_library(${PROJECT_NAME} INTERFACE)

target_sources(${PROJECT_NAME} INTERFACE
        ${INCLUDE_FILES}
        ${SOURCE_FILES})

# then can include with #include <rapidjson/document.h> for example.
target_include_directories(${PROJECT_NAME} INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}"
)
