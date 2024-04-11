COMP 2501 Final Project
Hendry Hu		#101263863
Ho Nam Leung	#101197127

INSTRUCTIONS for compilation:
Use CMake on Windows and set source as the folder containing CMakeLists.txt. Use the Visual Studio 2022 project generation preset & library path as usual,
same to all other class assignments.

Open the generated project in Visual Studio. The game should compile and run by default.

INSTRUCTIONS for playing:
The start screen should have good visual instruction on weapon details and objective, but here is a text description for reference.

The player is the white and orange ship on the center of the screen.
 - WASD to move
 - LMB to shoot
 - Space Bar to shoot secondary weapon (doesn't exist by default, see below for more details.)

To win the game, follow the blue arrow next to the player, which points in the direction of the Token of Appreciation.

Four Tokens will spawn randomly in the world. Collect all four to open the door. Then, the arrow will point towards the door. Finally, enter the door to win.

Enemies, which are monster-infested ships, will spawn around the player and will also spawn from the Tokens. If the player loses all of its health, it will be game over.
Enemies will attack players in various ways (see below for more details).

TO TEST:
In the config.h are flags that can be used for testing.

 - Test Mode
	Switch the TEST flag from false to true, the game will enter test mode when started.
	In test mode, all collectible objects will be spawned close to the player upon start for easy testing of all weapons.
	One enemy will spawn close to spawn for testing.
	Token of Appreciation will spawn at fixed locations near the player so that it can be tested easily
	Pressing the button 'x' will teleport the player far from the spawn point.
	Pressing the button 'z' will immediately spawn enemies around the player.
	* be noted that test mode would not stop enemies and collectibles from spawning.

 - SPAWN_ENEMY
	Swap this flag to enable or disable enemy spawn. This also works outside test mode.
	This does not stop collectibles from spawning.


PROJECT REQUIREMENTS:
1. Game mechanics.
 - To win the game, the player must collect four tokens scattered around the map, then escape through the door that opens to win.
 - If the enemies manage to kill the player, it is game over.

2. Enemies.
 - There are three enemy types, each with their distinct weapon and behaviour. 
 - Enemies are controlled by statemachines. States of enemies are IDLE, WANDER, ALERT and ENGAGE. When a player enters the detection radius of an enemy in it's IDLE, WANDER or ALERT state,
   it immediately enters ENGAGE state and start attacking the player.
 - If the player manages to leave the detection radius during the ENGAGE state, enemies will enter WANDER state and continue moving towards the last known location of the player.
 - If an enemy took damage in IDLE or WANDER state, it enters ALERT state, stares at the damage direction and increases it's detection radius for a couple seconds, then it will go back to IDLE.

 - Enemy types:
	1. Melee enemy ship
		Rams the player with its sawblade, damaging the player.
		It aims for player's path of mothion (position + velocity).
	2. Ranged enemy ship
		If the player is too close to the enemy, it will move away from the player. If the player is too far away but still in it's engage radius, it will move towards the player.
		If the player is in it's shooting range, it will calculate the intersaction point of the bullet and the player based on the bullet's speed and player's velocity.
		It calculates the point where the bullet and player will intersect by the speed of the bullet and the position + velocity of the player. It then try to aim and shoots towards that position.
	3. Chunky enemy ship'
		It has a big cannon and will try to shoot the player when possible.
		When it's cannon is cooling down or the player gets into charging range, it prepares and charges at the player in a straight line.

 - Player and enemies take damage when bumping into each other, however player does not deal melee damage to the melee enemy throught the sawblade or to the chunky enemy while it's charging.

3. Weapons.
 - There are 4 different weapons the players could obtain. The player can also have more than 1 weapon attached, making them even more powerful.
	1. Normal turret (dark green) -- The default weapon the player spawns with. Fires at a medium pace, shooting bullets that deal medium damage. 
	2. Machine gun turret (light blue) -- Obtained through a blue powerup. Fires at a fast pace, shooting faster bullets that deals medium damage. Expires after 10 seconds.
	3. Homing turret (light green) -- Obtained through a green powerup. Fires at a slow pace, shooting out crawlie creatures that steers towards the nearest enemy and dealing medium damage. Expires after 20 seconds.
	4. Cannon (orange/white) -- Obtained through a powerup. Fires at a very slow pace, shooting out cannonballs that can pierce through enemies, dealing massive damage. This is a permanent upgrade.
		- All 3 turrets above will slowly turn towards the mouse, but the cannon is fixed to the player, always pointing behind it. 
		- Use spacebar to shoot the cannon, and it will propell the player in the opposite direction of where it shot.
	5. Big bullet -- only shot by the Chunky enemy ships. Behaves similarly to the cannon, except it does not pierce through anything.
 - Some bullets have a bullet trail and different scaling. Each bullet has a different texture.
 - The bullets uses ray-circle collision to handle collision with enemies.

4. Collectible items.
 - The collectibles in this game either serve as a win condition or as a power-up.
	- There are 4 win condition collectibles that spawn randomly in the world. They move in a parametric circular motion around an origin point.
	  The final exit door is technically also a collectible -- touching it will change to the win screen.
	- The other collectibles are power-ups, some with expiration timers and some permanent.
		1. Machine gun turret power-up. Changes the first turret on the player to a machine gun turret. Lasts 10 seconds before reverting back to a normal turret.
		2. Homing turret power-up. Changes the first turret on the player to a homing turret. Lasts 20 seconds before reverting back to a normal turret.
		3. Cannon power-up. Adds a cannon that points backwards permanently. (Only 1 cannon can exist at a time.)
		4. Four turrets power-up. Adds 3 normal turrets that point left, right, and back of the player. These turrets have health, can be damaged, and will eventually break.
		5. Shotgun powerup. Adds 2 normal turrest either side of the player's main turret. hese turrets have health, can be damaged, and will eventually break.
	- When there are more than 1 turret attached to the player, damaing the player will shift the damage to the turrets instead, acting like a shield.

5. Movement and transformations.
 - All movements in the game is handled through transformations and rotations in the game objects.
 - Everything is physically-based other than the turret turning and win condition movement, which rotate/move in a circle using parametric motion. 

6. Collision detection.
 - Enemies and collectibles (pick-up) collide with the player using circle-circle collision. Bullets collide with the player and enemies using ray-circle collision.

7. Game world.
 - Camera centers on the player, and the player can travel infinitely in any distance (although you don't need to). 
 - The background texture tiles seamlessly.

8. Particle systems.
 - Two instances of particle systems.
	1. Silver bullet trail particles that spread in one direction.
	2. Orange explosion particles that are large and radiate from a point.

9. UI.
 - There are several dynamic UI elements. The text is rendered using shaders.
	1. Health bar
	2. Health bar text -- displays current health value
	3. Weapon display -- displays the weapon(s) the player has, using the collectible texture as the icon
	4. Weapon display text -- "Your current weapons: " header.
	5. Instruction text -- changes from "Follow the arrow and collect the Tokens of Appreciation!" to "Get to the exit" depending on the current state of the game.
	6. Timer -- counts up the moment the player goes from start screen to game screen.
	7. Finish timer -- static text on the winning screen that displays the total time it took for the player to beat the game.
	8. Arrow -- an arrow that rotates around the player and points towards the nearest win condition collectible, or the exit door.

10. Advanced methods. (and 11. Bonus points.)
 - Collision resolution between enemies and players
	- Each game object has a weight assigned, and they will bounce off of one another. The enemies that ram into players with a lot of speed will make the bounce stronger.
		- The bullets will also make the enemies and players bounce back.
	- The homing bullets steer gradualy toward the nearest enemy.
	- Hierarchical transformation on the melee enemy (multiple section arms with the saw attatched to the end).