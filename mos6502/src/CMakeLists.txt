add_library (mos6502
    MOS6502.cpp
)

target_include_directories(mos6502 PUBLIC
    ${PROJECT_SOURCE_DIR}/mos6502/include
)