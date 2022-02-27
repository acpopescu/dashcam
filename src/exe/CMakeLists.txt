add_executable(dashcam main.cpp)
target_include_directories(dashcam PRIVATE
    "${PROJECT_SOURCE_DIR}/src/"
    ${OpenCV_INCLUDE_DIRS}
    fmt::fmt)
    
target_link_libraries(dashcam 
  PRIVATE 
    fmt::fmt
    ${OpenCV_LIBS}
    dashcam_utils)
    
add_custom_command(TARGET dashcam POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:dashcam> $<TARGET_FILE_DIR:dashcam>
    COMMAND_EXPAND_LISTS
    )