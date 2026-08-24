project(RakNet)

file(GLOB INCLUDE_FILES
        ${PROJECT_NAME}/*.h
        ${PROJECT_NAME}/SAMP/*.h
)

file(GLOB SOURCE_FILES
        ${PROJECT_NAME}/*.cpp
        ${PROJECT_NAME}/SAMP/*.cpp
)

add_library(${PROJECT_NAME} INTERFACE)

target_sources(${PROJECT_NAME} INTERFACE
        ${INCLUDE_FILES}
        ${SOURCE_FILES})

# then can include with #include <RakNet/RakPeer.h> for example.
target_include_directories(${PROJECT_NAME} INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}"
)
