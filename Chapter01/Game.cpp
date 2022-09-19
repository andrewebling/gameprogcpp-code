// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

#include "Game.h"

const int thickness = 15;
const float paddleH = 100.0f;

Game::Game()
:mWindow(nullptr)
,mRenderer(nullptr)
,mTicksCount(0)
,mIsRunning(true)
,mPaddleLeftDir(0)
,mPaddleRightDir(0)
{
	
}

bool Game::Initialize()
{
	// Initialize SDL
	int sdlResult = SDL_Init(SDL_INIT_VIDEO);
	if (sdlResult != 0)
	{
		SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
		return false;
	}
	
	// Create an SDL Window
	mWindow = SDL_CreateWindow(
		"Game Programming in C++ (Chapter 1)", // Window title
		100,	// Top left x-coordinate of window
		100,	// Top left y-coordinate of window
		1024,	// Width of window
		768,	// Height of window
		0		// Flags (0 for no flags set)
	);

	if (!mWindow)
	{
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return false;
	}
	
	//// Create SDL renderer
	mRenderer = SDL_CreateRenderer(
		mWindow, // Window to create renderer for
		-1,		 // Usually -1
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
	);

	if (!mRenderer)
	{
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return false;
	}
	//
	mPaddleLeftPos.x = 10.0f;
	mPaddleLeftPos.y = 768.0f/2.0f;
    
    mPaddleRightPos.x = 1024.0f - 10.0 - thickness;
    mPaddleRightPos.y = mPaddleLeftPos.y;
    
	mBallPos.x = 1024.0f/2.0f;
	mBallPos.y = 768.0f/2.0f;
	mBallVel.x = 200.0f;
	mBallVel.y = 235.0f;
	return true;
}

void Game::RunLoop()
{
	while (mIsRunning)
	{
		ProcessInput();
		UpdateGame();
		GenerateOutput();
	}
}

void Game::ProcessInput()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			// If we get an SDL_QUIT event, end loop
			case SDL_QUIT:
				mIsRunning = false;
				break;
		}
	}
	
	// Get state of keyboard
	const Uint8* state = SDL_GetKeyboardState(NULL);
	// If escape is pressed, also end loop
	if (state[SDL_SCANCODE_ESCAPE])
	{
		mIsRunning = false;
	}
	
	// Update paddle direction based on W/S keys
	mPaddleLeftDir = 0;
	if (state[SDL_SCANCODE_W])
	{
		mPaddleLeftDir -= 1;
	}
	if (state[SDL_SCANCODE_S])
	{
		mPaddleLeftDir += 1;
	}
    
    // Update paddle direction based on I/K keys
    mPaddleRightDir = 0;
    if (state[SDL_SCANCODE_I])
    {
        mPaddleRightDir -= 1;
    }
    if (state[SDL_SCANCODE_K])
    {
        mPaddleRightDir += 1;
    }
}

