add_library(log "")

target_sources(log
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/log.cpp
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/log.h
        )

add_library(blockqueue "")

target_sources(blockqueue
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/blockqueue.h
        )

target_link_libraries(blockqueue
        buffer http timer pool
)

target_link_libraries(log
        blockqueue    
)

target_include_directories(log
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)