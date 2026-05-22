cmake_minimum_required(VERSION 3.15)

# 1. Name your project and language
project(ChessEngineV3 LANGUAGES CXX)

# 2. Set the C++ standard (C++20 is great for modern engines)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 3. Tell CMake where to look for your header (.h) files
include_directories(include)

# 4. Collect all your source (.cpp) files
# For small projects, listing them manually is safest
set(SOURCES 
    src/main.cpp
    src/bitboard/bitboard.cpp
    src/board/board.cpp
    src/movegen/attacks.cpp
    src/movegen/generateattacks.cpp
)

# 5. Define the final executable output
add_executable(${PROJECT_NAME} ${SOURCES})