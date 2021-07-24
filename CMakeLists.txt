# CMakeList.txt: проект CMake для hw6; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#
cmake_minimum_required (VERSION 3.8)

if(DEFINED ENV{GITHUB_RUN_NUMBER})
    project ("bulk_server" VERSION 0.0.$ENV{GITHUB_RUN_NUMBER})
else ()
    project ("bulk_server")
endif()

if (UNIX)
    find_package (Threads)
endif()

find_package(Boost COMPONENTS system  REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

# Добавьте источник в исполняемый файл этого проекта.
add_executable (bulk_server "main.cpp" "server.cpp" "command.cpp" )

target_link_libraries(bulk_server
        ${Boost_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        ) 

if (UNIX)
    target_link_libraries (bulk_server Threads::Threads)
endif()

if (MSVC)
    target_compile_options(bulk_server PUBLIC
        /W4
    )
else ()

    target_compile_options(bulk_server PUBLIC
        -Wall -Wextra -pedantic -Werror
    )
endif()

set_target_properties(bulk_server PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
install(TARGETS bulk_server RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)
