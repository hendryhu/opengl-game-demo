#ifndef HELPER_H_
#define HELPER_H_

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/constants.hpp>

#include <random>
#include <ctime>
#include <algorithm>

#include <iostream>

using namespace std;

namespace game {

	class Helper {

	public:
		static float angleBetweenVectors(glm::vec3 a, glm::vec3 b);

		// normalize angle to [-pi, pi]
		static float normalizeAngle(float angle);
		// normalize angle to [0, 2pi]
		static float normalizeAngleToPositive(float angle);


		// Random
		static void setSeed();
		static glm::vec3 generate2dPosition(float xLowerbound, float xUpperbound, float yLowerbound, float yUpperbound);
		static glm::vec3 generate2dPosition(glm::vec3 start_position, float min_range, float max_range);
		static float generateFloat(float lowerbound, float upperbound);
		static int generateInt(int lowerbound, int upperbound);

        // Remove an object from a vector by index, then free the memory
        template <typename T>
		static void removeByIndex(vector<T*>* vec, int index) {
			if (index < 0 || index >= (*vec).size()) {
				return;
			}
			delete (*vec)[index];
			(*vec).erase((*vec).begin() + index);
		};

		// takes a chance of success (0 - 100) and returns true if successful
		static bool roll(float successChance);

	}; // class Helper

} // namespace game

#endif