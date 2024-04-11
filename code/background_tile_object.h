#ifndef BACKGROUND_GAME_OBJECT_H_
#define BACKGROUND_GAME_OBJECT_H_

#include <vector>

#include "game_object.h"

namespace game {

	// Inherits from GameObject
	class BackgroundTileObject : public GameObject {
	public:
		BackgroundTileObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, GameObject* player);

		// Getters
		inline bool isVisited() { return visited; }

		// Setters
		inline void visit() { visited = true; }

	private:
		// player reference
		GameObject* player;


		// Flags
		bool visited = false;

	}; // class BackgroundGameObject
} // namespace game

#endif // BACKGROUND_GAME_OBJECT_H_