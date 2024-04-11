#ifndef COLLISION_HANDLER_
#define COLLISION_HANDLER_

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <vector>
#include <iostream>

#include "config.h"
#include "helper.h"

#include "bullet_object.h"
#include "charge_enemy_object.h"
#include "enemy_game_object.h"
#include "explosion.h"
#include "game_object.h"
#include "helper.h"
#include "melee_enemy_object.h"
#include "player_game_object.h"
#include "ranged_enemy_object.h"
#include "saw_object.h"
#include "turret_object.h"
#include "collectible_game_object.h"

using namespace std;

namespace game {

	class CollisionHandler {
	public:
		CollisionHandler();
		~CollisionHandler();

		void setUp(vector <GameObject*>* player_vector, vector <GameObject*>* enemy_vector,
                   vector <GameObject*>* bullet_vector, vector <GameObject*>* collectible_vector,
                   vector <TurretObject*>& turret_templates,
				   CollectibleGameObject** win_1, CollectibleGameObject** win_2,
				   CollectibleGameObject** win_3, CollectibleGameObject** win_4);

		void Update(double delta_time);

		void playerCollision(double delta_time);
		void enemyCollision(double delta_time);
		void bulletCollision(double delta_time);
        void collectibleCollision(double delta_time);

        // Handle collectible function
        void handleCollectible(CollectibleGameObject* collectible, PlayerGameObject* player);

		// Line collision check
			// returns index in vector for collided object, -1 if no collision
		int checkBulletCollision(BulletObject* bullet, std::vector<GameObject*> target, double delta_time);

	private:
        // win collectible pointers
		std::vector<CollectibleGameObject**> win_collectible_objects_;

		// Groups

		// Player
		vector <GameObject*>* player_objects_;

		// Enemies
		vector <GameObject*>* enemy_objects_;

		// Bullet
		vector <GameObject*>* bullet_objects_;

        // Collectibles
        vector <GameObject*>* collectible_objects_;

        // Turret templates
        vector <TurretObject*> turret_templates_;

	}; // class CollisionHandler

} // namespace game

#endif