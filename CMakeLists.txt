cmake_minimum_required(VERSION 3.10)
project(task5)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Исходные файлы
include_directories(${PROJECT_SOURCE_DIR}/include)

# Основной исполняемый файл
add_executable(main
    main.cpp
)

# Google Test
find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

# Тесты
add_executable(run_tests
    test/tests01.cpp
)

target_link_libraries(run_tests
    ${GTEST_LIBRARIES}
    pthread
)

# Включаем тестирование
enable_testing()
add_test(NAME FigureTests COMMAND run_tests)