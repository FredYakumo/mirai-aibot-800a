#ifndef BOT_ADAPTER_H
#define BOT_ADAPTER_H

#include "adapter_event.h"
#include "adapter_message.h"
#include "mutex_data.hpp"
#include <easywsclient.hpp>
#include <functional>
#include <optional>
#include <spdlog/spdlog.h>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace bot_adapter {
    class BotAdapter {
      public:
        BotAdapter(const std::string_view url) {
#ifdef _WIN32
            INT rc;
            WSADATA wsaData;
            rc = WSAStartup(MAKEWORD(2, 2), &wsaData);
            if (rc) {
                spdlog::error("WSAStartup Failed.");
                throw rc;
            }
#endif
            ws = easywsclient::WebSocket::from_url(std::string(url));
        }
        ~BotAdapter();

        int start();

        template <typename EventFuncT> inline void register_event(std::function<void(const EventFuncT &e)> func) {
            static_assert(std::is_base_of<MessageEvent, EventFuncT>::value,
                          "EventFuncT must be derived from MessageEvent");
            msg_handle_func_list.push_back([func](const MessageEvent &e) {
                if (auto specific_event = dynamic_cast<const EventFuncT *>(&e)) {
                    func(*specific_event);
                }
            });
        }

        void send_message(const Group &group, const MessageChainPtrList &message_chain,
                         std::optional<std::function<void(uint64_t &out_message_id)>> out_message_id_option = std::nullopt);

      private:
        void handle_message(const std::string &message);
        std::vector<std::function<void(const MessageEvent &e)>> msg_handle_func_list;

        MutexData<std::unordered_map<std::string, std::function<void(uint64_t message_id)>>>
            send_msg_result_handle_map;

        void handle_send_msg_result(const std::string &sync_id, const nlohmann::json &data_json);

        easywsclient::WebSocket::pointer ws;
    };

} // namespace bot_adapter
#endif