project(bass)

file(GLOB INCLUDE_FILES
        bass.h
        bass_fx.h
)
file(GLOB SOURCE_FILES
        bass.cpp
)

add_library(${PROJECT_NAME} INTERFACE)

target_sources(${PROJECT_NAME} INTERFACE
        ${INCLUDE_FILES}
        ${SOURCE_FILES}
)

target_include_directories(${PROJECT_NAME} INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}"
)
