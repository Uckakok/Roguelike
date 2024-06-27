Roguelike Game Project
----------------------

### Overview

This repository contains the documentation and source code for an university Roguelike project. It is a classic roguelike game with procedurally generated levels where players navigate through dungeons, encountering enemies and collecting items to progress to increasingly difficult stages.

### Key Features

*   **Procedurally Generated Levels**: The game generates new levels dynamically, ensuring unique experiences for each playthrough.
*   **Enemy Encounters**: Players encounter various types of enemies, each with distinct abilities and behaviors.
*   **Item Collection and Use**: Items such as potions and runes can be collected to enhance player abilities.
*   **Interactive User Interface**: The game features a responsive UI built using WPF, offering seamless integration with the core game mechanics implemented in C++.
*   **Multi-Language Support**: Supports multiple languages dynamically loaded from .po files in the Resources/Localization directory.
*   **Save and Load Mechanism**: Automatic saving of game progress allows players to resume from where they left off.
*   **Graphics and Rendering**: Utilizes OpenGL for graphical rendering, with textures and shaders defined in the Resources/Textures and Resources/Shaders directories.

### Project Structure

*   **Renderer.dll**: Contains core game functionalities written in C++ using OpenGL, compiled into a dynamic link library.
*   **TestXaml.exe**: Main executable file written in C# using WPF for handling user interface elements and interaction.
*   **Resources/**: Directory containing game assets such as textures, shaders, localization files (.po), and level templates.

### How to Run

1.  **Clone Repository**: Clone the repository from [GitHub](https://github.com/Uckakok/Roguelike).
2.  **Compile**: Open the TextXaml.sln project in Visual Studio 2022 and compile (Renderer.sln should be setup as dependency so it should be compiled automatically. If not compile manually).
3.  **Run the Game**: Execute TestXaml.exe to launch the game. The game will automatically load the last saved state if available; otherwise, it will start a new game.

### Gameplay Instructions

*   **Movement**: Click adjacent tiles to move the player character.
*   **Combat**: Click on enemies to engage in combat. The outcome is based on player and enemy statistics.
*   **Interaction**: Use the "Use" button to interact with items or stairs when standing on them.
*   **Languages**: Change the game language dynamically through the options menu.

### Used Libraries

The project utilizes additional libraries with licenses provided in the [TestXaml/TestXaml/Licenses](https://github.com/Uckakok/Roguelike/tree/main/TestXaml/TestXaml/Licenses) directory.

### Disclaimer

This project was developed by Kajetan Żmijewski for educational purposes. For issues or inquiries, please refer to the GitHub repository issues section.

* * *
