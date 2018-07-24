#pragma once

#ifndef _PARTICLE_PROCESSORS_SET
#define _PARTICLE_PROCESSORS_SET



#include "Particle_Processor_Particle_Fall.h"
#include "Particle_Processor_Particle_Gun.h"


#define		PP_TYPE_PARTICLE_FALL		1
#define		PP_TYPE_PARTICLE_GUN		2



int loadParticleProcessor(DWORD _type, const void* _data, UserDefinedParticleProcessor** _ppOut)
{
	UserDefinedParticleProcessor* ppOut = NULL;

	switch (_type)
	{
		case PP_TYPE_PARTICLE_FALL:
		{
			ParticleProcessor_ParticleFall* pp = new ParticleProcessor_ParticleFall();
			ppOut = pp;
			break;
		}

		case PP_TYPE_PARTICLE_GUN:
		{
			ParticleProcessor_ParticleGun* pp = new ParticleProcessor_ParticleGun();
			ppOut = pp;
			break;
		}

		default:
			return 100;
	}


	if (int res = ppOut->unserialize(cost_cast <void*> (_data)))
	{
		delete ppOut;
		return res;
	}

	*_ppOut = ppOut;
	return 0;
};



#endif //_PARTICLE_PROCESSORS_SET