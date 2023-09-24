#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

class Texture
{
public:
	SDL_Texture* texture_;
	int width_;
	int height_;

	Texture();

	~Texture();

	void FreeTexture();

	bool LoadFromPath(SDL_Renderer* renderer, const char* path);

	bool LoadFromText(SDL_Renderer* renderer, TTF_Font* font, const char* text, const SDL_Color& text_color, int text_length = -1);

	void Render(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip = nullptr, float scale = 1.0);
};

#endif