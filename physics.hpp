#pragma once
#include <Eigen/Dense>

// CONSTANTS
constexpr double EARTH_RADIUS = 6360000.0; 
constexpr double RAYLEIGH_SCALE_HEIGHT = 7994.0;

struct State {
    Eigen::Vector3d U;
};

//  rho(h) = rho_o*e^{-h/H}
double RayleighDensity(double height);

State Integrator(const State& s);

double ComputeOpticalDepth(const State& startState, const Eigen::Vector3d& rayDir, double maxDistance, int numSamples);
double RaySphereIntersection(const Eigen::Vector3d& origin, const Eigen::Vector3d& dir, double earthRadius, double atmosphereRadius);