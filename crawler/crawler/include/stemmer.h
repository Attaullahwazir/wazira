#pragma once
#include <string>

#if __has_include(<libstemmer.h>)
#include <libstemmer.h>
#endif

/**
 * @class Stemmer
 * @brief Pluggable word stemmer (Snowball or language-specific).
 */
class Stemmer {
public:
    Stemmer(const std::string& language = "english");
    ~Stemmer();
    std::string stem(const std::string& word) const;
private:
    std::string language_;
#if __has_include(<libstemmer.h>)
    struct sb_stemmer* sb_stemmer_ = nullptr;
#endif
}; 