cmake_minimum_required(VERSION 3.5)

project(Multi_Level_Authentication_pairFree_Client LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC_SEARCH_PATHS ${CMAKE_SOURCE_DIR}/ui)
set(OPENSSL_ROOT_DIR /usr/local/openssl)
set(OPENSSL_LIBRARIES /usr/local/openssl/lib64)
set(OPENSSL_INCLUDE_DIR /usr/local/openssl/include)

find_package(Qt5 COMPONENTS Widgets LinguistTools Network REQUIRED)

set(TS_FILES Multi_Level_Authentication_pairFree_Client_zh_CN.ts)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${QT_PATH}/include
)

link_libraries(
    ${PROJECT_SOURCE_DIR}/lib
    ${QT_PATH}/lib
)

FILE(GLOB ${PROJECT_NAME}_HEADER "include/*.h")
FILE(GLOB ${PROJECT_NAME}_SOURCES "src/*.cpp")
FILE(GLOB ${PROJECT_NAME}_UI "ui/*.ui")

add_executable(${PROJECT_NAME}
        #"qrc/ND-GUI.rc"
        #${${PROJECT_NAME}_QRC}
        ${${PROJECT_NAME}_UI}
        ${${PROJECT_NAME}_SOURCES}
        ${${PROJECT_NAME}_HEADER}
        )


target_link_libraries(Multi_Level_Authentication_pairFree_Client PRIVATE Qt5::Widgets Qt5::Network)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Widgets
    ${OPENSSL_LIBRARIES}/libcrypto.so

    libjsoncpp.so
    libgmp.so
    libgmpxx.so
    )

qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
