#include <iostream>

#include "arrow.h"
#include "animation.h"

using std::cin;
using std::cout;
using std::endl;

int main (int argc, char **argv) {
  if (argc != 7) {
    cout << "Usage: ./main [angle in deg] [mass in kg] [area in m²] [initial velocity in m/s] [timestep in seconds] [graphics yes/no]" << endl;
    cout << "Examples:" << endl;
    cout << "  - ./main 50 14.8 0.008 2100 0.1 yes" << endl;
    cout << "  - ./main 30 21 0.009 1900 0.0001 no" << endl;
    cout << "Notes:" << endl;
    cout << "  - You can use a much lower timestep without graphics, because the animation is capped at 60 steps per second" << endl;
    return 1;
  }

  double angle = std::stod(argv[1]);
  double mass = std::stod(argv[2]);
  double area = std::stod(argv[3]);
  double velocity = std::stod(argv[4]);
  double delta = std::stod(argv[5]);
  bool graphics = std::string(argv[6]) == "yes";

  Arrow arrow = Arrow(velocity, angle, mass, area);

  if (graphics) {
    renderloop(arrow, delta);
  } else {
    while (arrow.pos_y >= 0) {
      arrow.step(delta);
    }
  }

  cout << "distance flown: " << arrow.pos_x/1000.0 << " km" << endl;
  return 0;
}
