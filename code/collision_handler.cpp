#include "collision_handler.h"

namespace game {

	CollisionHandler::CollisionHandler() {}

	CollisionHandler::~CollisionHandler() {}


	void CollisionHandler::setUp(vector <GameObject*>* player_vector, vector <GameObject*>* enemy_vector,
                                 vector <GameObject*>* bullet_vector, vector <GameObject*>* collectible_vector,
                                 vector <TurretObject*>& turret_templates,
								 CollectibleGameObject** win_1, CollectibleGameObject** win_2,
								 CollectibleGameObject** win_3, CollectibleGameObject** win_4) {
		player_objects_ = player_vector;
		enemy_objects_ = enemy_vector;
		bullet_objects_ = bullet_vector;
        collectible_objects_ = collectible_vector;
        turret_templates_ = turret_templates;
		win_collectible_objects_.push_back(win_1);
		win_collectible_objects_.push_back(win_2);
		win_collectible_objects_.push_back(win_3);
		win_collectible_objects_.push_back(win_4);
	}

	
	void CollisionHandler::Update(double delta_time) {
		playerCollision(delta_time);
		enemyCollision(delta_time);
		bulletCollision(delta_time);
        collectibleCollision(delta_time);
	}

	// Player Collision

	void CollisionHandler::playerCollision(double delta_time) {
		for (int i = 0; i < player_objects_->size(); i++) {
			PlayerGameObject* current_player_object = dynamic_cast<PlayerGameObject*>((*player_objects_)[i]);
			// Enemy
			for (int j = 0; j < enemy_objects_->size(); j++) {
				EnemyGameObject* current_enemy_object = dynamic_cast<EnemyGameObject*>((*enemy_objects_)[j]);

				// hitbox collision
				float distance_to_enemy = glm::length(current_player_object->GetPosition() - current_enemy_object->GetPosition());
				float hitbox_distance = current_player_object->getHitbox() + current_enemy_object->getHitbox();
				if (distance_to_enemy <= hitbox_distance) {
					// collides
					current_enemy_object->knockback(current_player_object, 1.0f);
					current_enemy_object->takeDamage(current_player_object->getMeleeDamage(), current_player_object->GetPosition());
					
					if (current_enemy_object->isCharging()) {
						current_player_object->knockback(current_enemy_object, 3.0f);
						current_player_object->takeDamage(current_enemy_object->getChargeDamage());
						current_enemy_object->stopCharging();
					}
					else {
						current_player_object->knockback(current_enemy_object, 1.0f);
						current_player_object->takeDamage(current_enemy_object->getMeleeDamage());
					}
				}

				// saw collision
				if (current_enemy_object->isMeleeType()) {
					SawObject* current_saw_object = dynamic_cast<MeleeEnemyObject*>(current_enemy_object)->getSaw();
					float distance_to_saw = glm::length(current_player_object->GetPosition() - current_saw_object->GetPosition());
					hitbox_distance = current_player_object->getHitbox() + current_saw_object->getHitbox();
					if (distance_to_saw <= hitbox_distance) {
						// collides
						current_player_object->knockback(current_saw_object, 1.0f);
						current_player_object->takeDamage(current_saw_object->getDamage());
						current_enemy_object->knockback(current_player_object, 0.2f);
					}
				}
			}
		}
	}


	// Enemy Collision

	void CollisionHandler::enemyCollision(double delta_time) {
		for (int i = 0; i < enemy_objects_->size(); i++) {
			EnemyGameObject* current_enemy_object = dynamic_cast<EnemyGameObject*>((*enemy_objects_)[i]);
			// Enemy
			for (int j = i; j < enemy_objects_->size(); j++) {
				EnemyGameObject* target_enemy_object = dynamic_cast<EnemyGameObject*>((*enemy_objects_)[j]);

				// hitbox collision
				float distance_to_enemy = glm::length(current_enemy_object->GetPosition() - target_enemy_object->GetPosition());
				float hitbox_distance = current_enemy_object->getHitbox() + target_enemy_object->getHitbox();
				if (distance_to_enemy <= hitbox_distance) {
					if (current_enemy_object->isCharging()) {
						if (!target_enemy_object->isCharging()) {
							target_enemy_object->knockback(current_enemy_object, 1.0f);
						}

					}
					else if (target_enemy_object->isCharging()) {
						current_enemy_object->knockback(target_enemy_object, 1.0f);
					}
					else {
						current_enemy_object->knockback(target_enemy_object, 0.1f);
						target_enemy_object->knockback(current_enemy_object, 0.1f);
					}
				}
			}
		}
	}


