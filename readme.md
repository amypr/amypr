### The Treasure Hunter Board Game

## Game Story 
Four unfortunate beings have been stranded in a jungle, and the only major landmark they can see is a temple. Hopefully, once they reach it, they will find some tourists to help them. On the way, they find curious treasures that they can trade with locals to help them in their journey. After each player has had a turn to make a move (by rolling dice), they will be mysteriously sucked into a minigame to play against each other for the promise of more treasure, and the treasure will be delegated to the players according to the outcome of the minigame. Whoever reaches the temple first will get to escape!

The main entry of our game is a board where players take turns rolling a die. Any player landing on certain tiles will trigger minigames or other random events. Both players will get into the minigames and compete with each other. They will collect treasures by achieving certain goals. They can use treasures to buy props to move faster on the board or gain certain capabilities in the next minigame. The player who reaches the temple first will gain the most treasure and the game will end, it’s possible that this bonus is not enough and they may still lose to another player with more treasures.

Debug commands, controls, and explanation for skeleton game:

## Board/overworld stage:
Hold left click and drag around, then release - roll dice

B - Previous stage
N - Advance stage
A - Select roll
S - Select items (not yet)
D - Activate edge scrolling
B - Unload/Load board game and all assets, and spawn a single die

## Camera controls:
Z - zoom in
O - zoom out
I - move the camera up
K - move the camera down
J - move the camera left
L - move the camera right

## Platformer minigame:
Y - load/unload platformer game
WASD - move player 1
Arrow keys - move player 2


## Tank minigame (requires separate loading of tank_game folder for now):
Arrow keys - Control player tank 1

## Drawing game:
C - debug drawing mode (only background and test geometry right now)


Attribution:

The following sound effects were obtained and used under license from https://www.zapsplat.com:
	dice_pickup.wav (zapsplat_leisure_game_dice_x3_throw_roll_carpet_001_69522.mp3)
	dice_roll.wav (zapsplat_leisure_game_dice_x3_throw_roll_carpet_001_69522.mp3)
	tile_pass.wav (zapsplat_multimedia_alert_ping_high_pitched_notification_bell_006_73139.mp3)
	jump_1.wav (zapsplat_multimedia_game-sound_classic_retro_jump_002_65122.mp3)
	paint_1.wav (zapsplat_industrial_paint_brush_long_single_stroke_001_11977.mp3)
	big_roll.wav (zapsplat_multimedia_game_sound_positive_reward_success_ascending_digital_002_49793.mp3)

Assets Used:
https://www.freepik.com/free-vector/tropical-forest-landscape-with-dense-forest_5481317.htm#page=1&query=rainforest&position=3&from_view=keyword for start screen background
https://www.cleanpng.com/png-pyramid-teotihuacan-computer-icons-2026445/ for temple in logo
https://www.pngkey.com/maxpic/u2e6u2r5y3w7r5a9/ for textbox outline
https://www.pngaaa.com/detail/164896 for toad sprite base
https://nookazon.com/product/3309461865 for trap

The function in BoardSystem.cpp called parallaxMovementUpdate() was inspired by the Unity tutorial video explaining the theory around the parallax effect. Source: https://www.youtube.com/watch?v=zit45k6CUMk

The triple32 function in the dice.gs.glsl file is from Chris Wellons 2018: https://nullprogram.com/blog/2018/07/31/

https://learnopengl.com/Advanced-OpenGL/Instancing was used as a guide in developing the particle system.

https://learnopengl.com/Advanced-OpenGL/Geometry-Shader was used as a guide in developing the dice geometry shader.