# s5301744 - Osher Shechter CFGAA Assignment

##
- **Assignment Idea: First-person POV Pac-Man**     
    A 3D twist on the classic arcade game. player will navigate the 3D maze, collecting coins and avoiding ghost.

- **How the program will flow:**  
The game first sets up the OpenGL environment, loads the maze layout, initialises the game entities at the starting positions. During the game loop, the game continously processes player input to navigate the maze, update the state of ghosts, checks for collisions between the player, coins, walls, and ghost. Rendering occurs in each loop iteration, drawing the maze, player's first-person view, ghosts, and coins. The loop runs until the game ends which is when the ghost gets to the player.

- **What I did:**
    - Use OpenGL to build a navigable 3D maze, with walls, coins, and create the ghost.
    - Implement physics to simulate movement within the maze, along with collision detection to manage interactions with walls, dots, power-ups and ghosts.
    - Create a control system that allows players to navigate the maze, and collect the coins.


- **Project overview and process:**
    - **Starting point** - Utilised the (https://github.com/NCCA/VAOPrimitives/tree/main) repository as the foundational codebase. This repository provides a simple scene using all of the built in VAO primitives. The primary goal was to adapt this framework to create a more complex 3D maze.
    - **Image processing for maze creation** - To transform a complex image into a 3D maze, the project employs a method that interprets the image as overlaying a 15x15 grid, dividing it into 225 cells. Each cell's dimensions are calculated by proportionally dividing the image's dimensions. The image is then processed cell by cell to count the number of black pixels, determining whether each cell represents a wall (1) or a path (0) based on the majority color. This binary grid data is subsequently used to guide the automated placement of cubes in the 3D scene, creating a navigable maze that corresponds to the original image's layout.
    - **Array mapping** - A 2D array was created corresponding to the grid, where each cell was assigned a value based on the color detected in the image processing step:
                            - **1 (Wall)**: Indicates a black-colored area, suggesting the presence of a wall.
                            - **0 (Path)**: Represents a white-colored area, indicating open paths.
    - **3D Maze Construction** - Utilising the 2D array, I placed cubes within the 3D scene at positions corresponding to '1's in the array and placed 'coins' where there are paths, effectively translating the 2D maze representation into a navigable 3D environment. A coin will disappear once the character steps in its place (as it's collected).

<div style="display: flex; justify-content: space-around; align-items: center;">
  <img src="https://github.com/NCCA/cfgaa24programingassignment-Oshersh15/blob/main/image/Maze.png" alt="Maze Image 1" width="300"/>
  <img src="https://github.com/NCCA/cfgaa24programingassignment-Oshersh15/blob/main/image/mazeScreenshot.png" alt="Maze Image 2" width="300"/>
</div>
 The structure of the maze has been successfully implemented, with visual representation aligning with the conceptual design. 
 
   - **Key control** - I have implemented camera movement controlled by keys and set the initial camera position in the most central path. The camera position is represented by the number 2 within the array. The camera moves forward only, and rotations are required to change direction. Rotations are restricled to exactly 90-degree increments to ensure that movements align strictly with the grid so after a rotation, when to user wants to proceed forward, the camera angle will snap to the closesnt 90-degree increment.

 - **Collision Detection** - Before each movement, collision detection checks are performed to ensure the path is clear. The array is used to verify whether the next step  is a wall (1) or a path (0). If it is a wall it will remain in place and if it is a path it will move.

 - **Ghost** - A ghost, represented as a sphere, is initially placed in a random corner of the maze. Then, it actively chases the player by calculating possible paths from its current position and the distance from the player and chooses the next step that will minimize the most distance to the player. The game ends when the ghost gets to the player and concludes with a "Game Over" message. 

    <div style="display: flex; justify-content: space-around; align-items: center;">
  <img src="https://github.com/NCCA/cfgaa24programingassignment-Oshersh15/blob/main/image/game.jpg" alt="Maze Image 1" width="300"/>
  The image was generated using DALL-E

 Moving forward, the next phase of development will concentrate on:<ol> 
    <li>creating UI</li> 
    <li>making sure the game ends when the character and the ghost collide</li>
    <li> changing textures</li> </ol>
    <ol>
    If I have time: 
    <li>maybe switching the sphere with an obj file of a mesh resembling the ghost more</li>
    <li>maybe add more ghosts</li>
    </ol>

   - **Bibliography**
    https://www.enjoyalgorithms.com/blog/rotate-a-matrix-by-90-degrees-in-an-anticlockwise-direction - understanding how matrix rotation works.

    Most of the materials I used as a reference and for guidance where the materials in Jon Macey's WebPages and the NGL demos on NCCA gitHub, and as I stated before, my starting point was the VAOPrimitives demo.



