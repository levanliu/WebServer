# 将main.cpp编译成可执行文件
add_executable(main main.cpp)

set(srcDirs
  buffer
  http
  log
  pool
  server
  timer
)

foreach(CURRENT_FOLDER ${srcDirs})
        add_subdirectory(${CURRENT_FOLDER})
endforeach()

target_link_libraries(main
PRIVATE 
buffer
http
log
pool
server
timer
)