cmake_minimum_required(VERSION 3.16)
project(CinemaAutoatendimento LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Escolha o tipo de build (Release ou Debug)" FORCE)
endif()

if(MSVC)
    # /utf-8: o fonte esta em UTF-8 e tem acentos ("Ficção", "Padrão").
    add_compile_options(/utf-8)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /Oi /Gy /Zc:inline")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /OPT:REF /OPT:ICF")
else()
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -ffunction-sections -fdata-sections")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} -s -Wl,--gc-sections")
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Widgets QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 COMPONENTS Widgets REQUIRED)
endif()

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.cc")
file(GLOB_RECURSE HEADERS "include/*.h" "include/*.hpp")

set(RESOURCES "src/recursos.qrc")

add_executable(CinemaAutoatendimento 
    ${SOURCES} 
    ${HEADERS} 
    ${RESOURCES}
)

target_include_directories(CinemaAutoatendimento PRIVATE
    include
    include/classes
    include/visual
)

if(WIN32)
    target_link_libraries(CinemaAutoatendimento PRIVATE dwmapi)
endif()

if(Qt6_FOUND)
    target_link_libraries(CinemaAutoatendimento PRIVATE Qt6::Widgets)
else()
    target_link_libraries(CinemaAutoatendimento PRIVATE Qt5::Widgets)
endif()

if(EXISTS "${CMAKE_SOURCE_DIR}/src/assets")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/src/assets"
        "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets"
        COMMENT "Copiando pasta de assets..."
    )
endif()

if(WIN32)
    if(Qt6_FOUND)
        get_target_property(_qmake_loc Qt6::qmake IMPORTED_LOCATION)
    else()
        get_target_property(_qmake_loc Qt5::qmake IMPORTED_LOCATION)
    endif()

    if(_qmake_loc)
        get_filename_component(_qt_bin_dir "${_qmake_loc}" DIRECTORY)
        find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

        if(WINDEPLOYQT_EXECUTABLE)
            add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND "${CMAKE_COMMAND}" -E echo "Executando windeployqt enxuto..."
                COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                        --release
                        --no-compiler-runtime
                        --no-opengl-sw
                        --no-translations
                        "$<TARGET_FILE:${PROJECT_NAME}>"
                COMMENT "Enxugando e empacotando dependências do Qt..."
                VERBATIM
            )
        endif()
    endif()
endif()

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION .)
install(DIRECTORY "$<TARGET_FILE_DIR:${PROJECT_NAME}>/"
    DESTINATION .
    PATTERN "*.pdb" EXCLUDE
    PATTERN "*.ilk" EXCLUDE
    PATTERN "*.exp" EXCLUDE
    PATTERN "*.lib" EXCLUDE
)
set(CPACK_GENERATOR "ZIP")
set(CPACK_PACKAGE_FILE_NAME "CinemaAutoatendimento-Portable")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
include(CPack)