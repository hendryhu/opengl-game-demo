#include "background_controller_object.h"

namespace game {

BackgroundControllerObject::BackgroundControllerObject(Geometry* geom, Shader* shader, GLuint* texture, GameObject* player)
	: GameObject(glm::vec3(0, 0, 0), geom, shader, texture[0]) {
	tile_textures_ = texture;
	this->player = player;
	// add first tile
	BackgroundTileObject* tile = new BackgroundTileObject(glm::vec3(0, 0, 5), geom, shader, texture[0], player);
	tile->SetScale(tile_scale);
	tiles_.push_back(tile);

	// spawn distance
	spawn_distance = std::max(100 / tile_scale, tile_scale * 2);

	// render distance
	render_distance = std::max(50.0f, tile_scale * 2);
}


BackgroundControllerObject::~BackgroundControllerObject() {
	for (int i = 0; i < tiles_.size(); i++) {
		delete tiles_[i];
	}
}


void BackgroundControllerObject::Update(double delta_time) {
	// if player is within spawn_distance, new tiles are spawned

	// spawn tiles
	for (int i = 0; i < tiles_.size(); i++) {
		BackgroundTileObject* target_tile = tiles_[i];
		if (!target_tile->isVisited()) {
			glm::vec3 target_position = target_tile->GetPosition();
			target_position.z = 0.0f;
			// get distance to player
			float distance = glm::length(target_position - player->GetPosition());
			if (distance <= spawn_distance) {		
				// spawn neighbouring tiles
				glm::vec3 new_position;

				// top left tile
				new_position = glm::vec3(target_position.x - tile_scale, target_position.y + tile_scale, 5.0f);
				spawnTile(new_position);
				
				// top tile
				new_position = glm::vec3(target_position.x, target_position.y + tile_scale, 5.0f);
				spawnTile(new_position);

				// top right tile
				new_position = glm::vec3(target_position.x + tile_scale, target_position.y + tile_scale, 5.0f);
				spawnTile(new_position);

				// right
				new_position = glm::vec3(target_position.x + tile_scale, target_position.y, 5.0f);
				spawnTile(new_position);

				// bottom right tile
				new_position = glm::vec3(target_position.x + tile_scale, target_position.y - tile_scale, 5.0f);
				spawnTile(new_position);

				// bottom tile
				new_position = glm::vec3(target_position.x, target_position.y - tile_scale, 5.0f);
				spawnTile(new_position);

				// bottom left tile
				new_position = glm::vec3(target_position.x - tile_scale, target_position.y - tile_scale, 5.0f);
				spawnTile(new_position);

				// left tile
				new_position = glm::vec3(target_position.x - tile_scale, target_position.y, 5.0f);
				spawnTile(new_position);

				// set flag
				target_tile->visit();
			}
		}
	}

	// parent's update
	GameObject::Update(delta_time);
}


void BackgroundControllerObject::Render(glm::mat4 view_matrix, double current_time) {
	for (int i = 0; i < tiles_.size(); i++) {
		
		// if player is within render_distance, tiles are rendered
		glm::vec3 target_position = tiles_[i]->GetPosition();
		target_position.z = 0.0f;
		float distance = glm::length(target_position - player->GetPosition());
		if (distance <= render_distance) {
			tiles_[i]->Render(view_matrix, current_time);
		}
	}
}


// helpers

bool BackgroundControllerObject::inSet(std::set<std::pair<float, float>> s, float x, float y) {
	return s.find(std::make_pair(x, y)) != s.end();
}


void BackgroundControllerObject::spawnTile(glm::vec3 tile_position) {
	if (!inSet(spawned_tiles_, tile_position.x, tile_position.y)) {
		BackgroundTileObject* new_tile = new BackgroundTileObject(tile_position, this->geometry_, this->shader_, tile_textures_[0], player);
		new_tile->SetScale(tile_scale);
		tiles_.push_back(new_tile);
		spawned_tiles_.insert(std::make_pair(tile_position.x, tile_position.y));
	}
}

} // namespace game