# 将main.cpp编译成可执行文件
add_executable(webServer main.cpp)

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
       
        target_link_libraries(server
                PRIVATE ${CURRENT_FOLDER})

endforeach()

