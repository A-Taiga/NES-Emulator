add_library(other_lib
    window.cpp
)
target_include_directories(other_lib PUBLIC ${PROJECT_SOURCE_DIR}/debugger/include)
target_link_libraries(other_lib SDL2main SDL2)