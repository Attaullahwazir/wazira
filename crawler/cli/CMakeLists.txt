set(CMAKE_VERBOSE_MAKEFILE ON)
add_executable(p2p_crawler main.cpp)
target_include_directories(p2p_crawler PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ../crawler)
target_link_libraries(p2p_crawler PRIVATE
    crawler
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