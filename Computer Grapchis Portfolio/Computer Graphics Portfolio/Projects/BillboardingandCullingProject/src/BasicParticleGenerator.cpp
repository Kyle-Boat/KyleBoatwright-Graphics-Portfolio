#include "BasicParticleGenerator.h"
#include <stdlib.h>

void BasicParticleGenerator::respawn(BasicParticle& particle) const
{
	particle.setLife(1.0f);

	particle.setType((rand()%2));

	//set postion,velocity, color, ec ( make setters in basic partocle)

	particle.setPosition((rand() % 1000 / 1000.0f),0);
}
