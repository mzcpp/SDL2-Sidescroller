#include "Obstacle.hpp"
#include "Game.hpp"
#include "Constants.hpp"

#include <cassert>
#include <iostream>

Obstacle::Obstacle(Game* game, Texture* sprites_texture, ObstacleType type, int x, float scale) : Entity(game, sprites_texture), type_(type)
{
	scale_ = scale;
	SetType(type);
	bounding_box_.x = x;
}

Obstacle::~Obstacle()
{
}

void Obstacle::HandleEvent(SDL_Event* e)
{
	(void) e;
}

void Obstacle::Tick()
{
	bounding_box_.x -= game_->scrolling_speed_;

	if (bounding_box_.x + bounding_box_.w < 0)
	{
		Respawn();
	}
}

void Obstacle::Render()
{
	switch (type_)
	{
	case ObstacleType::SINGLE_BOX:
		sprites_texture_->Render(game_->renderer_, bounding_box_.x, bounding_box_.y, &sprites_clip_, scale_);
		break;
	case ObstacleType::DOUBLE_BOX:
		sprites_texture_->Render(game_->renderer_, bounding_box_.x, bounding_box_.y, &sprites_clip_, scale_);
		sprites_texture_->Render(game_->renderer_, bounding_box_.x, bounding_box_.y + (sprites_clip_.h * scale_), &sprites_clip_, scale_);
		break;
	case ObstacleType::QUAD_BOX:
		sprites_texture_->Render(game_->renderer_, bounding_box_.x, bounding_box_.y + (sprites_clip_.h * scale_), &sprites_clip_, scale_);
		sprites_texture_->Render(game_->renderer_, bounding_box_.x + (sprites_clip_.w * scale_), bounding_box_.y + (sprites_clip_.h * scale_), &sprites_clip_, scale_);
		sprites_texture_->Render(game_->renderer_, bounding_box_.x + (sprites_clip_.w * scale_), bounding_box_.y, &sprites_clip_, scale_);
		sprites_texture_->Render(game_->renderer_, bounding_box_.x, bounding_box_.y, &sprites_clip_, scale_);
		break;
	case ObstacleType::FIRE:
		sprites_texture_->Render(game_->renderer_, bounding_box_.x, bounding_box_.y, &sprites_clip_, scale_);
		break;
	}

	// SDL_SetRenderDrawColor(game_->renderer_, 0xFF, 0x00, 0x00, 0xFF);
	// SDL_RenderDrawRect(game_->renderer_, &bounding_box_);
}

void Obstacle::Respawn()
{
	constexpr int distances[4] = { 400, 600, 800, 1000 };
	SetType(static_cast<ObstacleType>(game_->random_index_(game_->mt_)));
	
	auto max_it = std::max_element(game_->obstacles_.begin(), game_->obstacles_.end(), [](const std::unique_ptr<Obstacle>& o1, const std::unique_ptr<Obstacle>& o2)
	{
		return o1->bounding_box_.x < o2->bounding_box_.x;
	});

	assert(max_it != game_->obstacles_.end());

	bounding_box_.x = (*max_it)->bounding_box_.x + distances[game_->random_index_(game_->mt_)];
}

void Obstacle::SetType(ObstacleType type)
{
	type_ = type;

	const int sprite_side_size = 32;
	const int scaled_sprite_side_size = sprite_side_size * scale_;

	if (type == ObstacleType::FIRE)
	{
		sprites_clip_.x = 0;
	}
	else if (type == ObstacleType::SINGLE_BOX || type == ObstacleType::DOUBLE_BOX || type == ObstacleType::QUAD_BOX)
	{
		sprites_clip_.x = sprite_side_size;
	}

	sprites_clip_.y = 0;
	sprites_clip_.w = sprite_side_size;
	sprites_clip_.h = sprite_side_size;

	switch (type)
	{
	case ObstacleType::SINGLE_BOX:
		bounding_box_.w = scaled_sprite_side_size;
		bounding_box_.h = scaled_sprite_side_size;
		break;
	case ObstacleType::DOUBLE_BOX:
		bounding_box_.w = scaled_sprite_side_size;
		bounding_box_.h = scaled_sprite_side_size * 2;
		break;
	case ObstacleType::QUAD_BOX:
		bounding_box_.w = scaled_sprite_side_size * 2;
		bounding_box_.h = scaled_sprite_side_size * 2;
		break;
	case ObstacleType::FIRE:
		bounding_box_.w = scaled_sprite_side_size;
		bounding_box_.h = scaled_sprite_side_size;
		break;
	}

	bounding_box_.y = game_->background_without_ground_h_ - bounding_box_.h;
}
