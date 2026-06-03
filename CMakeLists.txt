cmake_minimum_required(VERSION 3.15)

# 1. Name your project and language
project(ChessEngineV3 LANGUAGES CXX)


# Force CMake to generate the file VS Code is asking for
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 2. Set the C++ standard (C++20 is great for modern engines)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 3. Collect all your source (.cpp) files
set(SOURCES 
    src/main.cpp
    src/bitboard/bitboard.cpp
    src/board/board.cpp
    src/movegen/attacks.cpp
    src/movegen/generateattacks.cpp
    src/move/move.cpp
    src/bitboard/perft.cpp
    src/evaluation/evaluation.cpp
    src/search/search.cpp
    src/uci/uci.cpp
    src/board/zobrist.cpp
)

# 4. Define the final executable output using the project name macro
add_executable(${PROJECT_NAME} ${SOURCES})

# 5. Include your local headers directory
target_include_directories(${PROJECT_NAME} PRIVATE include)