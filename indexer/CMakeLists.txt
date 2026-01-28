cmake_minimum_required(VERSION 3.10)
project(indexer)

set(CMAKE_CXX_STANDARD 17)

add_executable(search_service search_service.cpp)
target_include_directories(search_service PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(search_service PRIVATE inverted_index)
add_subdirectory(inverted_index)
add_subdirectory(merkle_diff)
add_subdirectory(stemmer)
add_subdirectory(tokenizer) 