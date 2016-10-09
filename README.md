Calculates ans visualizes trajectories of high speed rounds. It considers
* air density as a function of altitude
* drag coefficient as a function of speed of air (which is a function of altitude)

It doesn't consider:
* curvature of the earth
* local weather conditions

## Dependencies
* libsdl
* libsdl_ttf

## Building and running
1. place any font as font.ttf in the same folder as main.cpp
2. change constants in the code (lines 1-10 and 174-176)
3. run `g++ main.cpp -lSDL2 -lSDL2_ttf`
4. ./main
