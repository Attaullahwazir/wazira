#include "stemmer.h"
#include <string>
#include <algorithm>

// Try to include Snowball stemmer
#if __has_include(<libstemmer.h>)
#include <libstemmer.h>
#define SNOWBALL_AVAILABLE 1
#else
#define SNOWBALL_AVAILABLE 0
#endif

/**
 * @brief Construct a Stemmer for a given language (default: English).
 */
Stemmer::Stemmer(const std::string& language) : language_(language) {
#if SNOWBALL_AVAILABLE
    sb_stemmer_ = sb_stemmer_new(language.c_str(), NULL);
    if (!sb_stemmer_) {
        // Fallback to English if language not supported
        sb_stemmer_ = sb_stemmer_new("english", NULL);
    }
#endif
}

/**
 * @brief Stem a word (basic English stemming, placeholder for Snowball integration).
 */
std::string Stemmer::stem(const std::string& word) const {
#if SNOWBALL_AVAILABLE
    if (sb_stemmer_) {
        const sb_symbol* stemmed = sb_stemmer_stem(sb_stemmer_, (const sb_symbol*)word.c_str(), word.size());
        if (stemmed) {
            return std::string((const char*)stemmed, sb_stemmer_length(sb_stemmer_));
        }
    }
#endif
    // Fallback: very basic English stemming (remove common suffixes)
    std::string w = word;
    if (w.length() > 4 && w.substr(w.length() - 3) == "ing") w = w.substr(0, w.length() - 3);
    else if (w.length() > 3 && w.substr(w.length() - 2) == "ed") w = w.substr(0, w.length() - 2);
    else if (w.length() > 3 && w.substr(w.length() - 1) == "s") w = w.substr(0, w.length() - 1);
    return w;
    // TODO: Integrate Snowball or other language-specific stemmers
}

Stemmer::~Stemmer() {
#if SNOWBALL_AVAILABLE
    if (sb_stemmer_) sb_stemmer_delete(sb_stemmer_);
#endif
} 