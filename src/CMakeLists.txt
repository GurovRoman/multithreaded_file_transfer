add_executable(transfer main.cpp
                        routines.h
                        LongOptionParser.cpp LongOptionParser.h)

if (UNIX)
    aux_source_directory(platform/linux PLATFORM_SOURCES)
    target_sources(transfer PRIVATE ${PLATFORM_SOURCES})
    target_include_directories(transfer PRIVATE platform/linux)
    target_link_libraries(transfer pthread)
endif()

if (WIN32)
    aux_source_directory(platform/win32 PLATFORM_SOURCES)
    target_sources(transfer PRIVATE ${PLATFORM_SOURCES})
    target_include_directories(transfer PRIVATE platform/win32)
    target_link_libraries(transfer Ws2_32)
endif()