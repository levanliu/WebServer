add_library(buffer "")

target_sources(buffer
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/buffer.cpp
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/buffer.h
        )

target_include_directories(buffer
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
