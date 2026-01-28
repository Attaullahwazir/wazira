cmake_minimum_required(VERSION 3.10)
project(crawler)

find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

add_subdirectory(p2p_dht)
add_subdirectory(content_store)
add_subdirectory(merkle_tree)
add_subdirectory(crawler)
add_subdirectory(cli)
add_subdirectory(${CMAKE_SOURCE_DIR}/indexer/tokenizer ${CMAKE_BINARY_DIR}/tokenizer)
add_subdirectory(${CMAKE_SOURCE_DIR}/indexer/stemmer ${CMAKE_BINARY_DIR}/stemmer)
add_subdirectory(${CMAKE_SOURCE_DIR}/indexer/inverted_index ${CMAKE_BINARY_DIR}/inverted_index)