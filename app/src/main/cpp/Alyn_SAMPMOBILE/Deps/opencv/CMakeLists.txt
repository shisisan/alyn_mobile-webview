project(opencv)

file(GLOB INCLUDE_FILES
        "include/**/*.h"
        "include/**/*.hpp"
)

file(GLOB SOURCE_FILES

)

add_library(${PROJECT_NAME} INTERFACE)

target_sources(${PROJECT_NAME} INTERFACE
        ${INCLUDE_FILES}
        ${SOURCE_FILES})


#[[set(libopencv_core "${CMAKE_CURRENT_SOURCE_DIR}/lib/${ANDROID_ABI}/libopencv_core.a")
add_library(opencv_core STATIC IMPORTED)
set_target_properties(opencv_core PROPERTIES IMPORTED_LOCATION ${libopencv_core})

set(libopencv_highgui "${CMAKE_CURRENT_SOURCE_DIR}/lib/${ANDROID_ABI}/libopencv_highgui.a")
add_library(opencv_highgui STATIC IMPORTED)
set_target_properties(opencv_highgui PROPERTIES IMPORTED_LOCATION ${libopencv_highgui})

set(libopencv_imgproc "${CMAKE_CURRENT_SOURCE_DIR}/lib/${ANDROID_ABI}/libopencv_imgproc.a")
add_library(opencv_imgproc STATIC IMPORTED)
set_target_properties(opencv_imgproc PROPERTIES IMPORTED_LOCATION ${libopencv_imgproc})

set(libopencv_imgcodecs "${CMAKE_CURRENT_SOURCE_DIR}/lib/${ANDROID_ABI}/libopencv_imgcodecs.a")
add_library(opencv_imgcodecs STATIC IMPORTED)
set_target_properties(opencv_imgcodecs PROPERTIES IMPORTED_LOCATION ${libopencv_imgcodecs})

target_link_libraries(${PROJECT_NAME} INTERFACE
        opencv_core
        opencv_highgui
        opencv_imgproc
        opencv_imgcodecs
)]]

# 指定 OpenCV 库所在的路径
set(OPENCV_LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/${ANDROID_ABI}")

# 批量搜索所有 .a 文件
file(GLOB OPENCV_LIBS ${OPENCV_LIB_DIR}/*.a)

# 为每个 .a 文件添加 IMPORTED 库
foreach(lib ${OPENCV_LIBS})
    get_filename_component(lib_name ${lib} NAME_WE)  # 获取库的文件名（不带扩展名）
    add_library(${lib_name} STATIC IMPORTED)
    set_target_properties(${lib_name} PROPERTIES IMPORTED_LOCATION ${lib})
endforeach()

# 将所有的 OpenCV 库链接到目标项目
target_link_libraries(${PROJECT_NAME} INTERFACE ${OPENCV_LIBS})

target_include_directories(${PROJECT_NAME} INTERFACE
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
