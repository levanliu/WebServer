# 编译静态库，暂不添加任何源码。
add_library(timer "")

target_sources(timer
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/heaptimer.cpp
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/heaptimer.h
        )

target_include_directories(timer
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
