# Install script for directory: /d/working/vscode-projects/Reference_Project/Gigatron/gigatron-lcc-master

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "RelWithDebInfo")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/mingw64/bin/objdump.exe")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/gigatron-lcc" TYPE EXECUTABLE FILES "/d/working/vscode-projects/Reference_Project/Gigatron/gigatron-lcc-master/build/cpp.exe")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/gigatron-lcc/cpp.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/gigatron-lcc/cpp.exe")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/mingw64/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/gigatron-lcc/cpp.exe")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/gigatron-lcc" TYPE EXECUTABLE FILES "/d/working/vscode-projects/Reference_Project/Gigatron/gigatron-lcc-master/build/rcc.exe")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/gigatron-lcc/rcc.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/gigatron-lcc/rcc.exe")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/mingw64/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/gigatron-lcc/rcc.exe")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/gigatron-lcc" TYPE EXECUTABLE FILES "/d/working/vscode-projects/Reference_Project/Gigatron/gigatron-lcc-master/build/lcc.exe")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/gigatron-lcc/lcc.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/gigatron-lcc/lcc.exe")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/mingw64/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/gigatron-lcc/lcc.exe")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/gigatron-lcc" TYPE EXECUTABLE FILES "/d/working/vscode-projects/Reference_Project/Gigatron/gigatron-lcc-master/build/gtsim.exe")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/gigatron-lcc/gtsim.exe" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/gigatron-lcc/gtsim.exe")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/mingw64/bin/strip.exe" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/gigatron-lcc/gtsim.exe")
    endif()
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/gigatron-lcc" TYPE PROGRAM FILES "/d/working/vscode-projects/Reference_Project/Gigatron/gigatron-lcc-master/build/glcc")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/gigatron-lcc" TYPE PROGRAM FILES "/d/working/vscode-projects/Reference_Project/Gigatron/gigatron-lcc-master/build/glink")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/gigatron-lcc" TYPE PROGRAM FILES "/d/working/vscode-projects/Reference_Project/Gigatron/gigatron-lcc-master/build/glink.py")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/gigatron-lcc" TYPE PROGRAM FILES "/d/working/vscode-projects/Reference_Project/Gigatron/gigatron-lcc-master/build/gt1dump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/gigatron-lcc" TYPE PROGRAM FILES "/d/working/vscode-projects/Reference_Project/Gigatron/gigatron-lcc-master/build/gtprof")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/gigatron-lcc" TYPE FILE FILES "/d/working/vscode-projects/Reference_Project/Gigatron/gigatron-lcc-master/build/interface.json")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/gigatron-lcc" TYPE FILE FILES "/d/working/vscode-projects/Reference_Project/Gigatron/gigatron-lcc-master/build/interface-dev.json")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/gigatron-lcc" TYPE FILE FILES "/d/working/vscode-projects/Reference_Project/Gigatron/gigatron-lcc-master/build/roms.json")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/gigatron-lcc" TYPE FILE FILES "/d/working/vscode-projects/Reference_Project/Gigatron/gigatron-lcc-master/build/glccver.py")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/gigatron-lcc" TYPE DIRECTORY FILES "/d/working/vscode-projects/Reference_Project/Gigatron/gigatron-lcc-master/build/include")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/gigatron-lcc" TYPE DIRECTORY FILES "/d/working/vscode-projects/Reference_Project/Gigatron/gigatron-lcc-master/build/cpu4")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/gigatron-lcc" TYPE DIRECTORY FILES "/d/working/vscode-projects/Reference_Project/Gigatron/gigatron-lcc-master/build/cpu5")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/gigatron-lcc" TYPE DIRECTORY FILES "/d/working/vscode-projects/Reference_Project/Gigatron/gigatron-lcc-master/build/cpu6")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/gigatron-lcc" TYPE DIRECTORY FILES "/d/working/vscode-projects/Reference_Project/Gigatron/gigatron-lcc-master/build/map128k")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/gigatron-lcc" TYPE DIRECTORY FILES "/d/working/vscode-projects/Reference_Project/Gigatron/gigatron-lcc-master/build/map32k")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/gigatron-lcc" TYPE DIRECTORY FILES "/d/working/vscode-projects/Reference_Project/Gigatron/gigatron-lcc-master/build/map512k")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/gigatron-lcc" TYPE DIRECTORY FILES "/d/working/vscode-projects/Reference_Project/Gigatron/gigatron-lcc-master/build/map64k")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/gigatron-lcc" TYPE DIRECTORY FILES "/d/working/vscode-projects/Reference_Project/Gigatron/gigatron-lcc-master/build/mapconx")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/gigatron-lcc" TYPE DIRECTORY FILES "/d/working/vscode-projects/Reference_Project/Gigatron/gigatron-lcc-master/build/mapsim")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  execute_process(COMMAND /usr/bin/cmake.exe -E make_directory
            "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  message(STATUS "Creating symlink: $ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/glcc")
                  execute_process(COMMAND /usr/bin/cmake.exe -E create_symlink
                     "../lib/gigatron-lcc/glcc" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/glcc" )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  message(STATUS "Creating symlink: $ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/glink")
                  execute_process(COMMAND /usr/bin/cmake.exe -E create_symlink
                     "../lib/gigatron-lcc/glink" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/glink" )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  message(STATUS "Creating symlink: $ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/gtsim")
                  execute_process(COMMAND /usr/bin/cmake.exe -E create_symlink
                     "../lib/gigatron-lcc/gtsim" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/gtsim" )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  message(STATUS "Creating symlink: $ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/gtprof")
                  execute_process(COMMAND /usr/bin/cmake.exe -E create_symlink
                     "../lib/gigatron-lcc/gtprof" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/gtprof" )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  message(STATUS "Creating symlink: $ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/gt1dump")
                  execute_process(COMMAND /usr/bin/cmake.exe -E create_symlink
                     "../lib/gigatron-lcc/gt1dump" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/bin/gt1dump" )
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
if(CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/d/working/vscode-projects/Reference_Project/Gigatron/gigatron-lcc-master/build/install_local_manifest.txt"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
if(CMAKE_INSTALL_COMPONENT)
  if(CMAKE_INSTALL_COMPONENT MATCHES "^[a-zA-Z0-9_.+-]+$")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
  else()
    string(MD5 CMAKE_INST_COMP_HASH "${CMAKE_INSTALL_COMPONENT}")
    set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INST_COMP_HASH}.txt")
    unset(CMAKE_INST_COMP_HASH)
  endif()
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  file(WRITE "/d/working/vscode-projects/Reference_Project/Gigatron/gigatron-lcc-master/build/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
endif()
