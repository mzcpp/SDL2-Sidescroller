#include "Entity.hpp"

Entity::Entity(Game* game, Texture* sprites_texture) : game_(game), sprites_texture_(sprites_texture)
{
	bounding_box_.x = 0;
	bounding_box_.y = 0;
	bounding_box_.w = 0;
	bounding_box_.h = 0;

	sprites_clip_.x = 0;
	sprites_clip_.y = 0;
	sprites_clip_.w = 0;
	sprites_clip_.h = 0;
}

Entity::~Entity()
{
}
