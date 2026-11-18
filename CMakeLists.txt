cmake_minimum_required(VERSION 3.16)
project(CinemaMecha VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(APP_TARGET CinemaMecha)

# Em geradores de uma unica configuracao (Makefile/Ninja) o padrao e Release.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Tipo de build (Release ou Debug)" FORCE)
endif()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# ---------------------------------------------------------------- dependencias
find_package(Qt6 COMPONENTS Widgets QUIET)
if(Qt6_FOUND)
    set(QT_WIDGETS_TARGET Qt6::Widgets)
else()
    find_package(Qt5 COMPONENTS Widgets REQUIRED)
    set(QT_WIDGETS_TARGET Qt5::Widgets)
endif()
find_package(Threads REQUIRED) # o console de admin usa std::thread

# ------------------------------------------------------------------- fontes
# CONFIGURE_DEPENDS: arquivos novos entram sem precisar rodar o CMake a mao.
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/*.cpp")
file(GLOB_RECURSE HEADERS CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/*.h")
# Resquicio da estrutura antiga: o main.cpp agora fica em src/. Se sobrou uma copia em
# src/classes/, ela e ignorada para nao gerar dois main().
list(FILTER SOURCES EXCLUDE REGEX "/src/classes/main\\.cpp$")
# main.cpp e listado explicitamente: se faltar, o CMake avisa na configuracao.
list(APPEND SOURCES "${PROJECT_SOURCE_DIR}/src/main.cpp")
list(REMOVE_DUPLICATES SOURCES)
set(RESOURCES "${PROJECT_SOURCE_DIR}/src/recursos.qrc")

# Sem WIN32 de proposito: o console de admin (stdin/stdout) precisa da janela de terminal.
add_executable(${APP_TARGET} ${SOURCES} ${HEADERS} ${RESOURCES})

target_include_directories(${APP_TARGET} PRIVATE
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/include/classes"
    "${PROJECT_SOURCE_DIR}/include/visual"
)

target_link_libraries(${APP_TARGET} PRIVATE ${QT_WIDGETS_TARGET} Threads::Threads)

# ------------------------------------------------------------ opcoes de build
if(MSVC)
    # /utf-8: o fonte esta em UTF-8 e tem acentos.
    target_compile_options(${APP_TARGET} PRIVATE /utf-8 /W4
        $<$<CONFIG:Release>:/Gy> $<$<CONFIG:Release>:/Zc:inline>)
    target_link_options(${APP_TARGET} PRIVATE $<$<CONFIG:Release>:/OPT:REF> $<$<CONFIG:Release>:/OPT:ICF>)
else()
    target_compile_options(${APP_TARGET} PRIVATE -Wall -Wextra
        $<$<CONFIG:Release>:-ffunction-sections> $<$<CONFIG:Release>:-fdata-sections>)
    if(APPLE)
        # O linker da Apple nao conhece --gc-sections nem -s.
        target_link_options(${APP_TARGET} PRIVATE $<$<CONFIG:Release>:-Wl,-dead_strip>)
    else()
        target_link_options(${APP_TARGET} PRIVATE $<$<CONFIG:Release>:-s> $<$<CONFIG:Release>:-Wl,--gc-sections>)
    endif()
endif()

# ------------------------------------------- empacotamento portatil (Windows)
if(WIN32)
    if(Qt6_FOUND)
        get_target_property(_qmake_loc Qt6::qmake IMPORTED_LOCATION)
    else()
        get_target_property(_qmake_loc Qt5::qmake IMPORTED_LOCATION)
    endif()

    if(_qmake_loc)
        get_filename_component(_qt_bin_dir "${_qmake_loc}" DIRECTORY)
        find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")
    endif()

    if(WINDEPLOYQT_EXECUTABLE)
        # So o MSVC tem DLLs do Qt separadas para Debug; no MinGW os binarios sao unicos
        # (qwindows.dll, sem sufixo "d"), e passar --debug faz o windeployqt falhar.
        if(MSVC)
            set(DEPLOY_CONFIG_FLAG $<IF:$<CONFIG:Debug>,--debug,--release>)
        else()
            set(DEPLOY_CONFIG_FLAG --release)
        endif()

        # Copia as DLLs do Qt ao lado do executavel.
        add_custom_command(TARGET ${APP_TARGET} POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                    ${DEPLOY_CONFIG_FLAG}
                    --no-opengl-sw --no-translations
                    "$<TARGET_FILE:${APP_TARGET}>"
            COMMENT "Empacotando dependencias do Qt..."
            VERBATIM
        )
    endif()
endif()

# ------------------------------------------------------------------- install
# Instala apenas o executavel (e, no Windows, as DLLs do Qt) - nunca o diretorio de build.
install(TARGETS ${APP_TARGET} RUNTIME DESTINATION .)
if(WIN32 AND WINDEPLOYQT_EXECUTABLE)
    install(CODE "
        execute_process(COMMAND \"${WINDEPLOYQT_EXECUTABLE}\" --release
                        --no-opengl-sw --no-translations
                        \"\${CMAKE_INSTALL_PREFIX}/${APP_TARGET}.exe\")
    ")
endif()

set(CPACK_GENERATOR "ZIP")
set(CPACK_PACKAGE_FILE_NAME "CinemaMecha-Portable")
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY ON) # o zip extrai em uma pasta CinemaMecha-Portable/
include(CPack)
