#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Texture.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <memory>

class Game;

class Player
{
private:
	Game* game_;

	SDL_FRect bounding_box_;
	bool grounded_;
	float speed_;
	float vy_;
	float ay_;
	float mass_;
	float Fy_;
	float Fy_net_;

	Mix_Chunk* jump_sfx_;
	Mix_Chunk* pickup_sfx_;

	int frame_;
	SDL_Rect sprite_clips_[2];
	SDL_Rect* current_clip_;

	std::unique_ptr<Texture> texture_;

public:
	Player(Game* game);

	~Player();
	
	void HandleEvent(SDL_Event* e);

	void Tick();

	void Render();

	bool Grounded();

	bool Collides(const SDL_Rect& obstacle_rect);
};

#endif