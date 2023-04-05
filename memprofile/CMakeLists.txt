add_library(libmemprofile)

target_sources(libmemprofile
    PRIVATE
    memprofile.h
    malloc_count.h
    
    PUBLIC
    malloc_count.c
)

target_include_directories(libmemprofile
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)


add_executable(memprofile main.cc)
target_link_libraries(memprofile btree libmemprofile)
