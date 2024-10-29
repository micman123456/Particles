#include "physics.h"

Physics::Physics() {
    // Constructor implementation
}

Physics::~Physics() {
    // Destructor implementation
}

VOID Physics::HandleParticleCollision(PARTICLE* P, PARTICLE* particles, int32_t particle_count) 
{
    int32_t P_y = P->y + (P->y_direction * P->y_velocity);
    int32_t P_x = P->x + (P->x_direction * P->x_velocity);
    
    if (P->y_velocity == 0 && P->x_velocity == 0) {
        return;
    }

    for (int i = 0; i < particle_count; i++) {
        // Skip if checking against itself
        if (&particles[i] == P) {
            continue;
        }

       // Particle collision //

        bool x_overlap = P_x < particles[i].x + particles[i].h && P_x + P->w > particles[i].x;
        bool y_overlap = P_y < particles[i].y + particles[i].h && P_y + P->h > particles[i].y;

        if (x_overlap && y_overlap) {
            P->x_velocity = 0;
            P->y_velocity = 0;
            PIXEL p = {0};
            p.alpha = 255;
            p.red = 255;

            P->p = p;
            particles[i].p = p;
            return; 
        }
    }


    // Boundary collision //
    if (P->y > GAME_HEIGHT - P->h) {
        //P->y_direction = -1;
        P->y_velocity = 0;
        P->y = GAME_HEIGHT - P->h;
    }
    if (P->y < 0) {
        P->y = GAME_HEIGHT - P->h;
        //P->y_direction = 1;
    }
    if (P->x > GAME_WIDTH - P->w) {
        //P->x_direction = -1;
        P->x_velocity = 0;
    }
    if (P->x < 0) {
        //P->x_direction = 1;
        P->x_velocity = 0;

    }
}

VOID Physics::HandleGravity(C_PARTICLE* P) 
{
    
    P->y_acceleration -= 9.18;
    
    if (P->x_velocity > 0){
        P->x_velocity -= 0.1;
    }
    else {
        P->x_velocity = 0;
    }




    
}

VOID Physics::HandleMovement(PARTICLE* P)
{
    P->y += P->y_direction * P->y_velocity;
    P->x += P->x_direction * P->x_velocity;
}



VOID Physics::HandleNoGravityMovement(C_PARTICLE* P)
{
    // Update velocity based on acceleration
    P->x_velocity += P->x_acceleration;
    P->y_velocity += P->y_acceleration;

    // Clamp velocity to maximum velocity
    if (P->x_velocity > MAX_VELOCITY) {
        P->x_velocity = MAX_VELOCITY;
    } else if (P->x_velocity < -MAX_VELOCITY) {
        P->x_velocity = -MAX_VELOCITY;
    }

    if (P->y_velocity > MAX_VELOCITY) {
        P->y_velocity = MAX_VELOCITY;
    } else if (P->y_velocity < -MAX_VELOCITY) {
        P->y_velocity = -MAX_VELOCITY;
    }

    // Apply gradual deceleration to acceleration
    if (P->x_acceleration > 0) {
        P->x_acceleration--;
    } else if (P->x_acceleration < 0) {
        P->x_acceleration++;
    }

    if (P->y_acceleration > 0) {
        P->y_acceleration--;
    } else if (P->y_acceleration < 0) {
        P->y_acceleration++;
    }

    // Update position based on current velocity
    P->x += P->x_velocity;
    P->y += P->y_velocity;
}






VOID Physics::HandleCircularParticleCollision(C_PARTICLE* P, C_PARTICLE* particles, int32_t particle_count) 
{
    int32_t P_y = P->y + P->y_velocity;
    int32_t P_x = P->x + P->x_velocity;
    
    if (P->y_velocity == 0 && P->x_velocity == 0) {
        P->y_velocity++;
        P->x_velocity++;
        return;
    }

    for (int i = 0; i < particle_count; i++) 
    {
        // Skip if checking against itself
        
        if (&particles[i] == P) 
        {
            continue;
        }

        // Check if Dis(C1,C2) < sum(C1 rad + C2 rad) //
        
        int32_t dx = P_x - particles[i].x;
        int32_t dy = P_y - particles[i].y;

        
        int32_t distanceSquared = dx * dx + dy * dy;
        int32_t radiiSum = P->r + particles[i].r;
        int32_t radiiSquared = radiiSum * radiiSum;

    
        bool overlap = distanceSquared < radiiSquared;

        // Particle collision occurs //

        if (overlap) {

            //CalculateVelocityOnImpact(P,&particles[i]);
            Two_dimensional_collision(P,&particles[i]);
        
        }
    }


    // Boundary collision //

    if (P->y > GAME_HEIGHT - P->r) 
    {
        P->y_velocity = -P->y_velocity;
        P->y_acceleration-=1;
        P->y = GAME_HEIGHT - P->r;
    }
    
    if (P->y < 0 + P->r )  
    {
        P->y_velocity = -P->y_velocity;
        P->y_acceleration+=1;
        P->y = P->r;
    }
    if (P->x > GAME_WIDTH - P->r) 
    {
        P->x_velocity = -P->x_velocity;
        P->x_acceleration-=1;
        P->x = GAME_WIDTH - P->r;
    }
    if (P->x < 0 + P->r) 
    {
        P->x_velocity = -P->x_velocity;
        P->x_acceleration+=1;
        P->x = 0 + P->r;
    }
}




