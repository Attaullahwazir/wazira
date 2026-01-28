add_library(crawler
    crawler.cpp
    # Add other .cpp files here if needed
)
target_include_directories(crawler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

add_subdirectory(../p2p_dht p2p_dht)
add_subdirectory(../content_store content_store)
add_subdirectory(../merkle_tree merkle_tree)
add_subdirectory(../cli cli)

add_subdirectory(../indexer/tokenizer tokenizer)
add_subdirectory(../indexer/stemmer stemmer)
add_subdirectory(../indexer/inverted_index inverted_index)

add_executable(p2p_crawler ../cli/main.cpp) # Adjust path if needed

target_link_libraries(crawler PUBLIC
    p2p_dht
    content_store
    merkle_tree
    tokenizer
    stemmer
    inverted_index
    leveldb
    OpenSSL::SSL
    OpenSSL::Crypto
    CURL::libcurl
)

target_link_libraries(p2p_crawler PRIVATE crawler)