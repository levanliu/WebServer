add_library(pool "")
add_library(sqlconnpool "")
add_library(sqlconnRAII "")

IF (MYSQL_INCLUDE_DIR)
  # Already in cache, be silent
  SET(MYSQL_FIND_QUIETLY TRUE)
ENDIF (MYSQL_INCLUDE_DIR)

FIND_PATH(MYSQL_INCLUDE_DIR mysql.h
  /usr/local/include/mysql
  /usr/include/mysql
)

SET(MYSQL_NAMES mysqlclient mysqlclient_r)
FIND_LIBRARY(MYSQL_LIBRARY
  NAMES ${MYSQL_NAMES}
  PATHS /usr/lib /usr/local/lib
  PATH_SUFFIXES mysql
)

IF (MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)
  SET(MYSQL_FOUND TRUE)
  SET( MYSQL_LIBRARIES ${MYSQL_LIBRARY} )
ELSE (MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)
  SET(MYSQL_FOUND FALSE)
  SET( MYSQL_LIBRARIES )
ENDIF (MYSQL_INCLUDE_DIR AND MYSQL_LIBRARY)

IF (MYSQL_FOUND)
  IF (NOT MYSQL_FIND_QUIETLY)
    MESSAGE(STATUS "Found MySQL: ${MYSQL_LIBRARY}")
  ENDIF (NOT MYSQL_FIND_QUIETLY)
ELSE (MYSQL_FOUND)
  IF (MYSQL_FIND_REQUIRED)
    MESSAGE(STATUS "Looked for MySQL libraries named ${MYSQL_NAMES}.")
    MESSAGE(FATAL_ERROR "Could NOT find MySQL library")
  ENDIF (MYSQL_FIND_REQUIRED)
ENDIF (MYSQL_FOUND)

MARK_AS_ADVANCED(
  MYSQL_LIBRARY
  MYSQL_INCLUDE_DIR
  )
include_directories(${MYSQL_INCLUDE_DIR})

target_sources(sqlconnpool
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/sqlconnpool.cpp
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/sqlconnpool.h
        )

target_sources(sqlconnRAII
PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}/sqlconnRAII.cpp
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/sqlconnRAII.h
        )
        
target_link_libraries(pool 
  sqlconnpool sqlconnRAII ${MYSQL_LIBRARY} buffer http log timer
)

target_include_directories(pool 
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)
