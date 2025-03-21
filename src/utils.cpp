#include "utils.h"
#include "config.h"
#include "constants.hpp"
#include "global_data.h"
#include <cstdint>
#include <fmt/format.h>
#include <string_view>

std::string gen_common_prompt(const std::string_view bot_name, const MiraiCP::QQID bot_id,
                              const std::string_view user_name, const uint64_t user_id) {
    return fmt::format("你的名字叫{}(qq号{})，{}。当前时间是: {}，当前跟你聊天的群友的名字叫\"{}\"(qq号{})，", bot_name,
                       bot_id, CUSTOM_SYSTEM_PROMPT, get_current_time_formatted(), user_name, user_id);
}

MessageProperties get_msg_prop_from_event(const MiraiCP::GroupMessageEvent &e) {
    MessageProperties ret{};

    for (auto msg : e.message) {
        MiraiCP::Logger::logger.info(std::string("Message Type: ") + std::to_string(msg.getType()) +
                                     ", Content: " + msg->content);

        if (msg.getType() == MiraiCP::SingleMessageType::At_t && msg->content == std::to_string(e.bot.id())) {
            ret.is_at_me = true;
        } else if (msg.getType() == MiraiCP::SingleMessageType::QuoteReply_t) {
            std::string s = msg.get()->toJson()["source"]["originalMessage"].dump();
            MiraiCP::Logger::logger.info(msg.get()->toJson());
            if (ret.ref_msg_content == nullptr) {
                ret.ref_msg_content = std::make_unique<std::string>(s);
            } else {
                *ret.ref_msg_content += s;
            }
        } else if (msg.getType() == MiraiCP::SingleMessageType::PlainText_t) {
            std::string s = msg->content;
            if (ret.plain_content == nullptr) {
                ret.plain_content = std::make_unique<std::string>(s);
            } else {
                *ret.plain_content += s;
            }
        } else if (msg.getType() == MiraiCP::SingleMessageType::OnlineForwardedMessage_t) {
            MiraiCP::Logger::logger.logger.info(msg->toJson());
        }
    }
    if (ret.plain_content != nullptr) {
        *ret.plain_content = std::string{ltrim(*ret.plain_content)};
        if (ret.plain_content->empty()) {
            *ret.plain_content = EMPTY_MSG_TAG;
        }
    } else {
        ret.ref_msg_content = std::make_unique<std::string>(EMPTY_MSG_TAG);
    }

    return ret;
}

// 寻找字符串中第一个 #keyword (...) (中的内容
std::string_view extract_quoted_content(const std::string_view s, const std::string_view keyword) {
    size_t keyword_pos = s.find(keyword);
    if (keyword_pos == std::string::npos) return "";
    size_t start_quote = s.find('(', keyword_pos + keyword.size());
    if (start_quote == std::string::npos) return "";
    size_t end_quote = s.find(')', start_quote + 1);
    if (end_quote == std::string::npos) return "";
    return s.substr(start_quote + 1, end_quote - start_quote - 1);
}

// 替换字符串中第一个 #keyword (...) 的内容
std::string replace_quoted_content(
    const std::string_view original_str,
    const std::string_view keyword,
    const std::string_view replacement
) {
    size_t keyword_pos = original_str.find(keyword);
    if (keyword_pos == std::string::npos) {
        return std::string{original_str}; // 未找到关键词，直接返回原字符串
    }
    size_t start_quote = original_str.find('(', keyword_pos + keyword.size());
    if (start_quote == std::string::npos) {
        return std::string{original_str}; // 起始双引号未找到
    }

    size_t end_quote = original_str.find(')', start_quote + 1);
    if (end_quote == std::string::npos) {
        return std::string{original_str};
    }
    size_t replace_start = keyword_pos;
    size_t replace_length = end_quote - keyword_pos + 1;

    std::string result = std::string{original_str};
    result.replace(replace_start, replace_length, replacement);
    return result;
}

#include <string>

bool is_strict_format(const std::string_view s, const std::string_view keyword) {
    const size_t key_len = keyword.size();

    if (s.size() < key_len + 2) return false;

    if (s.substr(0, key_len) != keyword) return false;

    size_t start_quote = s.find('(', key_len);
    if (start_quote == std::string_view::npos || start_quote < key_len) return false;

    size_t end_quote = s.find(')', start_quote + 1);
    if (end_quote != s.size() - 1) return false;

    return (end_quote > start_quote);
}
