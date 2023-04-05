add_library(btree)

target_sources(btree
    PRIVATE
    btree.h 
    btree_set.h 
    btree_map.h
    btree_multimap.h
    btree_multiset.h
    
    PUBLIC
    btree.cpp
    btree_set.cpp 
    btree_map.cpp
    btree_multimap.cpp
    btree_multiset.cpp
)

target_link_libraries(btree)

target_include_directories(btree
    PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

if (CMAKE_CXX_COMPILER_ID MATCHES GNU)
  target_compile_options(oss
      PUBLIC
        "--coverage"
  )
  target_link_options(oss
      INTERFACE
        "--coverage"
  )
endif()
