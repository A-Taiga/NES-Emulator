find_package(SDL2 REQUIRED)
find_package(OpenGL REQUIRED)

add_library(debugger
    ../imgui/imgui.cpp
    ../imgui/imgui_demo.cpp
    ../imgui/imgui_draw.cpp
    ../imgui/imgui_tables.cpp
    ../imgui/imgui_widgets.cpp
    ../imgui/backends/imgui_impl_sdl2.cpp
    ../imgui/backends/imgui_impl_opengl3.cpp
    
    window.cpp
    debugger.cpp
    hex_editor.cpp
)

target_include_directories(debugger PUBLIC 
    ${PROJECT_SOURCE_DIR}/debugger/include
    ${SDL2_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIR}
    ${PROJECT_SOURCE_DIR}/debugger/imgui
    ${PROJECT_SOURCE_DIR}/debugger/imgui/backends
    ${PROJECT_SOURCE_DIR}/debugger
)

target_link_libraries(debugger PUBLIC SDL2main SDL2)
target_link_libraries(debugger PUBLIC GL)
target_link_libraries(debugger PUBLIC nes)