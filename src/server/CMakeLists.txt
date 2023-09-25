# 编译静态库，暂不添加任何源码。
add_library(server "")

add_library(webserver "")
add_library(epoller "")


target_sources(webserver
PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}/webserver.cpp
PUBLIC
${CMAKE_CURRENT_SOURCE_DIR}/webserver.h
        )

target_sources(epoller
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/epoller.cpp
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/epoller.h
)

target_link_libraries(webserver
buffer http log pool timer
)
   

target_link_libraries(server
        epoller webserver
         
)

target_include_directories(server
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
