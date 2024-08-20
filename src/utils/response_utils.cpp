#include "utils/response_utils.h"

namespace duckdb
{
    std::vector<std::string> ParseApiResponse(const std::string &response_text, size_t num_responses)
    {
        std::vector<std::string> responses;

        // Define the regex pattern to match "Response {i}: content"
        std::regex response_pattern(R"(Response (\d+):\s*(.*?)(?=\s*Response \d+:|$))");
        std::smatch match;

        auto it = response_text.cbegin();
        auto end = response_text.cend();

        // Iterate over all matches
        while (std::regex_search(it, end, match, response_pattern))
        {
            if (match.size() == 3)
            { // Match group 1 is the index, group 2 is the content
                std::string content = match[2].str();
                responses.push_back(content);
            }
            it = match.suffix().first; // Move iterator to the end of the current match
        }

        // Ensure we have exactly the number of responses expected
        if (responses.size() < num_responses)
        {
            responses.resize(num_responses, ""); // Fill missing responses with empty strings
        }

        return responses;
    }

    std::vector<std::string> SendRequestAndHandleResponse(const std::string &combined_prompt, size_t num_responses)
    {
        std::string response_text;
        try
        {
            response_text = litellm(combined_prompt);
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error("Failed to get response from litellm: " + std::string(e.what()));
        }

        return ParseApiResponse(response_text, num_responses);
    }

    std::vector<std::string> ChunkAndSendRequests(KeyValueMap &data_map, size_t size)
    {
        std::vector<std::string> all_responses;

        // Determine chunk size based on context window and other factors

        size_t chunk_size = DetermineChunkSize(data_map);

        size_t total_prompts = size;
        
        size_t current_index = 0;

        while (current_index < total_prompts)
        {
            size_t end_index = std::min(current_index + chunk_size, total_prompts);

            // Create the prompt context
            inja::json context = CreatePromptContext(data_map, current_index, end_index);

            // Generate the combined prompt
            std::string combined_prompt = GenerateCombinedPrompt(context);

            // Send the request and handle the response
            std::vector<std::string> parsed_responses = SendRequestAndHandleResponse(combined_prompt, end_index - current_index);

            all_responses.insert(all_responses.end(), parsed_responses.begin(), parsed_responses.end());

            current_index = end_index;
        }

        return all_responses;
    }
}