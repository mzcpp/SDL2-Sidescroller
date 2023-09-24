#ifndef GAME_HPP
#define GAME_HPP

#include "Texture.hpp"
#include "Player.hpp"
#include "Obstacle.hpp"
#include "BonusItem.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include <memory>
#include <vector>
#include <random>

class Game
{
private:
	bool initialized_;
	bool running_;
	int ground_scrolling_offset_;

	std::unique_ptr<Player> player_;
	std::unique_ptr<Texture> background_texture_;
	std::unique_ptr<Texture> sprites_texture_;
	std::unique_ptr<Texture> score_info_;
	std::unique_ptr<Texture> game_over_info_;

public:
	bool game_over_;
	int score_;
	int scrolling_speed_;
	int background_without_ground_h_;

	std::vector<std::unique_ptr<Obstacle>> obstacles_;
	std::vector<std::unique_ptr<BonusItem>> bonus_items_;

	std::mt19937_64 mt_;
	std::uniform_int_distribution<int> random_index_;

	TTF_Font* font_;
	SDL_Window* window_;
	SDL_Renderer* renderer_;

	Game();

	~Game();

	bool Initialize();

	void Finalize();

	bool InitAssets();

	void SpawnObjects();

	void Run();

	void HandleEvents();

	void Tick();

	void Render();

	void Stop();

	void Reset();

	void UpdateScoreText();
};

#endif