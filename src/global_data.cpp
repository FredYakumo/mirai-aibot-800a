#include "global_data.h"
#include "mutex_data.hpp"

/**
 * @brief A global data structure for managing user chat sessions.
 * 
 * This is a thread-safe container that pairs a mutex with an unordered map.
 * The unordered map uses a 64-bit unsigned integer as the key (e.g., user ID)
 * and a ChatSession object as the value to represent the chat sessions of each user.
 * The mutex ensures synchronized access to the map in a multithreaded environment.
 */
MutexData<std::unordered_map<uint64_t, ChatSession>> g_chat_session_map;

/**
 * @brief A global data structure for managing chat processing states.
 * 
 * This is a thread-safe container that pairs a mutex with an unordered map.
 * The unordered map uses a 64-bit unsigned integer as the key (e.g., chat ID)
 * and a boolean as the value to indicate whether the chat is currently being processed.
 * The mutex ensures synchronized access to the map in a multithreaded environment.
 */
std::pair<std::mutex, std::unordered_map<uint64_t, bool>> g_chat_processing_map;

/**
 * @brief A global data structure for managing knowledge items to be added.
 * 
 * This is a thread-safe container that pairs a mutex with a vector.
 * The vector contains DBKnowledge objects representing knowledge items
 * that are waiting to be added to the database or system.
 * The mutex ensures synchronized access to the vector in a multithreaded environment.
 */
MutexData<std::vector<DBKnowledge>> g_wait_add_knowledge_list;

/**
 * @brief A global data structure for managing bot-sent messages in group chats.
 * 
 * This is a thread-safe container that pairs a mutex with an unordered map.
 * The unordered map uses a 64-bit unsigned integer as the key (e.g., group ID)
 * and a ChatSession object as the value to represent the messages sent by the bot
 * in each group chat.
 * The mutex ensures synchronized access to the map in a multithreaded environment.
 */
MutexData<std::unordered_map<uint64_t, ChatSession>> g_group_chat_bot_send_msg;