#include "background_tile_object.h"

namespace game {

BackgroundTileObject::BackgroundTileObject(const glm::vec3& position, Geometry* geom, Shader* shader, GLuint texture, GameObject* player)
	: GameObject(position, geom, shader, texture) {}

} // namespace game