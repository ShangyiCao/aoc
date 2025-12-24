add_executable(2025_1.exe 1.c)

add_executable(2025_2.exe 2.c)

add_executable(2025_3.exe 3.c)

add_executable(2025_4.exe 4.c)
target_include_directories(2025_4.exe PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(2025_5.exe 5.c)
target_include_directories(2025_5.exe PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(2025_6.exe 6.c)
target_include_directories(2025_6.exe PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(2025_7.exe 7.c)
target_include_directories(2025_7.exe PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(2025_8.exe 8.c)
target_include_directories(2025_8.exe PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(2025_9.exe 9.c)

add_executable(2025_10.exe 10.c)
find_library(GLPK_LIB glpk REQUIRED)
find_path(GLPK_INCLUDE_DIR glpk.h REQUIRED)
target_include_directories(2025_10.exe PRIVATE ${CMAKE_SOURCE_DIR}/include ${GLPK_INCLUDE_DIR})
target_link_libraries(2025_10.exe PRIVATE ${GLPK_LIB})

add_executable(2025_11.exe 11.c)
target_include_directories(2025_11.exe PRIVATE ${CMAKE_SOURCE_DIR}/include)

add_executable(2025_12.exe 12.c)