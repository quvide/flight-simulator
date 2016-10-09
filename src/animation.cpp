#define WINDOW_WIDTH 750
#define WINDOW_HEIGHT 480
#define PIXEL_TO_HEIGHT_RATIO 200
#define SQUARE_SIZE 4
#define PATH_SIZE 2

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <string>
#include <vector>
#include <tuple>
#include <stdio.h>
#include <iostream>

#include "arrow.h"
#include "helpers.h"

using std::cout;
using std::endl;

int renderloop(Arrow &arrow, double timestep) {
  // initializing sdl...
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    cout << SDL_GetError() << endl;
    return 1;
  }

  SDL_Window *win = SDL_CreateWindow("Flight Simulator - High Velocity Rounds", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);

  if (win == nullptr) {
    cout << SDL_GetError() << endl;
    SDL_Quit();
    return 1;
  }

  SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

  if (ren == nullptr) {
    SDL_DestroyWindow(win);
    cout << SDL_GetError() << endl;
    SDL_Quit();
    return 1;
  }

  TTF_Init();
  TTF_Font* font = TTF_OpenFont("font.ttf", 24);
  if (font == nullptr) {
    SDL_DestroyWindow(win);
    cout << SDL_GetError() << endl;
    SDL_Quit();
    return 1;
  }

  std::vector<std::tuple<int, int>> points;
  SDL_Event e;

  long long steps = 0;
  bool quit = false;


  // render loop
  while (!quit) {
    while (SDL_PollEvent(&e)) {
      if (e.type == SDL_QUIT) {
        quit = true;
      }
    }

    SDL_SetRenderDrawColor(ren, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(ren);

    for (int i = WINDOW_HEIGHT; i > 0; --i) {
      SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);
      SDL_SetRenderDrawColor(ren, 0x00, 0x00, 0xFF, 0xFF*density_by_altitude((WINDOW_HEIGHT-i)*PIXEL_TO_HEIGHT_RATIO)/density_by_altitude(0.0));
      SDL_RenderDrawLine(ren, 0, i, WINDOW_WIDTH, i);
    }

    int x = (int)arrow.pos_x/PIXEL_TO_HEIGHT_RATIO;
    int y = WINDOW_HEIGHT - (int)arrow.pos_y/PIXEL_TO_HEIGHT_RATIO;

    if (steps % (int)(1/timestep) == 0) {
      // save a little memory...
      points.push_back(std::tuple<int, int>(x, y));
    }

    SDL_Rect point;
    for (auto el : points) {
      point = {std::get<0>(el) - PATH_SIZE/2, std::get<1>(el) - PATH_SIZE/2, PATH_SIZE, PATH_SIZE};
      SDL_SetRenderDrawColor(ren, 0xFF, 0x00, 0x00, 0xAA);
      SDL_RenderFillRect(ren, &point);
    }

    SDL_Rect ammus = {x - SQUARE_SIZE/2, y - SQUARE_SIZE/2, SQUARE_SIZE, SQUARE_SIZE};
    SDL_SetRenderDrawColor(ren, 0xFF, 0x00, 0x00, 0xFF);
    SDL_RenderFillRect(ren, &ammus);

    int info_lines = 11;
    char text[info_lines][32];
    sprintf(text[0], "pos_x: %f km", arrow.pos_x/1000.0);
    sprintf(text[1], "pos_y: %f km", arrow.pos_y/1000.0);
    sprintf(text[2], "vel: %f mach", airspeed(total_vel(arrow.vel_x, arrow.vel_y), arrow.pos_y));
    sprintf(text[3], "ang: %f deg", angle(arrow.vel_x, arrow.vel_y));
    sprintf(text[4], "steps: %d", steps);
    sprintf(text[5], "time: %f", steps*timestep);
    sprintf(text[6], "initial values:");
    sprintf(text[7], "mass: %f kg", arrow.mass);
    sprintf(text[8], "area: %f m^2", arrow.area);
    sprintf(text[9], "angle: %f deg", arrow.initial_angle);
    sprintf(text[10], "velocity: %f m/s", arrow.initial_velocity);

    int current_h = 5;
    int current_w = 5;

    for (int i = 0; i < info_lines; i++) {
      int w, h;
      TTF_SizeText(font, text[i], &w, &h);
      SDL_Texture* text_texture;
      SDL_Surface* text_surface;
      text_surface = TTF_RenderText_Solid(font, text[i], {0x00, 0x00, 0x00, 0xFF});
      text_texture = SDL_CreateTextureFromSurface(ren, text_surface);
      SDL_FreeSurface(text_surface);
      SDL_Rect text_rect = {current_w, current_h, w, h};
      SDL_RenderCopy(ren, text_texture, NULL, &text_rect);
      SDL_DestroyTexture(text_texture);
      current_h += h+1;
      if (i == 5) {
        current_w = 400;
        current_h = 5;
      }
    }



    SDL_RenderPresent(ren);

    if (arrow.pos_y >= 0) {
      arrow.step(timestep);
      steps++;
    }
  }

  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return 0;
}
