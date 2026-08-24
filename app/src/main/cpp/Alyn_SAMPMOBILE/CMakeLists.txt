set(CMAKE_CXX_STANDARD 20)

if (${ANDROID_ABI} STREQUAL "armeabi-v7a")
    add_definitions(-DVER_x32=true)
elseif (${ANDROID_ABI} STREQUAL "arm64-v8a")
    add_definitions(-DVER_x32=false)
endif()

file(GLOB CORE_INCLUDE_FILES
        *.h
        Game/*.h
        Game/**/*.h
        Java/*.h
        Java/**/*.h
        Net/*.h
        UI/*.h
        UI/**/*.h
        UI/**/**/*.h
        Voice/*.h
        Voice/**/*.h
        Voice/**/**/*.h
)

file(GLOB CORE_SOURCE_FILES
        *.cpp
        Game/*.cpp
        Game/**/*.cpp
        Java/*.cpp
        Java/**/*.cpp
        Net/*.cpp
        UI/*.cpp
        UI/**/*.cpp
        UI/**/**/*.cpp
        Voice/*.cpp
        Voice/**/*.cpp
        Voice/**/**/*.cpp
)

file(GLOB CEF_INCLUDE_FILES
        cef/*.h
)

file(GLOB CEF_SOURCE_FILES
        cef/*.cpp
)

add_library(Alyn_SAMPMOBILE SHARED
        ${CORE_INCLUDE_FILES}
        ${CORE_SOURCE_FILES}
)

add_library(SAMPMobileCef SHARED
        ${CEF_INCLUDE_FILES}
        ${CEF_SOURCE_FILES}
)

target_include_directories(Alyn_SAMPMOBILE PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(SAMPMobileCef PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(Alyn_SAMPMOBILE PRIVATE
        SAMPMobileCef
        android
        log
        EGL
        GLESv2
        Memory
        GameSA
        imgui
        rapidjson
        RakNet
        Encoding
        opus
        obfuscator
        bass
        opencv
        spdlog::spdlog
)

target_link_libraries(SAMPMobileCef PRIVATE
        android
        log
        EGL
        GLESv2
)

target_compile_options(Alyn_SAMPMOBILE PRIVATE
        -w
        -O3
        -fexceptions
        -fstack-protector
)

target_compile_options(SAMPMobileCef PRIVATE
        -w
        -O3
        -fexceptions
        -fstack-protector
)

target_compile_definitions(Alyn_SAMPMOBILE PRIVATE
        RAKSAMP_CLIENT
)

target_compile_definitions(SAMPMobileCef PRIVATE
        RAKSAMP_CLIENT
)

add_subdirectory(Memory)
add_subdirectory(Deps)
add_subdirectory(GameSA)