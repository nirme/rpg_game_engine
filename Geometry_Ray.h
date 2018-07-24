#pragma once

#ifndef _GEOMETRY_RAY
#define _GEOMETRY_RAY

#include "utility.h"


class Ray
{
protected:

	D3DXVECTOR3 origin;
	D3DXVECTOR3 direction;
	D3DXVECTOR3 directionInverse;

public:

	Ray();
	Ray(D3DXVECTOR3* _origin, D3DXVECTOR3* _direction);
	Ray(D3DXMATRIX* _matrix);

	void updateOrigin(D3DXVECTOR3* _origin);
	void updateDirection(D3DXVECTOR3* _direction);
	void update(D3DXVECTOR3* _origin, D3DXVECTOR3* _direction);
	void update(D3DXMATRIX* _matrix);


	inline const D3DXVECTOR3* getOrigin()
	{
		return &origin;
	};

	inline const D3DXVECTOR3* getDirection()
	{
		return &direction;
	};

	inline const D3DXVECTOR3* getDirectionInverse()
	{
		return &directionInverse;
	};
};


#endif //_GEOMETRY_RAY
