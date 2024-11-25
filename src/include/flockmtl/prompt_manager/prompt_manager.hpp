#pragma once

#include <string>
#include <nlohmann/json.hpp>
#include <fmt/format.h>

#include "flockmtl/core/config.hpp"
#include "flockmtl/prompt_manager/repository.hpp"

namespace flockmtl {

class PromptManager {
public:
    static std::string ReplaceSection(const std::string& prompt_template, const PromptSection section,
                                      const std::string& section_content);
    static std::string ReplaceSection(const std::string& prompt_template, const std::string& replace_string,
                                      const std::string& section_content);

    template <typename T>
    static std::string ToString(const T element);
    template <typename T>
    static T FromString(const std::string& element);

    template <typename FunctionType>
    static std::string GetTemplate(FunctionType option) {
        auto prompt_template =
            PromptManager::ReplaceSection(META_PROMPT, PromptSection::INSTRUCTIONS, INSTRUCTIONS::Get(option));
        auto response_format = RESPONSE_FORMAT::Get(option);
        prompt_template =
            PromptManager::ReplaceSection(prompt_template, PromptSection::RESPONSE_FORMAT, response_format);
        return prompt_template;
    };

    static PromptDetails CreatePromptDetails(const nlohmann::json prompt_details_json);

    static std::string ConstructMarkdownHeader(const nlohmann::json& tuple);

    static std::string ConstructMarkdownSingleTuple(const nlohmann::json& tuple);

    static std::string ConstructMarkdownArrayTuples(const nlohmann::json& tuples);

    template <typename FunctionType>
    static std::string Render(const std::string& user_prompt, const nlohmann::json& tuples, FunctionType option) {
        auto prompt = PromptManager::GetTemplate(option);
        auto markdown_tuples = PromptManager::ConstructMarkdownArrayTuples(tuples);
        prompt = PromptManager::ReplaceSection(prompt, PromptSection::USER_PROMPT, user_prompt);
        prompt = PromptManager::ReplaceSection(prompt, PromptSection::TUPLES, markdown_tuples);
        return prompt;
    };
};

} // namespace flockmtl
