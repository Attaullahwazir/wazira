#include <string>
#include <vector>

/**
 * @class Tokenizer
 * @brief Unicode-aware, language-sensitive text tokenizer.
 */
class Tokenizer {
public:
    Tokenizer(const std::string& language = "en");
    std::vector<std::string> tokenize(const std::string& text) const;
    std::string normalize(const std::string& token) const;
}; 