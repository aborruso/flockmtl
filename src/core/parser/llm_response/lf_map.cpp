#include <large_flock/common.hpp>
#include <large_flock/core/parser/llm_response.hpp>
#include <large_flock_extension.hpp>
#include <regex>

namespace large_flock {
namespace core {

std::vector<std::string> CoreLlmResponseParsers::LfMapResponseParser(std::string &response, int num_responses) {
    std::vector<std::string> responses;

    // Define the regex pattern to match "Response {i}: content"
    std::regex response_pattern(R"(Response (\d+):\s*(.*?)(?=\s*Response \d+:|$))");
    std::smatch match;

    auto it = response.cbegin();
    auto end = response.cend();

    // Iterate over all matches
    while (std::regex_search(it, end, match, response_pattern)) {
        if (match.size() == 3) { // Match group 1 is the index, group 2 is the content
            std::string content = match[2].str();
            responses.push_back(content);
        }
        it = match.suffix().first; // Move iterator to the end of the current match
    }

    // Ensure we have exactly the number of responses expected
    if (responses.size() < num_responses) {
        responses.resize(num_responses,
                         ""); // Fill missing responses with empty strings
    }

    return responses;
}

} // namespace core
} // namespace large_flock