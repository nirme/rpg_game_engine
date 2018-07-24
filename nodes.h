#pragma once

#ifndef _NODES
#define _NODES


#include "Base_Node.h"


bool sameSide(D3DXVECTOR3 p1, D3DXVECTOR3 p2, D3DXVECTOR3 a, D3DXVECTOR3 b);

bool pointInTriangle(D3DXVECTOR3 p, D3DXVECTOR3 a, D3DXVECTOR3 b, D3DXVECTOR3 c);


class ActorNode;
class GroundNode;

class GroundNode : public BaseSGNode
{
	friend class ActorNode;
protected:
	UINT groungId;
	GraphicObject* object;
	BoundingSphere* boundingVol;
	static int (*requestFunction)(void*);

public:
	GroundNode(BaseSGNode* root, UINT _groungId, D3DXVECTOR3 _position, D3DXVECTOR3 _rotation, D3DXVECTOR3 _scale);

	HRESULT Load();

	bool CheckGrounding(ActorNode* ac);
};


class ActorNode : public BaseSGNode
{
	friend class GroundNode;
	friend class ActorNode;
protected:
	UINT actorId;
	GraphicObject* object;
	//ID3DXAnimationController* pAnimController;

	BoundingCylinder* boundingVol;
	wchar_t* name;

	Channel* soundChannel;
	vector<SoundData*> sounds;

	UINT hp;
	UINT mp;

	DWORD currentTrack;
	DWORD animLoiter;
	DWORD animRun;
	bool iddle;

	float speedRun;
	float speedTurn;

	bool userControl;

	static int (*requestFunction)(void*);

public:
	ActorNode(BaseSGNode* root, UINT _actorId, D3DXVECTOR3 _position, D3DXVECTOR3 _rotation, D3DXVECTOR3 _scale);

	HRESULT Load();

	void Stand( bool bResetPosition = false);

	void Walk();

	void Turn();

	void Animate( float timeDelta );
};

#endif //_NODES