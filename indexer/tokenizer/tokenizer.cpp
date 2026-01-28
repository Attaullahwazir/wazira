#include "tokenizer.h"
#include <vector>
#include <string>
#include <locale>
#include <algorithm>
#include <cctype>
#include <sstream>

/**
 * @brief Construct a Tokenizer for a given language (default: English).
 */
Tokenizer::Tokenizer(const std::string& language) {
    // For extensibility: store language, load language-specific rules if needed
    // (Not used in this basic implementation)
}

/**
 * @brief Tokenize input text into normalized tokens (words).
 *        Basic implementation: split on whitespace, strip punctuation, lowercase.
 */
std::vector<std::string> Tokenizer::tokenize(const std::string& text) const {
    std::vector<std::string> tokens;
    std::istringstream iss(text);
    std::string word;
    while (iss >> word) {
        std::string norm = normalize(word);
        if (!norm.empty()) tokens.push_back(norm);
    }
    return tokens;
}

/**
 * @brief Normalize a token: lowercase, strip leading/trailing punctuation.
 */
std::string Tokenizer::normalize(const std::string& token) const {
    std::string result;
    // Remove leading/trailing punctuation
    size_t start = 0, end = token.size();
    while (start < end && std::ispunct(static_cast<unsigned char>(token[start]))) ++start;
    while (end > start && std::ispunct(static_cast<unsigned char>(token[end-1]))) --end;
    for (size_t i = start; i < end; ++i) {
        result += std::tolower(static_cast<unsigned char>(token[i]));
    }
    return result;
} 