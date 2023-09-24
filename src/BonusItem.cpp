#include "BonusItem.hpp"
#include "Game.hpp"
#include "Constants.hpp"

#include <iostream>

BonusItem::BonusItem(Game* game, Texture* sprites_texture, BonusItemType type, int x, float scale) : Entity(game, sprites_texture)
{
	scale_ = scale;
	SetType(type);
	bounding_box_.x = x;
}

BonusItem::~BonusItem()
{
}

void BonusItem::HandleEvent(SDL_Event* e)
{
	(void) e;
}

void BonusItem::Tick()
{
	bounding_box_.x -= game_->scrolling_speed_;

	if (bounding_box_.x + bounding_box_.w < 0)
	{
		Respawn();
	}
}

void BonusItem::Render()
{
	switch (type_)
	{
	case BonusItemType::MONEY:
		sprites_texture_->Render(game_->renderer_, bounding_box_.x, bounding_box_.y, &sprites_clip_, scale_);
		break;
	}
}

void BonusItem::Respawn()
{
	constexpr int distances[4] = { 200, 400, 600, 800 };
		
	auto max_it = std::max_element(game_->bonus_items_.begin(), game_->bonus_items_.end(), [](const std::unique_ptr<BonusItem>& b1, const std::unique_ptr<BonusItem>& b2)
	{
		return b1->bounding_box_.x < b2->bounding_box_.x;
	});

	assert(max_it != game_->bonus_items_.end());
	bounding_box_.x = (*max_it)->bounding_box_.x + distances[game_->random_index_(game_->mt_)];
}

void BonusItem::SetType(BonusItemType type)
{
	type_ = type;

	if (type == BonusItemType::MONEY)
	{
		sprites_clip_.x = 64;
		sprites_clip_.y = 0;
		sprites_clip_.w = 16;
		sprites_clip_.h = 8;
	}

	switch (type_)
	{
	case BonusItemType::MONEY:
		bounding_box_.y = constants::screen_height / 2;
		bounding_box_.w = 16 * scale_;
		bounding_box_.h = 8 * scale_;
		break;
	}
}
