#ifndef BONUS_ITEM_HPP
#define BONUS_ITEM_HPP

#include "Entity.hpp"

class Game;

enum class BonusItemType
{
	MONEY
};

class BonusItem : public Entity
{
private:
	BonusItemType type_;

public:
	BonusItem(Game* game, Texture* sprites_texture, BonusItemType type, int x, float scale);

	~BonusItem();

	void HandleEvent(SDL_Event* e) override;

	void Tick() override;

	void Render() override;

	void Respawn() override;

	void SetType(BonusItemType type);
};

#endif
