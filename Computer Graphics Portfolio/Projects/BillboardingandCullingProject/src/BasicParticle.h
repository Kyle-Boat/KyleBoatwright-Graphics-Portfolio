#pragma once
#include "Particle.h"

class BasicParticle:public Particle
{
public:
    // Gets how much life the particle has left.  
    // When a particle's life is reduced to zero, it will be removed from the particle system.
    virtual float getLife() const ;
    void setLife(float life);
	float getXPos() const;
	float getYPos() const;
    int getType();
	void setType(int texType);
    void setPosition(float x, float y);
    // Updates the particle for a specified time increment (dt).
    virtual void update(float dt) ;

private:
    float life{ 0.0f };
    float xPos{ 1000.0f };
    float yPos{ 1000.0f };
	int texType{ 0};


};