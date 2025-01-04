add_executable(Emulator 
    main.cpp
    mapper.cpp
    MOS6502.cpp
    rom.cpp
    )
    
target_include_directories(Emulator PUBLIC ${PROJECT_SOURCE_DIR}/NES/include)
target_link_libraries(Emulator other_lib)