cmake_minimum_required(VERSION 3.28)

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

if (TEST)
  add_subdirectory(c-testing-and-mocking)

  project(project-test C)

  add_library(common INTERFACE)

  target_compile_definitions(common INTERFACE
    -D_FORTIFY_SOURCE=2
  )

  target_include_directories(common INTERFACE
    .
    test
  )

  target_compile_options(common INTERFACE
    -g
    -std=c99
    -Wall
    -Werror
    #-Wconversion
    #-Wsign-conversion
    -Wformat-security
    #-pedantic
    -ftrapv
    -fstack-protector-all
    --param ssp-buffer-size=4
    -fstack-clash-protection
  )

  target_link_libraries(common INTERFACE
    c-testing-and-mocking
  )

  add_executable(app-test
    test/avr_mock.c

    test/app_tc.c

    app.c
  )

  target_link_libraries(app-test PUBLIC common)

  add_custom_command(
    TARGET app-test
    POST_BUILD
    COMMAND app-test
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    VERBATIM
  )

else()
  set(F_CPU 1000000UL)

  # CPU, you can find the list here:
  # https://gcc.gnu.org/onlinedocs/gcc/AVR-Options.html
  set(MCU_GCC attiny45)

  # Part for avrdude
  # https://avrdudes.github.io/avrdude/8.0/avrdude_45.html#List-of-Parts
  set(MCU_DUDE t45)

  # ISR names:
  # https://www.nongnu.org/avr-libc/user-manual/group__avr__interrupts.html

  # The programmer to use, read avrdude manual for list
  set(PROG_TYPE avrispmkII)

  # AVR Fuses, must be in concordance with your hardware and F_CPU
  # https://www.engbedded.com/fusecalc/
  set(L_FUSE 0x62)
  set(H_FUSE 0xDF)

  set(CMAKE_SYSTEM_NAME Generic)
  set(CMAKE_SYSTEM_PROCESSOR avr)
  set(CMAKE_CXX_COMPILER avr-g++)
  set(CMAKE_C_COMPILER avr-gcc)
  set(CMAKE_ASM_COMPILER avr-gcc)

  set(AVR_LIBC_PROJECT_PATH ${CMAKE_SOURCE_DIR}/avr-libc)
  set(AVR_LIBC_PATH ${AVR_LIBC_PROJECT_PATH}/install-dir)

  project(app C)

  add_executable(app EXCLUDE_FROM_ALL
    avr.c
    app.c
    main.c

    ${AVR_LIBC_PATH}/avr/include/stdlib.h
  )

  target_compile_definitions(app PUBLIC
    -DF_CPU=${F_CPU}
  )

  # building avr-libc: https://avrdudes.github.io/avr-libc/avr-libc-user-manual/install_tools.html
  target_include_directories(app PUBLIC
    ${AVR_LIBC_PATH}/avr/include
    ${AVR_LIBC_PATH}/include
  )

  target_compile_options(app PUBLIC
    -mmcu=${MCU_GCC}
    -std=c99
    -Os
    -flto
    -Wall
    -Wundef
    -pedantic
    -Werror
    -Wl,--relax,--gc-sections
    -fpack-struct
    -fshort-enums
    -ffunction-sections
    -fdata-sections
  )

  target_link_options(app PUBLIC
    -mmcu=${MCU_GCC}
    --sysroot=${AVR_LIBC_PATH}/avr/
  )

  set_target_properties(app PROPERTIES OUTPUT_NAME app.elf)

  add_custom_command(TARGET app POST_BUILD
    DEPENDS $<TARGET_FILE:app>
    COMMAND avr-size -C $<TARGET_FILE:app> --mcu=${MCU_GCC}
  )

  add_custom_command(
    OUTPUT ${AVR_LIBC_PATH}/avr/include/stdlib.h
    DEPENDS build-avr-libc
  )

  add_custom_command(
    OUTPUT ${AVR_LIBC_PATH}/bin/avr-man # this is the last built file of the avr-libc building process
    COMMAND ./bootstrap && ./configure --prefix=${AVR_LIBC_PATH} --build=x86_64-pc-linux-gnu --host=avr && make -j$(nproc) && make install -j$(nproc)
    WORKING_DIRECTORY ${AVR_LIBC_PROJECT_PATH}
  )
  add_custom_target(build-avr-libc DEPENDS ${AVR_LIBC_PATH}/bin/avr-man)

  add_custom_target(hex ALL avr-objcopy -R .eeprom -O ihex app.elf app.hex DEPENDS app)

  add_custom_target(upload avrdude -c ${PROG_TYPE} -p ${MCU_DUDE} -U flash:w:app.hex DEPENDS hex)

  add_custom_target(upload-eeprom avrdude -c ${PROG_TYPE} -p ${MCU}  -U eeprom:w:eeprom.hex:i)
  add_custom_target(download-eeprom avrdude -c ${PROG_TYPE} -p ${MCU}  -U eeprom:r:readback.hex:i)

  add_custom_target(fuses avrdude -c ${PROG_TYPE} -p ${MCU_DUDE} -U lfuse:w:${L_FUSE}:m -U hfuse:w:${H_FUSE}:m)

endif()
