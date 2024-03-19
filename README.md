# s5301744 - Osher Shechter CFGAA Assignment

- **Assignment Idea #1: Foosball**   
    Creating a foosball game that simulates the classic tabletop game in a virtual environment. Players will control the movement and rotation of the rods to hit the ball and score goals.

- **What i'm planning on doing:**
    - Use OpenGL to display a complete foosball table, featuring players, rods, and a ball, with physics and   lighting.
    - Implement physics to handle ball movement, collisions with players and table boundaries, and the impact of rod movements on the ball's trajectory.
    - Create a control system that allows players to move and rotate the rods.
    - Implement the rules of foosball, including scoring, game restart, and tracking of wins.  

- **Design of classes / data structures:**
    - **Game** - Manage the overall game state, including starting, pausing, ending, keeping track of the score.
    - **Table** - The table's dimensions, goal areas, texture.
    - **Rod** - The rod holding the player figures, should move along its axis and rotate. Each rod has a list of attached 'Player' instances.
    - **Player** - position on the rod, methods to interact with the ball.
    - **Ball** - position of the ball, velocity, methods for its movement and interaction with players and table boundaries.
    - **Physics** - Collision detection and response, movement, interactions between the ball and other entities.
    - **UserControl** - Processes player inputs from keyboard/mouse and translates them into actions in the game.
    - **Renderer** - Rendering the scene in 3D using OpenGL.
    - **UI** - Manages in-game UI elements like scoreboards, game menu.

- **How the program will flow:**



- **Assignment Idea #2: First-person POV Pac-Man**     
    A 3D twist on the classic arcade game. player will navigate the 3D maze, collecting dots and power-ups, and avoiding ghosts.

- **What i'm planning on doing:**
    - Use OpenGL to build a navigable 3D maze, with walls, dots, and power-ups and also create the ghosts.
    - Implement physics to simulate movement within the maze, along with collision detection to manage interactions with walls, dots, power-ups and ghosts.
    - Create a control system that allows players to navigate the maze, and collect and use power-ups.

- **Design of classes / data structures:**
    - **Game** - Manage the overall game state, including starting, pausing, ending, keeping track of the 
    - **Player (Pac-Man)** - Represents the player navigating the maze. It tracks the first-person camera position, orientation, collected dots, and inventory of power-ups.
    - **Maze** - The layout of the maze, including walls, dots, and power-ups locations. Also responsible for rendering the maze from the player's perspective.
    - **Dot** - Collectible dot in the maze, manages state (collected or not) and rendering.
    - **PowerUp** - Power-ups can be stored in the player's inventory and used against ghosts.
    - **Ghost** - Represents the ghosts, managing their behaviours of chasing the player and responding to power-ups by exploding to pixels when getting hit by them. Also responsible for resetting the ghosts after they explode.
    - **Inventory** - Manages the player's collection of power-ups. Allows for storing and using them.
    - **Physics** - Handles movement, collision detection, and interactions between game entities.
