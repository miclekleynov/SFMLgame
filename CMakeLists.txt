cmake_minimum_required(VERSION 3.21)

# ——————————————————————————————————————————————————————————————
# Project definition
# ——————————————————————————————————————————————————————————————
project(SFMLgame
        VERSION 0.1.0
        DESCRIPTION "Excavation 3d game"
        LANGUAGES CXX)

# ——————————————————————————————————————————————————————————————
# Compiler settings
# ——————————————————————————————————————————————————————————————
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/W4 /WX)
else ()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif ()

# ——————————————————————————————————————————————————————————————
# Fetch and integrate nlohmann/json
# ——————————————————————————————————————————————————————————————
include(FetchContent)
FetchContent_Declare(
        nlohmann_json
        GIT_REPOSITORY https://github.com/nlohmann/json.git
        GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(nlohmann_json)

# ——————————————————————————————————————————————————————————————
# Fetch and integrate SFML 2.6.2
# ——————————————————————————————————————————————————————————————
FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 2.6.2
)
FetchContent_MakeAvailable(SFML)

# ——————————————————————————————————————————————————————————————
# Sources
# ——————————————————————————————————————————————————————————————
file(GLOB_RECURSE
        ALL_SOURCES
        CONFIGURE_DEPENDS
        "${PROJECT_SOURCE_DIR}/src/*.cpp"
        "${PROJECT_SOURCE_DIR}/src/*.h"
)

# ——————————————————————————————————————————————————————————————
# Executable target
# ——————————————————————————————————————————————————————————————
add_executable(${PROJECT_NAME} ${ALL_SOURCES})

target_include_directories(${PROJECT_NAME}
        PRIVATE
        "${PROJECT_SOURCE_DIR}/src"
)

# ——————————————————————————————————————————————————————————————
# Link libraries
# ——————————————————————————————————————————————————————————————
target_link_libraries(${PROJECT_NAME}
        PRIVATE
        nlohmann_json::nlohmann_json
        sfml-graphics
        sfml-window
        sfml-system
)

# Копируем все зависимые DLL в папку с exe (Windows)
if (WIN32)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_RUNTIME_DLLS:${PROJECT_NAME}>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMAND_EXPAND_LISTS
            COMMENT "Copying runtime DLLs next to the executable")
endif()

# ——————————————————————————————————————————————————————————————
# Copy content folder after build
# ——————————————————————————————————————————————————————————————
add_custom_command(TARGET ${PROJECT_NAME}
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${PROJECT_SOURCE_DIR}/content"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/content"
        COMMENT "Copying content folder to working directory")

# ---- MinGW runtime DLLs (если собираем GCC/MinGW) ----
if (MINGW)
    # Папка, где лежит g++.exe и нужные DLL
    get_filename_component(MINGW_BIN_DIR "${CMAKE_CXX_COMPILER}" DIRECTORY)

    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${MINGW_BIN_DIR}/libstdc++-6.dll"
            "${MINGW_BIN_DIR}/libgcc_s_seh-1.dll"
            "${MINGW_BIN_DIR}/libwinpthread-1.dll"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMENT "Copying MinGW runtime DLLs")
endif()
