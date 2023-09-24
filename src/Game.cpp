#include "Game.hpp"
#include "Constants.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <cstdint>
#include <iostream>
#include <string>
	
Game::Game() : 
	initialized_(false), 
	running_(false), 
	ground_scrolling_offset_(0), 
	player_(nullptr), 
	background_texture_(std::make_unique<Texture>()), 
	sprites_texture_(std::make_unique<Texture>()), 
	score_info_(std::make_unique<Texture>()), 
	game_over_info_(std::make_unique<Texture>()), 
	game_over_(false), 
	score_(0), 
	scrolling_speed_(10), 
	background_without_ground_h_(640), 
	mt_(std::random_device{}()), 
	random_index_(0, 3), 
	font_(nullptr), 
	window_(nullptr), 
	renderer_(nullptr)
{
	initialized_ = Initialize();
}

Game::~Game()
{
	Finalize();
}

bool Game::Initialize()
{
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not be initialized! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0"))
	{
		printf("%s\n", "Warning: Texture filtering is not enabled!");
	}

	window_ = SDL_CreateWindow(constants::game_title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, constants::screen_width, constants::screen_height, SDL_WINDOW_SHOWN);

	if (window_ == nullptr)
	{
		printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);

	if (renderer_ == nullptr)
	{
		printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
		return false;
	}

	SDL_SetRenderDrawColor(renderer_, 0xFF, 0xFF, 0xFF, 0xFF);

	constexpr int img_flags = IMG_INIT_PNG;

	if (!(IMG_Init(img_flags) & img_flags))
	{
		printf("SDL_image could not be initialized! SDL_image Error: %s\n", IMG_GetError());
		return false;
	}

	if (TTF_Init() == -1)
	{
		printf("SDL_ttf could not be initialized! SDL_ttf Error: %s\n", TTF_GetError());
		return false;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		printf("SDL_mixer could not be initialized! SDL_mixer Error: %s\n", Mix_GetError());
		return false;
	}

	SpawnObjects();

	return InitAssets();
}

void Game::Finalize()
{
	SDL_DestroyWindow(window_);
	window_ = nullptr;

	SDL_DestroyRenderer(renderer_);
	renderer_ = nullptr;

	TTF_CloseFont(font_);
	font_ = nullptr;

	IMG_Quit();
	SDL_Quit();
	TTF_Quit();
	Mix_Quit();
}

bool Game::InitAssets()
{
	background_texture_->LoadFromPath(renderer_, "res/gfx/background.png");
	sprites_texture_->LoadFromPath(renderer_, "res/gfx/objects.png");

	font_ = TTF_OpenFont("res/font/font.ttf", 28);

	if (font_ == nullptr)
	{
		printf("Failed to load font! SDL_ttf Error: %s\n", TTF_GetError());
		return false;
	}

	SDL_Color text_color = { 0x00, 0x00, 0x00, 0xFF };
	game_over_info_->LoadFromText(renderer_, font_, "GAME OVER. Press 'R' to reset. ", text_color);

	UpdateScoreText();

	return true;
}

void Game::SpawnObjects()
{
	player_ = std::make_unique<Player>(this);

	constexpr float scale = 2.0f;
	constexpr int distances[4] = { 400, 600, 800, 1000 };

	obstacles_.emplace_back(std::make_unique<Obstacle>(this, sprites_texture_.get(), static_cast<ObstacleType>(random_index_(mt_)), 1400, scale));

	for (std::size_t i = 1; i < 5; ++i)
	{
		obstacles_.emplace_back(std::make_unique<Obstacle>(this, sprites_texture_.get(), static_cast<ObstacleType>(random_index_(mt_)), obstacles_.at(i - 1)->bounding_box_.x + distances[random_index_(mt_)], scale));
	}

	bonus_items_.emplace_back(std::make_unique<BonusItem>(this, sprites_texture_.get(), BonusItemType::MONEY, 1400, scale * 2));

	for (std::size_t i = 1; i < 5; ++i)
	{
		bonus_items_.emplace_back(std::make_unique<BonusItem>(this, sprites_texture_.get(), BonusItemType::MONEY, bonus_items_.at(i - 1)->bounding_box_.x + distances[random_index_(mt_)], scale * 2));
	}
}