VOID Physics::One_dimensional_collision(C_PARTICLE* P1, C_PARTICLE* P2)
{
    int32_t MA_plus_MB = P1->r + P2->r;
    int32_t MA_min_MB = P1->r - P2->r;

    // Calculate new velocities after collision using momentum conservation
    int32_t P1_XV = (MA_min_MB / MA_plus_MB) * P1->x_velocity + (2 * P2->r / MA_plus_MB) * P2->x_velocity;
    int32_t P1_YV = (MA_min_MB / MA_plus_MB) * P1->y_velocity + (2 * P2->r / MA_plus_MB) * P2->y_velocity;

    int32_t P2_XV = (2 * P1->r / MA_plus_MB) * P1->x_velocity + (-MA_min_MB / MA_plus_MB) * P2->x_velocity;
    int32_t P2_YV = (2 * P1->r / MA_plus_MB) * P1->y_velocity + (-MA_min_MB / MA_plus_MB) * P2->y_velocity;

    // Update particle velocities
    P1->x_velocity = P1_XV;
    P1->y_velocity = P1_YV;

    P2->x_velocity = P2_XV;
    P2->y_velocity = P2_YV;
}


void Physics::Two_dimensional_collision(C_PARTICLE* P1, C_PARTICLE* P2)
{

   

    
    double y_diff = P1->y - P2->y;
    double x_diff = P1->x - P2->x;

    double contactAngle = atan2(y_diff, x_diff);

    
    double v1 = sqrt((P1->x_velocity * P1->x_velocity) + (P1->y_velocity * P1->y_velocity));
    double v2 = sqrt((P2->x_velocity * P2->x_velocity) + (P2->y_velocity * P2->y_velocity));

    
    double angleOne = atan2(P1->y_velocity, P1->x_velocity);
    double angleTwo = atan2(P2->y_velocity, P2->x_velocity);

    
    double m1 = P1->mass;
    double m2 = P2->mass;

    
    double P1_new_x_velocity = ((v1 * cos(angleOne - contactAngle) * (m1 - m2) + 
                                (2 * m2 * v2 * cos(angleTwo - contactAngle))) / (m1 + m2)) * cos(contactAngle) +
                                v1 * sin(angleOne - contactAngle) * cos(contactAngle + PI / 2);

    double P1_new_y_velocity = ((v1 * cos(angleOne - contactAngle) * (m1 - m2) + 
                                (2 * m2 * v2 * cos(angleTwo - contactAngle))) / (m1 + m2)) * sin(contactAngle) +
                                v1 * sin(angleOne - contactAngle) * sin(contactAngle + PI / 2);

    
    double P2_new_x_velocity = ((v2 * cos(angleTwo - contactAngle) * (m2 - m1) + 
                                (2 * m1 * v1 * cos(angleOne - contactAngle))) / (m1 + m2)) * cos(contactAngle) +
                                v2 * sin(angleTwo - contactAngle) * cos(contactAngle + PI / 2);

    double P2_new_y_velocity = ((v2 * cos(angleTwo - contactAngle) * (m2 - m1) + 
                                (2 * m1 * v1 * cos(angleOne - contactAngle))) / (m1 + m2)) * sin(contactAngle) +
                                v2 * sin(angleTwo - contactAngle) * sin(contactAngle + PI / 2);

    P1->x_velocity = P1_new_x_velocity;
    P1->y_velocity = P1_new_y_velocity;

    P2->x_velocity = P2_new_x_velocity;
    P2->y_velocity = P2_new_y_velocity;
}
