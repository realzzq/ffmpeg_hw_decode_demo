cmake_minimum_required(VERSION 3.10)

# 引入FFmpeg的头文件
include_directories(${CMAKE_SOURCE_DIR}/cpp/ffmpeg_include)
get_filename_component(PARENT_DIR ${CMAKE_SOURCE_DIR} DIRECTORY)
get_filename_component(PARENT_DIR1 ${PARENT_DIR} DIRECTORY)
# 动态链接库或静态链接库的搜索路径
link_directories(${PARENT_DIR1}/libs/${CMAKE_ANDROID_ARCH_ABI})
message("test: ${PARENT_DIR1}/libs/${CMAKE_ANDROID_ARCH_ABI}")
#找到包含所有的cpp文件
#file(GLOB_RECURSE  allFile "${CMAKE_SOURCE_DIR}/*")
file(GLOB_RECURSE  CPP_HEADER_FILES "${CMAKE_SOURCE_DIR}/cpp/*")

#message("allCpp: ${allCpp}")
message("allFile: ${CPP_HEADER_FILES}")
message("include: ${CMAKE_SOURCE_DIR}/cpp/ffmpeg_include")
message("asd")
add_library(
        # 生成的库的名字
        ffmpegDecode

        # 动态库
        SHARED

#        ${nativeLib}
        # 源文件
        ${CPP_HEADER_FILES} )


target_link_libraries( #目标库
        ffmpegDecode

        # 把ffmpeg的动态库依赖进来
        avformat
        avcodec
        avutil
        avfilter
        swscale
        swresample
        android
        # NDK中的log库
        log )

