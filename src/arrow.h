#include <cmath>

class Arrow {
public:
  double pos_x;
  double pos_y;
  double vel_x;
  double vel_y;

  const double initial_velocity;
  const double initial_angle;
  const double mass;
  const double area;

  Arrow(double velocity, double angle, double mass, double area);
  void step(double delta);
};
