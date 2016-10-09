#include "arrow.h"
#include "helpers.h"
#include <cmath>

Arrow::Arrow(double velocity, double angle, double mass, double area) : mass(mass), area(area), initial_velocity(velocity), initial_angle(angle) {
  pos_x = 0;
  pos_y = 0;

  vel_x = cos(angle * PI/180)*velocity;
  vel_y = sin(angle * PI/180)*velocity;
}

void Arrow::step(double delta) {
  double vel_total = sqrt(pow(vel_x, 2) + pow(vel_y, 2));
  double acc_x = force_to_acceleration(air_resistance(pos_y, vel_x, shape_coefficient(vel_total, pos_y), area), mass);
  double acc_y = force_to_acceleration(air_resistance(pos_y, vel_y, shape_coefficient(vel_total, pos_y), area) + mass*g(pos_y), mass);

  pos_x += vel_x*delta + 0.5*acc_x*pow(delta, 2);
  pos_y += vel_y*delta + 0.5*acc_y*pow(delta, 2);

  vel_x += acc_x*delta;
  vel_y += acc_y*delta;
}