	// Bullet Collision

	void CollisionHandler::bulletCollision(double delta_time) {
		for (int i = 0; i < bullet_objects_->size(); i++) {
			BulletObject* current_bullet = dynamic_cast<BulletObject*>((*bullet_objects_)[i]);
			ObjectType shooter_type = current_bullet->GetShooterType();
			
			if (shooter_type == ObjectType::Player) {
				// Enemy
				int index = checkBulletCollision(current_bullet, *enemy_objects_, delta_time);
				if (index != -1) {
					EnemyGameObject* enemy_hit = dynamic_cast<EnemyGameObject*>((*enemy_objects_)[index]);
                    if (current_bullet->GetBulletType() == BulletType::CANNONBALL) {
                        bool hit = current_bullet->Pierce(enemy_hit);
                        if (hit)
                        {
                            enemy_hit->takeDamage(current_bullet->GetDamage(), current_bullet->GetPosition());
                            enemy_hit->knockback(current_bullet, 1.0f);
                        }
                    }
                    else {
                        enemy_hit->takeDamage(current_bullet->GetDamage(), current_bullet->GetPosition());
                        enemy_hit->knockback(current_bullet, 1.0f);
                        Helper::removeByIndex(bullet_objects_, i);
                        i--;
                    }
					continue;
				}
			}
			else if (shooter_type == ObjectType::Enemy) {
				// Player
				int index = checkBulletCollision(current_bullet, *player_objects_, delta_time);
				if (index != -1) {
					PlayerGameObject* player = dynamic_cast<PlayerGameObject*>((*player_objects_)[index]);
					player->takeDamage(current_bullet->GetDamage());
					player->knockback(current_bullet, 0.5);
					Helper::removeByIndex(bullet_objects_, i);
					i--;
					continue;
				}
			}
		}
	}


	// Bullet Collision check

	int CollisionHandler::checkBulletCollision(BulletObject* bullet, std::vector<GameObject*> target, double delta_time) {
		int index = -1;
		float collision_time = numeric_limits<float>::max();

		glm::vec3 bullet_velocity = bullet->GetDirection() * bullet->GetSpeed();
		glm::vec3 bullet_position = bullet->GetPosition();
		for (int i = 0; i < target.size(); i++) {
			glm::vec3 target_position = target[i]->GetPosition();
			float target_collision_radius = target[i]->getHitbox();

			float a = glm::dot(bullet_velocity, bullet_velocity);
			float b = glm::dot((2.0f * bullet_velocity), (bullet_position - target_position));
			float c = glm::dot((bullet_position - target_position), (bullet_position - target_position)) - (pow(target_collision_radius, 2));

			float t1 = (-b + sqrt(pow(b, 2) - (4 * a * c))) / (2 * a);
			float t2 = (-b - sqrt(pow(b, 2) - (4 * a * c))) / (2 * a);
			
			float t;
			if (t1 >= 0 && t2 >= 0) {
				t = min(t1, t2);
			}
			else if (t1 >= 0 && t2 < 0) {
				t = t1;
			}
			else if (t1 < 0 && t2 >= 0) {
				t = t2;
			}
			else {
				t = -1;
			}

			if (t >= 0 && t <= delta_time) {
				// will collide
				if (t < collision_time) {
					// closest to source
					index = i;
					collision_time = t;
				}
			}
		}
		return index;
	}


    // Collectible Collision
    void CollisionHandler::collectibleCollision(double delta_time)
    {
        for (int i = 0; i < collectible_objects_->size(); i++)
        {
            CollectibleGameObject* current_collectible = dynamic_cast<CollectibleGameObject *>((*collectible_objects_)[i]);
            for (auto & player_object : *player_objects_)
            {
                auto* current_player = dynamic_cast<PlayerGameObject*>(player_object);

                // hitbox collision
                float distance_to_collectible = glm::length(current_player->GetPosition() - current_collectible->GetPosition());
                float hitbox_distance = current_player->getHitbox() + current_collectible->getHitbox();
                if (distance_to_collectible <= hitbox_distance)
                {
                    // collides
                    handleCollectible(current_collectible, current_player);
                    Helper::removeByIndex(collectible_objects_, i);
                    i--;
                    break;
                }
            }
        }
    }


