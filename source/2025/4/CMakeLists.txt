add_executable(2025_4.exe main.c)
target_include_directories(
    2025_4.exe
    PUBLIC
    ${CMAKE_SOURCE_DIR}/include
)
set_target_properties(2025_4.exe PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
