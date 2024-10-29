#ifndef PHYSICS_H
#define PHYSICS_H

#include "main.h"

class Physics {
public:
    Physics(); 
    ~Physics(); 

    VOID HandleParticleCollision(PARTICLE* P, PARTICLE* particles, int32_t particle_count);
    VOID HandleGravity(C_PARTICLE* P);
    VOID HandleMovement(PARTICLE* P);
    VOID HandleCircularParticleCollision(C_PARTICLE* P, C_PARTICLE* particles, int32_t particle_count);
    VOID HandleNoGravityMovement(C_PARTICLE* P);
    VOID One_dimensional_collision(C_PARTICLE* P1, C_PARTICLE* P2);
    VOID Two_dimensional_collision(C_PARTICLE* P1, C_PARTICLE* P2);
};

#endif 