    // Handle collectible function
    void CollisionHandler::handleCollectible(CollectibleGameObject* collectible, PlayerGameObject* player)
    {
        // switch statement to handle different types of collectibles
        switch (collectible->GetType()) {
            case CollectibleType::FOUR_TURRETS: {
                // if the player has too many turrets, do nothing
                if (player->GetTurrets()->size() >= 9)
                    break;

                // add 3 turrets to the player, each with its own offset
                auto* new_turret1 = new TurretObject(*turret_templates_.at(0));
                new_turret1->SetAngleOffset(glm::pi<float>() / 2.0f);
                new_turret1->SetParentRotationOffset(-glm::pi<float>() / 2.0f);
                player->AddChildObject(new_turret1);
                auto* new_turret2 = new TurretObject(*turret_templates_.at(0));
                new_turret2->SetAngleOffset(3 * glm::pi<float>() / 2.0f);
                new_turret2->SetParentRotationOffset(0);
                player->AddChildObject(new_turret2);
                auto* new_turret3 = new TurretObject(*turret_templates_.at(0));
                new_turret3->SetAngleOffset(glm::pi<float>());
                new_turret3->SetParentRotationOffset(glm::pi<float>());
                player->AddChildObject(new_turret3);
                break;
            }
            case CollectibleType::WIN_CONDITION: {
				for (int i = 0; i < win_collectible_objects_.size(); i++) {
					if (*win_collectible_objects_[i] == collectible) {
                        *win_collectible_objects_[i] = nullptr;
						break;
					}
				}
                break;
            }
            case CollectibleType::MACHINE_GUN: {
                player->SetTurret(turret_templates_.at(1));
                break;
            }
            case CollectibleType::HOMING: {
                player->SetTurret(turret_templates_.at(3));
                break;
            }
            case CollectibleType::SHOTGUN: {

                // if the player has too many turrets, do nothing
                if (player->GetTurrets()->size() >= 9)
                    break;

                // add 4 turrets to the player, each with its own offset
                auto* new_turret1 = new TurretObject(*turret_templates_.at(0));
                new_turret1->SetAngleOffset(glm::pi<float>() / 2.0f / 3.0f);
                new_turret1->SetParentRotationOffset(glm::pi<float>() / 2.0f / 3.0f + glm::pi<float>() / 2.0f );
                player->AddChildObject(new_turret1);
                auto* new_turret2 = new TurretObject(*turret_templates_.at(0));
                new_turret2->SetAngleOffset(- (glm::pi<float>() / 2.0f / 3.0f));
                new_turret2->SetParentRotationOffset(- (glm::pi<float>() / 2.0f / 3.0f)  + glm::pi<float>() / 2.0f);
                player->AddChildObject(new_turret2);
                auto* new_turret3 = new TurretObject(*turret_templates_.at(0));
                new_turret3->SetAngleOffset(glm::pi<float>() / 2.0f / 4.0f);
                new_turret3->SetParentRotationOffset(glm::pi<float>() / 2.0f / 4.0f  + glm::pi<float>() / 2.0f);
                player->AddChildObject(new_turret3);
                auto* new_turret4 = new TurretObject(*turret_templates_.at(0));
                new_turret4->SetAngleOffset(-(glm::pi<float>() / 2.0f / 4.0f));
                new_turret4->SetParentRotationOffset(-(glm::pi<float>() / 2.0f / 4.0f)  + glm::pi<float>() / 2.0f);
                player->AddChildObject(new_turret4);
                break;
            }
            case CollectibleType::CANNON: {
                // if player already has a cannon, do nothing
                for (auto & turret : *player->GetTurrets()) {
                    if (turret->GetType() == TurretType::CANNON) {
                        return;
                    }
                }
                player->AddChildObject(new TurretObject(*turret_templates_.at(2)));
                break;
            }
            default:
                break;
        }
    }


} // namespace game