#include "Player.hpp"
#include "Constants.hpp"
#include "Game.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include <iostream>

Player::Player(Game* game) : game_(game)
{
	bounding_box_.x = constants::screen_width / 5.0;
	bounding_box_.y = constants::screen_height / 5.0;
	bounding_box_.w = 60.0;
	bounding_box_.h = 140.0;

	grounded_ = false;
	speed_ = 10.0;
	vy_ = 0.0;
	ay_ = 0.0;
	mass_ = 3.0;
	Fy_ = 0.0;
	Fy_net_ = 0.0;

	frame_ = 0;

	sprite_clips_[0].x = 0;
	sprite_clips_[0].y = 0;
	sprite_clips_[0].w = 15;
	sprite_clips_[0].h = 35;

	sprite_clips_[1].x = 15;
	sprite_clips_[1].y = 0;
	sprite_clips_[1].w = 15;
	sprite_clips_[1].h = 35;

	current_clip_ = &sprite_clips_[0];

	texture_ = std::make_unique<Texture>();
	texture_->LoadFromPath(game_->renderer_, "res/gfx/player.png");

	jump_sfx_ = Mix_LoadWAV("res/sfx/jump.wav");
	pickup_sfx_ = Mix_LoadWAV("res/sfx/pickup.wav");
}

Player::~Player()
{
	if (texture_ != nullptr)
	{		
		texture_->FreeTexture();
	}

	Mix_FreeChunk(jump_sfx_);
	jump_sfx_ = nullptr;

	Mix_FreeChunk(pickup_sfx_);
	pickup_sfx_ = nullptr;
}

void Player::HandleEvent(SDL_Event* e)
{
	if (e->type == SDL_KEYDOWN)
	{
		// if (e->key.keysym.sym == SDLK_UP && e->key.repeat == 0)
		// {
		// 	Fy_ = -speed_;
		// }
		
		if ((e->key.keysym.sym == SDLK_SPACE || e->key.keysym.sym == SDLK_UP) && grounded_)
		{
			Mix_PlayChannel(-1, jump_sfx_, 0);
			vy_ = -30.0;
		}
	}
	else if (e->type == SDL_KEYUP && e->key.repeat == 0)
	{
		// if (e->key.keysym.sym == SDLK_UP)
		// {
		// 	Fy_ = 0.0;
		// }
	}
}

void Player::Tick()
{
	const int background_without_ground_h = 640;
	const float weight = mass_ * constants::g;
	Fy_net_ = Fy_ + weight;

	ay_ = Fy_net_ / mass_;
	vy_ += ay_;

	bounding_box_.y += vy_;
	grounded_ = Grounded();

	if (grounded_)
	{
		bounding_box_.y = background_without_ground_h - bounding_box_.h;
		ay_ = 0.0;
		vy_ = 0.0;
	}

	const int frame_divisor = 16;

	current_clip_ = &sprite_clips_[frame_ / frame_divisor];

	++frame_;

	if (frame_ / frame_divisor >= 2)
	{
		frame_ = 0;
	}

	for (const std::unique_ptr<Obstacle>& obstacle : game_->obstacles_)
	{
		if (Collides(obstacle->bounding_box_))
		{
			game_->Stop();
		}
	}

	for (const std::unique_ptr<BonusItem>& bonus_item : game_->bonus_items_)
	{
		if (Collides(bonus_item->bounding_box_))
		{
			Mix_PlayChannel(-1, pickup_sfx_, 0);
			bonus_item->Respawn();
			game_->score_ += 5;
		}
	}

	//std::cout << bounding_box_.x << " " << bounding_box_.y << "\n";
}

void Player::Render()
{
	texture_->Render(game_->renderer_, bounding_box_.x, bounding_box_.y, current_clip_, 4.0);

	// SDL_SetRenderDrawColor(game_->renderer_, 0xFF, 0x00, 0x00, 0xFF);
	// SDL_RenderDrawRectF(game_->renderer_, &bounding_box_);
}

bool Player::Grounded()
{
	return (bounding_box_.y + bounding_box_.h) >= game_->background_without_ground_h_;
}

bool Player::Collides(const SDL_Rect& obstacle_rect)
{
	SDL_Rect player_rect = { static_cast<int>(bounding_box_.x), static_cast<int>(bounding_box_.y), static_cast<int>(bounding_box_.w), static_cast<int>(bounding_box_.h) };

	return SDL_HasIntersection(&player_rect, &obstacle_rect);
}