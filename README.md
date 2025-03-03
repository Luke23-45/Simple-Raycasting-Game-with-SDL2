# Raycasting Engine

A raycasting engine implemented in C++ using SDL2. It features textured walls, basic enemy AI, a minimap, and a user interface displaying FPS and player position.

## Getting Started

### Prerequisites
Ensure you have the following installed:
- C++ Compiler (e.g., g++)
*   [SDL2](https://www.libsdl.org/)
*   [SDL2_ttf](https://www.libsdl.org/projects/SDL_ttf/)


## Project Structure

*   `ray.cc`: Main C++ source file containing all game logic.
*   `Makefile`: Build the project.
*   `src/`
    *   `include/`: Header files 

### Installation
1. Clone the repository:
    ```bash
    git clone git@github.com:Luke23-45/Simple-Raycasting-Game-with-SDL2.git
    ```
## Building

This project uses `make` for building. To build the project, run the following command in your terminal:

1. Navigate to the project directory:
    ```bash
    cd Simple-Raycasting-Game-with-SDL2
    ```
3. Compile the code:
    ```bash
     make
    ```
4. Run the executable (Linux/macOS):
    ```bash
    ./main

    ```
5. Run the executable (Windows):
    ```bash
    main.exe
    ```
6. To clean up the build artifacts:
    ```bash
     make clean
    ```

## Features
- Raycasting rendering of a 3D environment.
- Textured walls to enhance visual depth.
- Basic enemy AI: Enemies actively move towards the player.
- Shooting mechanism to eliminate enemies.
- Minimap display to aid navigation.
- Real-time FPS counter for performance monitoring.
- Display of player coordinates for debugging or information.
- Simple crosshair for aiming.
- Floor rendering with a gradient effect.
- Keyboard and mouse input controls.
- Basic collision detection to prevent player from passing through walls.

## Key Controls

| Action            | Key/Control         | Description                                  |
| ----------------- | ------------------- | -------------------------------------------- |
| Exit simulation   | `ESC` key           | Quits the application.                       |
| Move Forward      | `W` key             | Moves the player forward in the viewing direction. |
| Move Backward     | `S` key             | Moves the player backward.                    |
| Strafe Left       | `A` key             | Moves the player to the left.                 |
| Strafe Right      | `D` key             | Moves the player to the right.                |
| Rotate Left       | `LEFT ARROW` key    | Rotates the player's view to the left.       |
| Rotate Right      | `RIGHT ARROW` key   | Rotates the player's view to the right.      |
| Look Around       | Mouse Movement      | Controls the viewing direction.              |
| Shoot             | `Left Mouse Button` | Attempts to shoot enemies in the crosshair.   |


## Code Structure
The project is structured as follows:

*   `ray.cc`: This single file contains the entire C++ source code for the raycasting engine.
    *   **Includes**: Includes necessary SDL2 libraries (`SDL.h`, `SDL_image.h`, `SDL_ttf.h`), standard C++ libraries (`cmath`, `vector`, `iostream`, `string`).
    *   **Constants**: Defines constants for screen dimensions, map size, mouse sensitivity, and player settings.
    *   **Data Structures**: Defines structs for `Player` and `Enemy` to manage game entities. The `worldMap` array represents the game world.
    *   **Global Variables**: Declares global variables for SDL textures (`wallTexture`), font (`font`), enemy vector (`enemies`), and shooting cooldown management.
    *   **Function Prototypes**: Declares function prototypes for input handling, enemy updates, shooting, raycasting, rendering, and utility functions.
    *   **`main` Function**:
        *   Initializes SDL, SDL_image, and SDL_ttf.
        *   Creates a window and renderer.
        *   Loads wall texture and font.
        *   Sets up relative mouse mode.
        *   Initializes player and enemies.
        *   Enters the main game loop for event handling, input processing, game logic updates, and rendering.
        *   Cleans up resources on exit.
    *   **Input Handling (`handleInput`)**: Manages keyboard inputs for player movement (forward, backward, strafing, rotation) and exit.
    *   **Enemy Updates (`updateEnemies`)**: Updates enemy positions, making them move towards the player.
    *   **Shooting (`handleShooting`)**: Handles player shooting logic, detecting if enemies are in sight and reducing their health.
    *   **Raycasting (`castRays`)**: Implements the core raycasting algorithm to render the 3D view, including wall textures.
    *   **Wall Detection (`isWall`)**: Checks if a given coordinate is a wall in the `worldMap`.
    *   **Floor Rendering (`renderFloor`)**: Renders a gradient-shaded floor.
    *   **UI Rendering (`renderUI`)**: Renders the minimap, player position, FPS, and enemy count on the screen.
    *   **Text Rendering (`renderText`)**: Utility function to render text on the screen using SDL_ttf.
    *   **Crosshair Rendering (`renderCrosshair`)**: Renders a simple crosshair in the center of the screen.


## Demo Video
Check out the project demo video on YouTube: https://youtu.be/HDTzYFRzg50
## License

This project is licensed under the MIT License. Feel free to use, modify, and distribute the code.

## Acknowledgements

- SDL2 for graphics rendering.
