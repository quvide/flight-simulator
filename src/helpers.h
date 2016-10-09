#include <cmath>
#define PI 3.1415926535897932384626433832795

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

inline double density_by_altitude(double altitude) {
  return 1.225*exp(altitude*-1/8421);
}

inline double air_resistance(double altitude, double velocity, double shape_coefficient, double area) {
  return -1*sgn(velocity)*0.5*density_by_altitude(altitude)*pow(velocity, 2)*shape_coefficient*area;
}

inline double force_to_acceleration(int force_total, int mass) {
  return force_total/mass;
}

inline double g(double altitude) {
  return -1*(altitude*3.03863758461374*pow(10, -6) + 9.80635968625765);
}

inline double speed_of_sound(double altitude) {
  if (altitude <= 11000) {
    return altitude*-0.004064375763562 + 340.547447427933;
  } else if (altitude <= 20000) {
    return 295.1;
  } else {
    return altitude*0.000657692307692 + 281.960256410256;
  }
}

inline double total_vel(double vel_x, double vel_y) {
  return sqrt(pow(vel_x, 2) + pow(vel_y, 2));
}

inline double shape_coefficient(double velocity, double altitude) {
  // source: http://www.lapua.com/upload/ballistics/qtu-lapua-edition-2010.pdf

  velocity /= speed_of_sound(altitude);

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

inline double angle(double vel_x, double vel_y) {
  return atan(vel_y / vel_x)/(2*PI) * 360;
}

inline double airspeed(double vel_total, double altitude) {
  return vel_total/speed_of_sound(altitude);
}
