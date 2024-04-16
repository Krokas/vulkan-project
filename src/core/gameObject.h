#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <memory>
#include <unordered_map>

// #include "MovementComponent.h"
#include "model.h"
#include "textComponent.h"

struct TransformComponent {
  glm::vec3 translation{};
  glm::vec3 scale{1.0f, 1.0f, 1.0f};
  glm::vec3 rotation{};

  // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
  // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
  // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
  glm::mat4 mat4();
  glm::mat3 normalMatrix();
};

struct PointLightComponent {
  float lightIntensity = 1.0f;
};

struct TerrainComponent {
  bool isTerrain = true;  // Temp
};

struct PlayerComponent {
  glm::vec3 color = glm::vec3(1.0f);  // Temp
  glm::vec3 cameraOffset{};
};

class GameObject {
 public:
  using id_t = unsigned int;
  using Map = std::unordered_map<id_t, GameObject>;

  static GameObject createGameObject() {
    static id_t currentId = 0;
    return GameObject{currentId++};
  }

  static GameObject makePointLight(
      float intensity = 10.0f, float radius = 0.1f, glm::vec3 color = glm::vec3(1.f));
  static GameObject makeTerrain();
  static GameObject makePlayer();
  static GameObject makeText(std::shared_ptr<Font> font);

  GameObject(const GameObject&) = delete;
  GameObject& operator=(const GameObject&) = delete;
  GameObject(GameObject&&) = default;
  GameObject& operator=(GameObject&&) = default;

  id_t getId() { return id; }

  glm::vec3 color{};
  TransformComponent transform{};

  // optional
  std::shared_ptr<Model> model{};
  std::unique_ptr<PointLightComponent> pointLight = nullptr;
  std::unique_ptr<TerrainComponent> terrain = nullptr;
  std::unique_ptr<PlayerComponent> player = nullptr;
  //   std::unique_ptr<MovementComponent> movement = nullptr;
  std::unique_ptr<TextComponent> text = nullptr;

 private:
  GameObject(id_t id) : id{id} {}

  id_t id;
};
