# s5301744 - Osher Shechter CFGAA Assignment

##
- **Assignment Idea: First-person POV Pac-Man**     
    A 3D twist on the classic arcade game. player will navigate the 3D maze, collecting dots and power-ups, and avoiding ghosts.

- **What I'm planning on doing:**
    - Use OpenGL to build a navigable 3D maze, with walls, dots, and power-ups and also create the ghosts.
    - Implement physics to simulate movement within the maze, along with collision detection to manage interactions with walls, dots, power-ups and ghosts.
    - Create a control system that allows players to navigate the maze, and collect and use power-ups.


- **Project overview and process:**
    - **Starting point** - Utilised the (https://github.com/NCCA/VAOPrimitives/tree/main) repository as the foundational codebase. This repository provides a simple scene using all of the built in VAO primitives. The primary goal was to adapt this framework to create a more complex 3D maze.
    - **Image processing for maze creation** - To transform a complex image into a 3D maze, the project employs a method that interprets the image as overlaying a 15x15 grid, dividing it into 225 cells. Each cell's dimensions are calculated by proportionally dividing the image's dimensions. The image is then processed cell by cell to count the number of black pixels, determining whether each cell represents a wall (1) or a path (0) based on the majority color. This binary grid data is subsequently used to guide the automated placement of cubes in the 3D scene, creating a navigable maze that corresponds to the original image's layout.
    - **Array mapping** - A 2D array was created corresponding to the grid, where each cell was assigned a value based on the color detected in the image processing step:
                            - **1 (Wall)**: Indicates a black-colored area, suggesting the presence of a wall.
                            - **0 (Path)**: Represents a white-colored area, indicating open paths.
    - **3D Maze Construction** - Utilizing the 2D array, I placed cubes within the 3D scene at positions corresponding to '1's in the array, effectively translating the 2D maze representation into a navigable 3D environment.

- **Current status and next steps:**
<div style="display: flex; justify-content: space-around; align-items: center;">
  <img src="https://github.com/NCCA/cfgaa24programingassignment-Oshersh15/blob/main/image/Maze.png" alt="Maze Image 1" width="300"/>
  <img src="https://github.com/NCCA/cfgaa24programingassignment-Oshersh15/blob/main/image/mazeScreenshot.png" alt="Maze Image 2" width="300"/>
</div>
 The structure of the maze has been successfully implemented, with visual representation aligning with the conceptual design. Moving forward, the next phase of development will concentrate on enhancing user interaction by implementing camera movement controls and integrating collision detection.  




- **Design of classes / data structures:**
    - **Game** - Manage the overall game state, including starting, pausing, ending, game loop and interactions between components. 
    - **Player (Pac-Man)** - Represents the player navigating the maze. It tracks the first-person camera position, orientation, collected dots, and inventory of power-ups.
    - **Maze** - The layout of the maze, including walls, dots, and power-ups locations. Also responsible for rendering the maze from the player's perspective.
    - **Dot** - Collectible dot in the maze, manages state (collected or not) and rendering.
    - **PowerUp** - Power-ups can be stored in the player's inventory and used against ghosts.
    - **Ghost** - Represents the ghosts, managing their behaviours of chasing the player and responding to power-ups by exploding to pixels when getting hit by them. Also responsible for resetting the ghosts after they explode.
    - **Inventory** - Manages the player's collection of power-ups. Allows for storing and using them.
    - **Physics** - Handles movement, collision detection, and interactions between game entities.

- **How the program will flow:**  
    The game first sets up the OpenGL environment, loads the maze layout, initialises the game entities at the starting positions. During the game loop, the game continously processes player input to navigate the maze, update the state of ghosts, checks for collisions between the player, dots, power-ups, and ghosts, and applies the effects of the power-ups used by the player. The game also updates the inventory when power-ups are collected and allows the player to use them when he chooses to. Rendering occurs in each loop iteration, drawing the maze, player's first-person view, ghosts, and UI elements from the current game state. The loop runs until the game ends which is when all lives (3) are ran out, then the game concludes by displaying the final score and offering an option to restart or exit.


