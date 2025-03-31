cmake_minimum_required(VERSION 3.10)
project(MyApp)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找 wxWidgets
find_package(wxWidgets REQUIRED COMPONENTS core base)

# 查找 MariaDB
#find_package(MariaDB REQUIRED)
#find_package(MariaDB REQUIRED)
#pkg_check_modules(mariadb REQUIRED IMPORTED_TARGET "mysqlclient")
set(MariaDB_INCLUDE_DIRS /mingw64/include/mysql)
set(MariaDB_LIBRARIES /mingw64/lib)

# 查找 cURL
find_package(CURL REQUIRED)

# 设置源文件
set(SOURCES
    my.cpp
)

# 添加可执行文件
add_executable(my ${SOURCES})

# 链接 wxWidgets
include(${wxWidgets_USE_FILE})
target_link_libraries(my ${wxWidgets_LIBRARIES} ${CURL_LIBRARIES} ${MariaDB_LIBRARIES})
#target_link_libraries(my PUBLIC PkgConfig::mariadb)

# 包含 wxWidgets 头文件
target_include_directories(my PRIVATE ${wxWidgets_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS} ${MariaDB_INCLUDE_DIRS})

#list(INSERT CMAKE_MODULE_PATH 0 "${CMAKE_CURRENT_SOURCE_DIR}/cmake-modules")
#find_package(MariaDBClient REQUIRED)
#target_link_libraries(
#    my
#    MariaDBClient::MariaDBClient
#)