# s5301744 - Osher Shechter CFGAA Assignment

## - Assignment Idea #1: Foosball   
    Creating a foosball game that simulates the classic tabletop game in a virtual environment. Players will control the movement and rotation of the rods to hit the ball and score goals.

## - What i'm planning on doing:
    - Use OpenGL to display a complete foosball table, featuring players, rods, and a ball, with physics and   lighting.
    - Implement physics to handle ball movement, collisions with players and table boundaries, and the impact of rod movements on the ball's trajectory.
    - Create a control system that allows players to move and rotate the rods.
    - Implement the rules of foosball, including scoring, game restart, and tracking of wins.  

## - Design of classes / data structures:
    - Game - Manage the overall game state, including starting, pausing, ending, keeping track of the score.
    - Table - The table's dimensions, goal areas, texture.
    - Rod - The rod holding the player figures, should move along its axis and rotate. Each rod has a list of attached 'Player' instances.
    - Player - position on the rod, methods to interact with the ball.
    - Ball - position of the ball, velocity, methods for its movement and interaction with players and table boundaries.
    - Physics - Collision detection and response, movement, interactions between the ball and other entities.
    - UserControl - Processes player inputs from keyboard/mouse and translates them into actions in the game.
    - Renderer - Rendering the scene in 3D using OpenGL.
    - UI - Manages in-game UI elements like scoreboards, game menu.

## - How the program will flow


## - Assignment Idea #2: First-person POV Pac-Man   
    A 3D twist on the classic arcade game. player will navigate the 3D maze, collecting dots and power-ups, and avoiding ghosts.

## - What i'm planning on doing: 