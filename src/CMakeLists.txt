add_executable(Emulator 
    main.cpp
    )
    
target_link_libraries(Emulator debugger)
target_link_libraries(Emulator nes)