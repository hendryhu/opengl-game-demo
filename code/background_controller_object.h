#ifndef BACKGROUND_CONTROLLER_OBJECT_H_
#define BACKGROUND_CONTROLLER_OBJECT_H_

#include <vector>
#include <set>
#include <utility>
#include <iostream>

#include "game_object.h"
#include "background_tile_object.h"


namespace game {

	class BackgroundControllerObject : public GameObject {
	public:
		// takes background textures and a player reference
		BackgroundControllerObject(Geometry* geom, Shader* shader, GLuint* texture, GameObject* player);

		~BackgroundControllerObject();

		void Update(double delta_time) override;

		void Render(glm::mat4 view_matrix, double current_time) override;

		// helpers
		bool inSet(std::set<std::pair<float, float>> s, float x, float y);
		void spawnTile(glm::vec3 tile_position);

	private:
		// properties
		float tile_scale = 20;
		float spawn_distance;
		float render_distance;

		// background tiles
		std::vector<BackgroundTileObject*> tiles_;

		// visited coordinates (pair(x,y))
		// note* consider using unordered_set for better performance
		std::set<std::pair<float, float>> spawned_tiles_;

		// texture
		GLuint* tile_textures_;

		// player pointer
		GameObject* player;

	}; // class BackgroundControllerObject
} // namespace game

#endif // BACKGROUND_CONTROLLER_OBJECT_H_