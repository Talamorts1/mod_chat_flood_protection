/*
 * Copyright (C) 2016+ AzerothCore <www.azerothcore.org>, released under GNU AGPL v3 license:
 * https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3
 *
 * Scripted by Talamortis
 * https://github.com/talamortis - Main
 * https://github.com/Talamorts1
 *
 * 
 */

#include "ScriptMgr.h"
#include "Player.h"
#include "Config.h"
#include "Chat.h"

#include <unordered_map>
#include <deque>
#include <ctime>

uint32 MAX_MESSAGES = 5;        // Maximum allowed messages
uint32 TIME_FRAME = 10;         // Max Time Frame in seconds
uint32 MUTE_DURATION = 60;      // Mute Duration in seconds

// Maps to track message logs and muted players
std::unordered_map<uint64, std::deque<time_t>> _messageLogs;
std::unordered_map<uint64, time_t> _mutedPlayers;

// Player script for chat flood protection
class chat_flood_protection : public PlayerScript
{
public:
    chat_flood_protection() : PlayerScript("chat_flood_protection") {}

    void OnLogin(Player* player) override
    {
        uint64 guid = player->GetGUID().GetCounter();
        time_t currentTime = time(nullptr); // Get the current time
        auto muteIt = _mutedPlayers.find(guid);

        // Check if the player has the mute aura
        if (player->HasAura(1852))
        {
            // If the player is not already in the mutedPlayers map
            if (muteIt == _mutedPlayers.end())
            {
                // Insert the player into the mutedPlayers map with a refreshed mute duration
                _mutedPlayers[guid] = currentTime + MUTE_DURATION;

                // Optionally, you can notify the player about their mute status
                ChatHandler(player->GetSession()).PSendSysMessage("You are currently muted. The mute duration has been refreshed.");
            }
        }
    }

    // Overloaded OnChat functions to handle different chat contexts
    void OnChat(Player* player, uint32 type, uint32 lang, std::string& msg) override
    {
        handleChatMessage(player, type, msg);
    }

    void OnChat(Player* player, uint32 type, uint32 lang, std::string& msg, Player* receiver) override
    {
        if (receiver->GetSession()->IsGMAccount())
            return;

        handleChatMessage(player, type, msg);
    }

    void OnChat(Player* player, uint32 type, uint32 lang, std::string& msg, Group* group) override
    {
        handleChatMessage(player, type, msg);
    }

    void OnChat(Player* player, uint32 type, uint32 lang, std::string& msg, Guild* guild) override
    {
        handleChatMessage(player, type, msg);
    }

    void OnChat(Player* player, uint32 type, uint32 lang, std::string& msg, Channel* channel) override
    {
        handleChatMessage(player, type, msg);
    }

    // Called periodically to update player state
    void OnBeforeUpdate(Player* player, uint32 diff) override
    {
        RemoveMuteDeBuff(player);
    }

private:

    void RemoveMuteDeBuff(Player* player)
    {
        uint64 guid = player->GetGUID().GetCounter();
        time_t currentTime = time(nullptr); // Get the current time

        auto muteIt = _mutedPlayers.find(guid);

        if (muteIt != _mutedPlayers.end())
        {
            if (currentTime >= muteIt->second)
            {
                // Remove expired mute
                _mutedPlayers.erase(muteIt);
                player->RemoveAura(1852);
                ChatHandler(player->GetSession()).PSendSysMessage("You have been unmuted.");
            }
        }
    }

    // Handles incoming chat messages for flood protection
    void handleChatMessage(Player* p, uint32 type, std::string& message)
    {
        uint64 guid = p->GetGUID().GetCounter();
        time_t currentTime = time(nullptr); // Get the current time

        // Check if the player is currently muted
        auto muteIt = _mutedPlayers.find(guid);
        if (muteIt != _mutedPlayers.end())
        {
            // Player is muted; prevent sending the message
            ChatHandler(p->GetSession()).PSendSysMessage("You are muted and cannot send messages.");
            return;
        }

        // Initialize message log for the player if it doesn't exist
        if (_messageLogs.find(guid) == _messageLogs.end())
            _messageLogs[guid] = std::deque<time_t>();

        std::deque<time_t>& timestamps = _messageLogs[guid];

        // Remove timestamps that are outside the time frame
        while (!timestamps.empty() && (currentTime - timestamps.front()) > TIME_FRAME)
        {
            timestamps.pop_front();
        }

        // Add the current message timestamp
        timestamps.push_back(currentTime);

        // Check if the player has exceeded the maximum allowed messages
        if (timestamps.size() > MAX_MESSAGES)
        {
            // Mute the player
            _mutedPlayers[guid] = currentTime + MUTE_DURATION;
            p->AddAura(1852, p);
            ChatHandler(p->GetSession()).PSendSysMessage("You have been muted for spamming. Please wait before sending more messages.");

            // Optionally, you can clear the message log to reset after muting
            _messageLogs.erase(guid);
        }
    }
};

class ChatFloodConf : public WorldScript
{
public:
    ChatFloodConf() : WorldScript("chat_flood_conf") {}

    void OnBeforeConfigLoad(bool /*reload*/) override
    {
        MAX_MESSAGES = sConfigMgr->GetOption<uint32>("max_messages", 5);
        TIME_FRAME = sConfigMgr->GetOption<uint32>("time_frame", 10);
        MUTE_DURATION = sConfigMgr->GetOption<uint32>("mute_duration", 60);
    }
};

// Register the script
void AddChatFloodProtection()
{
    new ChatFloodConf();
    new chat_flood_protection();
}
