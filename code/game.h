#ifndef GAME_H_
#define GAME_H_

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "config.h"

#include "background_controller_object.h"
#include "background_sprite.h"
#include "bullet_object.h"
#include "charge_enemy_object.h"
#include "collision_handler.h"
#include "explosion.h"
#include "enemy_game_object.h"
#include "collectible_game_object.h"
#include "game_object.h"
#include "health_bar_sprite.h"
#include "helper.h"
#include "melee_enemy_object.h"
#include "ranged_enemy_object.h"
#include "particles.h"
#include "particle_system.h"
#include "player_game_object.h"
#include "turret_object.h"
#include "shader.h"
#include "sprite.h"
#include "ui_object.h"
#include "text_game_object.h"

namespace game {

    // A class for holding the main game objects
    class Game {

        public:
            // Constructor and destructor
            Game(void);
            ~Game();

            // Call Init() before calling any other method
            // Initialize graphics libraries and main window
            void Init(void); 

            // Set up the game (scene, game objects, etc.)
            void Setup(void);

            // Run the game (keep the game active)
            void MainLoop(void); 

        private:
            // Main window: pointer to the GLFW window structure
            GLFWwindow *window_;

            // Sprite geometry
            Geometry *sprite_;

            // Particle geometry;
            Geometry* explosion_particles_;

            Geometry* bullet_particles_;

            // Background Sprite
            Geometry* background_sprite_;

            // Particle geometry
            Geometry *particles_;

            // Health bar geometry
            Geometry* health_bar_sprite_;

            // Shader for rendering sprites in the scene
            Shader sprite_shader_;

            // Shader for rendering particles
            Shader particle_shader_;

            // Shader for rendering ui
            Shader ui_shader_;

            // shader for text
            Shader text_shader_;


            // Textures
            
            // main
            GLuint* tex_;

            // Groups
            // Player
            GLuint player_body_tex_;

            // Turret
            GLuint turret_tex_;

            // Bullet
            GLuint bullet_tex_;
            
            // Background
            GLuint* background_tex_;

            // End screen Lose
            GLuint end_screen_lose_tex_;

            // End screen Win
            GLuint end_screen_win_tex_;

            // Start screen
            GLuint start_screen_tex_;

            // Enemies
            GLuint melee_enemy_tex_;

            GLuint ranged_enemy_tex_;

            GLuint charge_enemy_tex_;

            GLuint arm_tex_;

            GLuint saw_tex_;

            // Particles
            GLuint particle_tex_;

            // Arrow
            GLuint arrow_tex_;

            // Collision Handler
            CollisionHandler collision_handler;


            // GameObjects
            // weapon display objects
            vector <GameObject*> weapon_display_objects_;

            // main
            std::vector <std::vector<GameObject*>*> game_objects_;

            // Groups
            
            // Player
            std::vector <GameObject*> player_objects_;
            // Player body
            PlayerGameObject* player;

            // Enemies
            std::vector <GameObject*> enemy_objects_;

            // Background
            std::vector <GameObject*> background_objects_;

            // Endscreen
            GameObject* end_screen_lose_;
            GameObject* end_screen_win_;

            // Start screen
            GameObject* start_screen_;

            // Bullet templates
            std::vector <BulletObject*> bullet_templates_;

            // Current bullets
            std::vector <GameObject*> bullet_objects_;

            // Explosion effects
            std::vector <GameObject*> explosion_objects_;
            ParticleSystem* explosion_particle; // for deep copy

            // Bullet particles
            ParticleSystem* bullet_particle;    // for deep copy

            // Collectibles
            std::vector <GameObject*> collectible_objects_;

            // text objects
            std::vector <GameObject*> text_objects_;

            // Turrets
            // 1. default turret
            std::vector <TurretObject*> turret_templates_;

            // UI
            std::vector <GameObject*> ui_objects_;

            // Four winning condition collectibles
            CollectibleGameObject* win_1;
            CollectibleGameObject* win_2;
            CollectibleGameObject* win_3;
            CollectibleGameObject* win_4;
            CollectibleGameObject* exit_door;
            bool exit_door_opened = false;

            // Keep track of time
            double current_time_;

            // Keep track of player input
            float front_input = 0.0f;
            float back_input = 0.0f;
            float left_input = 0.0f;
            float right_input = 0.0f;
            glm::vec2 movement_input = glm::vec2(0.0f, 0.0f);   // (front, right)

            // Game State
            int state_;

            // Callback for when the window is resized
            static void ResizeCallback(GLFWwindow* window, int width, int height);

            // Set a specific texture
            void SetTexture(GLuint w, const char *fname);

            // Load all textures
            void SetAllTextures();

            // Handle user input for different game states
            void HandleControlsInGame(double delta_time);
            void HandleControlsEndScreen(double delta_time);
            void HandleControlsStartScreen(double delta_time);

            // Update all the game objects
            void Update(double delta_time);
 
            // Render the game world
            void Render(void);

            // Object Spawner
            void spawnExplosion(glm::vec3 position, float scale);
            void spawnEnemy(glm::vec3 position, EnemyType enemy_type);
            void spawnEnemyNearPlayer();
            void spawnEnemyNearCollectible();

            void spawnCollectible();
            CollectibleGameObject* spawnWinningCollectible(GLuint texture);

            Timer player_enemy_spawn_timer;

            Timer collectible_1_enemy_spawn_timer;
            Timer collectible_2_enemy_spawn_timer;
            Timer collectible_3_enemy_spawn_timer;
            Timer collectible_4_enemy_spawn_timer;

            Timer collectible_spawn_timer;

            double start_time = 0.0;
            double finish_time = -1.0;

    }; // class Game

} // namespace game

#endif // GAME_H_
