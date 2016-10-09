
#define PI 3.14159265
#define DELTA 0.1
#define EARTH_CIRCUMFRENCE = 6371*2*PI

#define WINDOW_WIDTH 750
#define WINDOW_HEIGHT 480
#define PIXEL_TO_HEIGHT_RATIO 200
#define SQUARE_SIZE 4
#define PATH_SIZE 2

#include <cmath>
#include <unistd.h>
#include <stdio.h>

#include <iostream>
#include <string>
#include <vector>

#include <tuple>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

using std::cin;
using std::cout;
using std::endl;

class Arrow {
public:
  double pos_x;
  double pos_y;
  double vel_x;
  double vel_y;

  Arrow(double velocity, double angle, double mass, double area);
  void step(double delta);
  double shape_coefficient(double velocity);
  double airspeed();
  double total_vel();
  inline double angle() { return atan(vel_y / vel_x)/(2*PI) * 360; }
private:
  const double mass;
  const double area;
};

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

// kg/m³
double density_by_altitude(double altitude) {
  // magic
  return 1.225*exp(altitude*-1/8421);
}

// N
double air_resistance(double altitude, double velocity, double shape_coefficient, double area) {
  return -1*sgn(velocity)*0.5*density_by_altitude(altitude)*pow(velocity, 2)*shape_coefficient*area;
}

// m/s²
double force_to_acceleration(int force_total, int mass) {
  return force_total/mass;
}

// m/s²
double g(double altitude) {
  return -1*(altitude*3.03863758461374*pow(10, -6) + 9.80635968625765);
}

double speed_of_sound(double altitude) {
  if (altitude <= 11000) {
    return altitude*-0.004064375763562 + 340.547447427933;
  } else if (altitude <= 20000) {
    return 295.1;
  } else {
    return altitude*0.000657692307692 + 281.960256410256;
  }
}

Arrow::Arrow(double velocity, double angle, double mass, double area) : mass(mass), area(area) {
  pos_x = 0;
  pos_y = 0;

  vel_x = cos(angle * PI/180)*velocity;
  vel_y = sin(angle * PI/180)*velocity;
}

double Arrow::shape_coefficient(double velocity) {
  // http://www.lapua.com/upload/ballistics/qtu-lapua-edition-2010.pdf

  velocity /= speed_of_sound(pos_y);

  if (velocity <= 0.4) {
    return 0.23;
  } else if (velocity <= 0.8) {
    return velocity*-0.225 + 0.32;
  } else if (velocity <= 1.1) {
    return velocity*0.666666666666667 + -0.393333333333333;
  } else if (velocity <= 1.2) {
    return 0.34;
  } else if (velocity <= 3) {
    return velocity*-0.044444444444445 + 0.393333333333333;
  } else {
    return velocity*-0.01 + 0.29;
  }
}

double Arrow::airspeed() {
  return total_vel()/speed_of_sound(pos_y);
}

double Arrow::total_vel() {
  return sqrt(pow(vel_x, 2) + pow(vel_y, 2));
}

void Arrow::step(double delta) {
  double vel_total = sqrt(pow(vel_x, 2) + pow(vel_y, 2));
  double acc_x = force_to_acceleration(air_resistance(pos_y, vel_x, shape_coefficient(vel_total), area), mass);
  double acc_y = force_to_acceleration(air_resistance(pos_y, vel_y, shape_coefficient(vel_total), area) + mass*g(pos_y), mass);

  pos_x += vel_x*delta + 0.5*acc_x*pow(delta, 2);
  pos_y += vel_y*delta + 0.5*acc_y*pow(delta, 2);

  vel_x += acc_x*delta;
  vel_y += acc_y*delta;

}

int main (int argc, char **argv) {
  if (argc != 5) {
    cout << "Usage: ./main [angle in deg] [mass in kg] [area in m²] [initial velocity in m/s]" << endl;
    cout << "Example: ./main 50 14.8 0.008 2100" << endl;
    return 1;
  }

  double angle = std::stod(argv[1]);
  double mass = std::stod(argv[2]);
  double area = std::stod(argv[3]);
  double velocity = std::stod(argv[4]);

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

  bool quit = false;
  SDL_Event e;
  SDL_Texture* text_texture;

  Arrow arrow = Arrow(velocity, angle, mass, area);
  double t = 0.0;

  std::vector<std::tuple<int, int>> points;

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

    points.push_back(std::tuple<int, int>(x, y));

    SDL_Rect point;
    for (auto el : points) {
      point = {std::get<0>(el) - PATH_SIZE/2, std::get<1>(el) - PATH_SIZE/2, PATH_SIZE, PATH_SIZE};
      SDL_SetRenderDrawColor(ren, 0xFF, 0x00, 0x00, 0xAA);
      SDL_RenderFillRect(ren, &point);
    }

    SDL_Rect ammus = {x - SQUARE_SIZE/2, y - SQUARE_SIZE/2, SQUARE_SIZE, SQUARE_SIZE};
    SDL_SetRenderDrawColor(ren, 0xFF, 0x00, 0x00, 0xFF);
    SDL_RenderFillRect(ren, &ammus);

    int info_lines = 4;

    char text[info_lines][32];
    sprintf(text[0], "pos_x: %f km", arrow.pos_x/1000.0);
    sprintf(text[1], "pos_y: %f km", arrow.pos_y/1000.0);
    sprintf(text[2], "vel: %f mach", arrow.airspeed());
    sprintf(text[3], "ang: %f deg", arrow.angle());
    sprintf(text[4], "time: %f s", t);

    int current_h = 5;

    SDL_Surface* text_surface;

    for (int i = 0; i < info_lines; i++) {
      int w, h;
      TTF_SizeText(font, text[i], &w, &h);
      text_surface = TTF_RenderText_Solid(font, text[i], {0x00, 0x00, 0x00, 0xFF});
      text_texture = SDL_CreateTextureFromSurface(ren, text_surface);
      SDL_Rect text_rect = {5, current_h, w, h};
      current_h += h+2;
      SDL_RenderCopy(ren, text_texture, NULL, &text_rect);
    }

    SDL_RenderPresent(ren);

    if (arrow.pos_y >= 0) {
      arrow.step(DELTA);
      t += DELTA;
    }
  }

  SDL_DestroyTexture(text_texture);
  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  SDL_Quit();

  return 0;
}
