#pragma once
#include <optional>

#include <glm/vec3.hpp>
#include "ChunkManager.h"	
#include "debugTools.h"

void resolveConstrains(glm::vec3 &pos, glm::vec3 lastPos, ChunkManager &cm, glm::vec3 dimensions, CubeWireRenderer *cw);

std::optional<glm::vec3> rayCast(ChunkManager &cm, glm::vec3 position, glm::vec3 direction, float maxLength);