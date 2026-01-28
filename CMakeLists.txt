cmake_minimum_required(VERSION 3.16)

project(inventory LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(inventory main.cpp
    item.h item.cpp
    inventory.h inventory.cpp
    player.cpp
    player.h
    game.cpp
    game.h
    singleCharacter.h
)

include(GNUInstallDirs)
install(TARGETS inventory
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
