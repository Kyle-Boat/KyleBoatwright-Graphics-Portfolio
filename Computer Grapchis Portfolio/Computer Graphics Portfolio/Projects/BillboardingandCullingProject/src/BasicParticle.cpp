#include "BasicParticle.h"

#include <stdlib.h>

float BasicParticle::getLife() const
{
	return this->life;
}

void BasicParticle::setLife(float life)
{
	this->life = life;
}

void BasicParticle::setPosition(float x, float y)
{
	this->xPos = x;
	this->yPos = y;

}

float BasicParticle::getXPos() const
{
	return this->xPos;
}

float BasicParticle::getYPos() const
{
	return this->yPos;
}
int BasicParticle::getType() {
	return this->texType;
}
void BasicParticle::setType(int texType)
{
	this->texType = texType;
}

void BasicParticle::update(float dt)
{
	life -= (dt*.2); 
	float r = rand() % 1000 / 100.0f;
	float r2 = rand() % 1000 / 100.0f;
	xPos -= dt * .05 * r;
	yPos += dt * .2 * r2;
}
