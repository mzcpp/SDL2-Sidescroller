#ifndef OBSTACLE_HPP
#define OBSTACLE_HPP

#include "Entity.hpp"

class Game;

enum class ObstacleType
{
	SINGLE_BOX, DOUBLE_BOX, QUAD_BOX, FIRE
};

class Obstacle : public Entity
{
private:
	ObstacleType type_;
	
public:
	Obstacle(Game* game, Texture* sprites_texture, ObstacleType type, int x, float scale);

	~Obstacle();

	void HandleEvent(SDL_Event* e) override;

	void Tick() override;

	void Render() override;

	void Respawn() override;

	void SetType(ObstacleType type);
};

#endif