#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "Texture.hpp"

#include <memory>

class Game;

class Entity
{
protected:
	Game* game_;
	Texture* sprites_texture_;
	float scale_;

public:
	SDL_Rect bounding_box_;
	SDL_Rect sprites_clip_;

	Entity(Game* game, Texture* sprites_texture);

	virtual ~Entity();

	virtual void HandleEvent(SDL_Event* e) = 0;

	virtual void Tick() = 0;

	virtual void Render() = 0;

	virtual void Respawn() = 0;
};

#endif