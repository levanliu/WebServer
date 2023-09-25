# 编译静态库，暂不添加任何源码。
add_library(http "")

add_library(httpconn "")
add_library(httprequest "")
add_library(httpresponse "")

target_sources(httpconn
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/httpconn.cpp
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/httpconn.h
        )
target_sources(httprequest
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/httprequest.cpp
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/httprequest.h
        )

target_sources(httpresponse
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/httpresponse.cpp
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/httpresponse.h
        )

target_link_libraries(httpconn
        buffer log pool epoller timer
)

target_link_libraries(httprequest
        buffer log pool epoller timer
)

target_link_libraries(httpresponse
        buffer log pool epoller timer
)

target_link_libraries(http
   httpconn httprequest httpresponse
)

# 增加头文件目录
target_include_directories(http
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
