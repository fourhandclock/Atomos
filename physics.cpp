#include "physics.hpp"
#include <cmath>

double RayleighDensity(double height) {
    if (height < 0.0) return 0.0;
    return std::exp(-height / RAYLEIGH_SCALE_HEIGHT);
}

State Integrator(const State& s) {
    State nextState;
    nextState.U = s.U;
    nextState.U = s.U;
    nextState.U = s.U;
    return nextState;
}

double ComputeOpticalDepth(const State& startState, const Eigen::Vector3d& rayDir, double maxDistance, int numSamples) {
    double stepSize = maxDistance / numSamples;
    double opticalDepth = 0.0;
    
    // Explicit indexing extraction
    double startX = startState.U(0);
    double startY = startState.U(1);
    double startZ = startState.U(2);
    
    // Trapezoidal Rule Integration
    for (int i = 0; i <= numSamples; ++i) {
        double t = i * stepSize;
        
        // Step along the ray
        double sampleX = startX + (rayDir(0) * t);
        double sampleY = startY + (rayDir(1) * t);
        double sampleZ = startZ + (rayDir(2) * t);
        
        // Calculate altitude
        double r = std::sqrt(sampleX * sampleX + sampleY * sampleY + sampleZ * sampleZ);
        double height = r - EARTH_RADIUS;
        
        double density = RayleighDensity(height);
        
        // Trapezoidal weights: 0.5 at boundaries, 1.0 everywhere else
        if (i == 0 || i == numSamples) {
            opticalDepth += density * 0.5;
        } else {
            opticalDepth += density;
        }
    }
    
    return opticalDepth * stepSize;
}

double RaySphereIntersection(const Eigen::Vector3d& origin, const Eigen::Vector3d& dir, double earthRadius, double atmosphereRadius) {
    // Quadratic equation coefficients: at^2 + bt + c = 0
    double a = dir.dot(dir); // Will be 1.0 if dir is normalized
    double b = 2.0 * origin.dot(dir);
    double c = origin.dot(origin) - (atmosphereRadius * atmosphereRadius);
    
    double discriminant = b * b - 4.0 * a * c;
    
    if (discriminant < 0.0) return -1.0; 
    
    // We are inside the atmosphere, so we want the positive forward root
    double t = (-b + std::sqrt(discriminant)) / (2.0 * a);
    return t;
}