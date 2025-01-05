add_executable(Emulator 
    main.cpp
    mapper.cpp
    rom.cpp
    )
    
target_include_directories(Emulator PUBLIC ${PROJECT_SOURCE_DIR}/NES/include)
target_link_libraries(Emulator debugger)
target_link_libraries(Emulator mos6502)