void Game::UpdateGame()
{
	// Wait until 16ms has elapsed since last frame
	while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
		;

	// Delta time is the difference in ticks from last frame
	// (converted to seconds)
	float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
	
	// Clamp maximum delta time value
	if (deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}

	// Update tick counts (for next frame)
	mTicksCount = SDL_GetTicks();
	
	// Update paddle position based on direction
	if (mPaddleLeftDir != 0)
	{
		mPaddleLeftPos.y += mPaddleLeftDir * 300.0f * deltaTime;
		// Make sure paddle doesn't move off screen!
		if (mPaddleLeftPos.y < (paddleH/2.0f + thickness))
		{
			mPaddleLeftPos.y = paddleH/2.0f + thickness;
		}
		else if (mPaddleLeftPos.y > (768.0f - paddleH/2.0f - thickness))
		{
			mPaddleLeftPos.y = 768.0f - paddleH/2.0f - thickness;
		}
	}
    
    if (mPaddleRightDir != 0)
    {
        mPaddleRightPos.y += mPaddleRightDir * 300.0f * deltaTime;
        if (mPaddleRightPos.y < (paddleH/2.0f + thickness))
        {
            mPaddleRightPos.y = paddleH/2.0f + thickness;
        }
        else if (mPaddleRightPos.y > (768.0f - paddleH/2.0f - thickness))
        {
            mPaddleRightPos.y = 768.0f - paddleH/2.0f - thickness;
        }
    }
	
	// Update ball position based on ball velocity
	mBallPos.x += mBallVel.x * deltaTime;
	mBallPos.y += mBallVel.y * deltaTime;
	
	// Bounce if needed
	// Did we intersect with the paddle?
	float diffL = mPaddleLeftPos.y - mBallPos.y;
    float diffR = mPaddleRightPos.y - mBallPos.y;
    
	// Take absolute value of difference
	diffL = (diffL > 0.0f) ? diffL : -diffL;
    diffR = (diffR > 0.0f) ? diffR : -diffR;
	if (
		// Our y-difference is small enough
		diffL <= paddleH / 2.0f &&
		// We are in the correct x-position
		mBallPos.x <= 25.0f && mBallPos.x >= 20.0f &&
		// The ball is moving to the left
		mBallVel.x < 0.0f)
	{
		mBallVel.x *= -1.0f;
	}
    else if (
             diffR <= paddleH / 2.0f &&
             mBallPos.x >= 1024.0f - 25.0f && mBallPos.x >= 1024.0f - 20.0f &&
             mBallVel.x > 0.0f)
    {
        mBallVel.x *= -1.0f;
    }
	// Did the ball go off the screen? (if so, end game)
	else if (mBallPos.x <= 0.0f || mBallPos.x >= 1024.0f)
	{
		mIsRunning = false;
	}
	
	// Did the ball collide with the top wall?
	if (mBallPos.y <= thickness && mBallVel.y < 0.0f)
	{
		mBallVel.y *= -1;
	}
	// Did the ball collide with the bottom wall?
	else if (mBallPos.y >= (768 - thickness) &&
		mBallVel.y > 0.0f)
	{
		mBallVel.y *= -1;
	}
}

void Game::GenerateOutput()
{
	// Set draw color to blue
	SDL_SetRenderDrawColor(
		mRenderer,
		0,		// R
		0,		// G 
		255,	// B
		255		// A
	);
	
	// Clear back buffer
	SDL_RenderClear(mRenderer);

	// Draw walls
	SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
	
	// Draw top wall
	SDL_Rect wall{
		0,			// Top left x
		0,			// Top left y
		1024,		// Width
		thickness	// Height
	};
	SDL_RenderFillRect(mRenderer, &wall);
	
	// Draw bottom wall
	wall.y = 768 - thickness;
	SDL_RenderFillRect(mRenderer, &wall);
	
	// Draw paddle
	SDL_Rect paddleL{
		static_cast<int>(mPaddleLeftPos.x),
		static_cast<int>(mPaddleLeftPos.y - paddleH/2),
		thickness,
		static_cast<int>(paddleH)
	};
	SDL_RenderFillRect(mRenderer, &paddleL);
    
    SDL_Rect paddleR{
        static_cast<int>(mPaddleRightPos.x),
        static_cast<int>(mPaddleRightPos.y - paddleH/2),
        thickness,
        static_cast<int>(paddleH)
    };
    SDL_RenderFillRect(mRenderer, &paddleR);
	
	// Draw ball
	SDL_Rect ball{	
		static_cast<int>(mBallPos.x - thickness/2),
		static_cast<int>(mBallPos.y - thickness/2),
		thickness,
		thickness
	};
	SDL_RenderFillRect(mRenderer, &ball);
	
	// Swap front buffer and back buffer
	SDL_RenderPresent(mRenderer);
}

void Game::Shutdown()
{
	SDL_DestroyRenderer(mRenderer);
	SDL_DestroyWindow(mWindow);
	SDL_Quit();
}