void Game::Run()
{
	if (!initialized_)
	{
		Finalize();
		return;
	}

	running_ = true;

	constexpr long double ms = 1.0 / 60.0;
	std::uint64_t last_time = SDL_GetPerformanceCounter();
	long double delta = 0.0;

	double timer = SDL_GetTicks();

	int frames = 0;
	int ticks = 0;

	while (running_)
	{
		const std::uint64_t now = SDL_GetPerformanceCounter();
		const long double elapsed = static_cast<long double>(now - last_time) / static_cast<long double>(SDL_GetPerformanceFrequency());

		last_time = now;
		delta += elapsed;

		HandleEvents();

		while (delta >= ms)
		{
			Tick();	
			delta -= ms;
			++ticks;
		}

		//printf("%Lf\n", delta / ms);
		Render();
		++frames;

		if (SDL_GetTicks() - timer > 1000)
		{
			timer += 1000;
			++score_;

			if (score_ % 50 == 0)
			{
				++scrolling_speed_;
			}

			//printf("Frames: %d, Ticks: %d\n", frames, ticks);
			frames = 0;
			ticks = 0;
		}
	}
}

void Game::HandleEvents()
{
	SDL_Event e;

	while (SDL_PollEvent(&e) != 0)
	{
		if (e.type == SDL_QUIT)
		{
			running_ = false;
		}

		if (game_over_ && e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_r)
		{
			Reset();
		}
		
		if (!game_over_)
		{
			player_->HandleEvent(&e);
		}
	}
}

void Game::Tick()
{
	if (game_over_)
	{
		return;
	}

	player_->Tick();

	UpdateScoreText();

	ground_scrolling_offset_ -= scrolling_speed_;

	if (ground_scrolling_offset_ < -constants::screen_width)
	{
		ground_scrolling_offset_ = 0;
	}

	for (const std::unique_ptr<Obstacle>& obstacle : obstacles_)
	{
		obstacle->Tick();
	}

	for (const std::unique_ptr<BonusItem>& bonus_item : bonus_items_)
	{
		bonus_item->Tick();
	}
}

void Game::Render()
{
	SDL_RenderSetViewport(renderer_, NULL);
	SDL_SetRenderDrawColor(renderer_, 0x00, 0x00, 0x00, 0xFF);

	SDL_RenderClear(renderer_);

	SDL_Rect background_clip;
	background_clip.x = 0;
	background_clip.y = 0;
	background_clip.w = constants::screen_width;
	background_clip.h = background_without_ground_h_;

	background_texture_->Render(renderer_, 0, 0, &background_clip);

	SDL_Rect ground_clip;
	ground_clip.x = 0;
	ground_clip.y = background_without_ground_h_;
	ground_clip.w = constants::screen_width;
	ground_clip.h = constants::screen_height - background_without_ground_h_;

	background_texture_->Render(renderer_, ground_scrolling_offset_, background_without_ground_h_, &ground_clip);
	background_texture_->Render(renderer_, ground_scrolling_offset_ + constants::screen_width, background_without_ground_h_, &ground_clip);

	player_->Render();
	score_info_->Render(renderer_, (constants::screen_width / 2) - score_info_->width_ / 2, 0);

	for (auto& obstacle : obstacles_)
	{
		obstacle->Render();
	}

	for (auto& bonus_item : bonus_items_)
	{
		bonus_item->Render();
	}

	if (game_over_)
	{
		game_over_info_->Render(renderer_, (constants::screen_width / 2) - game_over_info_->width_ / 2, constants::screen_height / 2);
	}

	SDL_RenderPresent(renderer_);
}

void Game::Stop()
{
	game_over_ = true;
}

void Game::Reset()
{
	game_over_ = false;
	score_ = 0;
	scrolling_speed_ = 10;
	obstacles_.clear();
	bonus_items_.clear();

	SpawnObjects();

	UpdateScoreText();
}

void Game::UpdateScoreText()
{
	SDL_Color text_color = { 0x00, 0x00, 0x00, 0xFF };
	const std::string score_text = "Score: " + std::to_string(score_);
	score_info_->LoadFromText(renderer_, font_, score_text.c_str(), text_color);
}