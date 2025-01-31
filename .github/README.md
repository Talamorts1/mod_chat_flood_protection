# Chat Flood Protection Module for AzerothCore

![AzerothCore Logo](https://azerothcore.org/media/logo.png)

**Chat Flood Protection** is a C++ module designed for [AzerothCore](https://www.azerothcore.org/), a popular World of Warcraft server emulator. This module implements robust chat flood protection mechanisms to prevent spamming and maintain a healthy in-game chat environment.

## Table of Contents

- [Features](#features)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Configuration](#configuration)
- [Usage](#usage)
- [How It Works](#how-it-works)
- [Contributing](#contributing)
- [License](#license)
- [Support](#support)

## Features

- **Message Rate Limiting:** Restricts the number of messages a player can send within a specified time frame.
- **Automatic Muting:** Automatically mutes players who exceed the message limit for a configurable duration.
- **Persistent Mute Status:** Maintains mute status across player logins and server restarts.
- **Configurable Parameters:** Easily adjust maximum messages, time frames, and mute durations via configuration files.
- **User Notifications:** Informs players when they are muted and when their mute has expired.
- **Aura Management:** Applies and removes mute auras (`1852`) to visually indicate muted status.

## Prerequisites

- **AzerothCore Server:** Ensure you have a running AzerothCore server setup. For installation instructions, visit the [AzerothCore Installation Guide](https://www.azerothcore.org/wiki/installation).
- **C++ Compiler:** A compatible C++ compiler for building AzerothCore modules.
- **Development Tools:** Necessary tools and dependencies for compiling AzerothCore scripts.

## Installation

1. **Download the Module:**

   Clone the repository or download the module's source code.

   ```bash
   git clone https://github.com/Talamorts1/mod_chat_flood_protection.git
   ```

2. **Place the Script:**

   Copy the `chat_flood_protection.cpp` file into the AzerothCore `src/server/scripts/Custom` directory.

   ```bash
   cp chat_flood_protection.cpp /path/to/azerothcore/src/server/scripts/Custom/
   ```

3. **Update CMakeLists.txt:**

   Add the module to the AzerothCore build system by editing the `CMakeLists.txt` in the `Custom` scripts directory.

   ```cmake
   set(CUSTOM_SCRIPTS
       chat_flood_protection.cpp
       # Add other custom scripts here
   )
   ```

4. **Recompile AzerothCore:**

   Navigate to your AzerothCore build directory and compile the server.

   ```bash
   cd /path/to/azerothcore/build/
   cmake ../ -DCMAKE_INSTALL_PREFIX=/path/to/azerothcore/install/
   make -j$(nproc)
   make install
   ```

5. **Restart the Server:**

   After successful compilation, restart your AzerothCore server to apply the changes.

   ```bash
   ./worldserver
   ```

## Configuration

The module's behavior can be customized via configuration options in your server's configuration files (e.g., `world.conf`).

### Configurable Parameters

| Parameter        | Description                                | Default Value |
|------------------|--------------------------------------------|---------------|
| `max_messages`   | Maximum number of allowed messages.        | `5`           |
| `time_frame`     | Time frame in seconds to count messages.    | `10`          |
| `mute_duration`  | Duration in seconds for which a player is muted after exceeding the message limit. | `60`          |

### Setting Configuration Options

Edit the following lines to your `flood_protection.conf` or appropriate configuration file to customize the module:

```ini
# Chat Flood Protection Settings
max_messages = 5
time_frame = 10
mute_duration = 60
```

**Note:** Adjust the values according to your server's requirements and player behavior patterns.

## Usage

Once installed and configured, the Chat Flood Protection module operates automatically:

1. **Message Monitoring:**
   - The module tracks the number of messages each player sends within the specified `time_frame`.
   
2. **Automatic Muting:**
   - If a player exceeds `max_messages` within the `time_frame`, they are automatically muted.
   - A mute aura (`1852`) is applied to the player, preventing them from sending further messages.
   
3. **Mute Duration:**
   - The player remains muted for `mute_duration` seconds.
   - Upon logging in, if a player has an active mute aura, the mute duration is refreshed.
   
4. **Notifications:**
   - Players receive system messages informing them when they are muted and when the mute is lifted.

### Example Scenario

1. **Player Spamming:**
   - A player sends 6 messages within 10 seconds.
   - The module detects the excess and mutes the player for 60 seconds.
   - The player receives a system message: "You have been muted for spamming. Please wait before sending more messages."
   
2. **Player Attempts to Chat While Muted:**
   - The player tries to send a message during the mute period.
   - The module blocks the message and notifies the player: "You are muted and cannot send messages."
   
3. **Mute Expiration:**
   - After 60 seconds, the mute is lifted.
   - The player receives a system message: "You have been unmuted."

## How It Works

### Core Components

- **Message Tracking:**
  - Uses an `std::unordered_map` to keep track of each player's message timestamps using their GUID.
  - Employs a `std::deque` to efficiently manage and purge old message timestamps outside the `time_frame`.

- **Muting Mechanism:**
  - When a player exceeds the `max_messages` within the `time_frame`, they are muted by applying an aura (`1852`).
  - The player's mute status and expiration time are stored in the `_mutedPlayers` map.

- **Login Handling:**
  - Upon player login, the module checks for the mute aura.
  - If the player is muted, their mute duration is refreshed to ensure consistency across server restarts.

- **Periodic Updates:**
  - The `OnBeforeUpdate` function periodically checks if the mute duration has expired.
  - If the mute has expired, the aura is removed, and the player is notified.

### Key Functions

- **`OnLogin(Player* player)`:**
  - Handles player login events.
  - Ensures that muted players are tracked and their mute durations are refreshed.

- **`handleChatMessage(Player* p, uint32 type, std::string& message)`:**
  - Processes incoming chat messages.
  - Determines if a player has exceeded the message limit and applies mutes accordingly.

- **`RemoveMuteDeBuff(Player* player)`:**
  - Checks and removes expired mutes.
  - Notifies players upon unmute.

## Contributing

Contributions are welcome! If you'd like to enhance the Chat Flood Protection module, please follow these steps:

1. **Fork the Repository:**

   Click the "Fork" button at the top-right corner of the repository page.

2. **Clone Your Fork:**

   ```bash
   git clone https://github.com/yourusername/chat-flood-protection.git
   ```

3. **Create a Feature Branch:**

   ```bash
   git checkout -b feature/YourFeatureName
   ```

4. **Commit Your Changes:**

   ```bash
   git commit -m "Add new feature: YourFeatureName"
   ```

5. **Push to Your Fork:**

   ```bash
   git push origin feature/YourFeatureName
   ```

6. **Open a Pull Request:**

   Navigate to the original repository and open a pull request detailing your changes.

### Guidelines

- **Code Quality:** Ensure your code follows the existing style and conventions.
- **Documentation:** Update the README and inline comments as necessary.
- **Testing:** Thoroughly test your changes to prevent regressions.

## License

This project is licensed under the **GNU Affero General Public License v3.0 (AGPL-3.0)**. See the [LICENSE-AGPL3](https://github.com/azerothcore/azerothcore-wotlk/blob/master/LICENSE-AGPL3) file for details.

## Support

If you encounter issues or have questions regarding the Chat Flood Protection module, feel free to reach out:

- **AzerothCore Forums:** [https://www.azerothcore.org/wiki/forum](https://www.azerothcore.org/wiki/forum)
- **GitHub Issues:** Open an issue in the [repository](https://github.com/Talamorts1/mod_chat_flood_protection/issues).
- **Discord:** Join the AzerothCore Discord server for real-time assistance.

---

*Thank you for using the Chat Flood Protection module! We hope it enhances your AzerothCore server's chat experience.*
