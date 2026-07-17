find_package(PkgConfig REQUIRED)
find_package(yaml-cpp REQUIRED)
pkg_check_modules(LIBSSH2 REQUIRED libssh2)
 
set(SOURCES
    main.cpp
    LocalExecutor.cpp
    RemoteSSHExecutor.cpp
    ShellTechnique.cpp
    FileModificationTechnique.cpp
    NetworkScanTechnique.cpp
    ProcessEnumTechnique.cpp
    UserEnumTechnique.cpp
    TechniqueFactory.cpp
    PlaybookEngine.cpp
    HtmlReporter.cpp
)
 
add_executable(sentinel-red ${SOURCES})
 
target_include_directories(sentinel-red
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        ${LIBSSH2_INCLUDE_DIRS}
)
 
target_link_libraries(sentinel-red
    PRIVATE
        yaml-cpp
        ${LIBSSH2_LIBRARIES}
)
 
target_compile_options(sentinel-red
    PRIVATE
        ${LIBSSH2_CFLAGS_OTHER}
        -Wall -Wextra
)
 
