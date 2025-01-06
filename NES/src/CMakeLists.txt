
add_library(nes
    MOS6502.cpp
    mapper.cpp
    rom.cpp
)
target_include_directories(nes PUBLIC ${PROJECT_SOURCE_DIR}/NES/include)
target_link_libraries(nes debugger)