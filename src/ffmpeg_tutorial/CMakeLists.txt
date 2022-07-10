if(NOT FFMPEG_FOUND)
  message(FATAL_ERROR "FFMPEG not found")
else()
  message(STATUS "Found FFMPEG libraries:" "${FFMPEG_LIBRARIES}")
  message(STATUS "Found FFMPEG library dirs:" "${FFMPEG_LIBRARY_DIRS}")
  message(STATUS "Found opencv libraries:" "${OpenCV_LIBS}")
endif()

add_executable(ffmpeg_tutorial main.cpp)
set_target_properties(ffmpeg_tutorial PROPERTIES LINKER_LANGUAGE CXX)
target_include_directories(ffmpeg_tutorial PRIVATE
    "${PROJECT_SOURCE_DIR}/src/"
    ${FFMPEG_INCLUDE_DIRS}
    fmt::fmt)


target_link_libraries(ffmpeg_tutorial 
  PRIVATE 
    dashcam_utils
    fmt::fmt
    ${OpenCV_LIBS}
    ${FFMPEG_LIBRARIES}
    )
    
add_custom_command(TARGET ffmpeg_tutorial POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:ffmpeg_tutorial> $<TARGET_FILE_DIR:ffmpeg_tutorial>
    COMMAND_EXPAND_LISTS
    )