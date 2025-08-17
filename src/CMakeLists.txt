add_executable(Gody_Art main.cpp "Editor.h" "Editor.cpp" "Drawing.h" "Drawing.cpp" "Util.h" "Util.cpp")
target_link_libraries(Gody_Art PRIVATE ImGui-SFML::ImGui-SFML)

# Copy DLLs needed for runtime on Windows
if(WIN32)
    add_custom_command(TARGET Gody_Art POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy_if_different
          $<TARGET_FILE:ImGui-SFML::ImGui-SFML>
          $<TARGET_FILE:sfml-graphics>
          $<TARGET_FILE:sfml-window>
          $<TARGET_FILE:sfml-system>
          $<TARGET_FILE_DIR:Gody_Art>)
endif()