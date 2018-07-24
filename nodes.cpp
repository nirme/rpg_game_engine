#include "nodes.h"

int (*GroundNode::requestFunction)(void*) = NULL;
int (*ActorNode::requestFunction)(void*) = NULL;




bool sameSide(D3DXVECTOR3 p1, D3DXVECTOR3 p2, D3DXVECTOR3 a, D3DXVECTOR3 b)
{
	D3DXVECTOR3 cp1, cp2, tmp1, tmp2;
	tmp1 = b-a;
	tmp2 = p1-a;
	D3DXVec3Cross(&cp1, &tmp1, &tmp2);
	tmp2 = p2-a;
	D3DXVec3Cross(&cp2, &tmp1, &tmp2);

	if (D3DXVec3Dot(&cp1, &cp2) >= (-1.0f * EPSILON))
		return true;
	return false;
};

bool pointInTriangle(D3DXVECTOR3 p, D3DXVECTOR3 a, D3DXVECTOR3 b, D3DXVECTOR3 c)
{
	if (sameSide(p,a, b,c) && sameSide(p,b, a,c) && sameSide(p,c, a,b))
		return true;
	return false;
};



GroundNode::GroundNode(BaseSGNode* root, UINT _groungId, D3DXVECTOR3 _position, D3DXVECTOR3 _rotation, D3DXVECTOR3 _scale)
{
	groungId = _groungId;
	pParentNode = root;
	pWorldPos = new WorldPosition(false, _position, _rotation, _scale);
	object = NULL;
	boundingVol = NULL;
};

HRESULT GroundNode::Load()
{
	(*requestFunction)((void*)this);
	return S_OK;
};

bool GroundNode::CheckGrounding(ActorNode* ac)
{
	if (boundingVol->RoughColision(pWorldPos->GetUpdatedWorldMatrix(), ac->boundingVol, ac->pWorldPos->GetUpdatedWorldMatrix()))
	{
		void* vertices = NULL;
		void* indices = NULL;
		object->getMeshPointer()->LockVertexBuffer(0, &vertices);
		object->getMeshPointer()->LockIndexBuffer(0, &indices);
		DWORD faces = object->getMeshPointer()->GetNumFaces();
		UINT vertSize = object->getMeshPointer()->GetNumBytesPerVertex();
		BYTE* wVer = (BYTE*) vertices;
		WORD* wInd = (WORD*) indices;
		D3DXVECTOR3 a, b, c, a0,b0,c0;
		D3DXMATRIX* m = pWorldPos->GetUpdatedWorldMatrix();
		ac->pWorldPos->GetUpdatedWorldMatrix();
		D3DXVECTOR3* ap = ac->pWorldPos->GetPositionVector();
		D3DXVECTOR3 acPos = D3DXVECTOR3(ap->x, 0.0f, ap->z);

		for (UINT i=0;i<faces;i+=3)
		{
			D3DXVec3TransformCoord(&a, ((D3DXVECTOR3*) wVer[(wInd[i]) * vertSize]), m);
			D3DXVec3TransformCoord(&b, ((D3DXVECTOR3*) wVer[(wInd[i+1]) * vertSize]), m);
			D3DXVec3TransformCoord(&c, ((D3DXVECTOR3*) wVer[(wInd[i+2]) * vertSize]), m);
/*
			if (fabs(a.x-acPos.x) <= EPSILON && fabs(a.z-acPos.z) <= EPSILON)
			{
				acPos.y = a.y;
				ac->pWorldPos->SetPosition(acPos);
				return true;
			}

			if (fabs(b.x-acPos.x) <= EPSILON && fabs(b.z-acPos.z) <= EPSILON)
			{
				acPos.y = b.y;
				ac->pWorldPos->SetPosition(acPos);
				return true;
			}

			if (fabs(c.x-acPos.x) <= EPSILON && fabs(c.z-acPos.z) <= EPSILON)
			{
				acPos.y = c.y;
				ac->pWorldPos->SetPosition(acPos);
				return true;
			}
*/
			a0 = D3DXVECTOR3(a.x, 0.0f, a.z);
			b0 = D3DXVECTOR3(b.x, 0.0f, b.z);
			c0 = D3DXVECTOR3(c.x, 0.0f, c.z);

			if (pointInTriangle(acPos, a0, b0, c0))
			{
				D3DXPLANE plane;
				D3DXPlaneFromPoints(&plane, &a,&b,&c);
				acPos.y = (plane.a * acPos.x + plane.c * acPos.z + plane.d) / (-1 * plane.b);
				ac->pWorldPos->SetPosition(acPos);

				object->getMeshPointer()->UnlockVertexBuffer();
				object->getMeshPointer()->UnlockIndexBuffer();

				return true;
			}
		}

		object->getMeshPointer()->UnlockVertexBuffer();
		object->getMeshPointer()->UnlockIndexBuffer();
	}
	return false;
};


ActorNode::ActorNode(BaseSGNode* root, UINT _actorId, D3DXVECTOR3 _position, D3DXVECTOR3 _rotation, D3DXVECTOR3 _scale)
{
	actorId = _actorId;
	pParentNode = root;
	pWorldPos = new WorldPosition(false, _position, _rotation, _scale);
	object = NULL;
	boundingVol = NULL;
	name = NULL;
	soundChannel = NULL;
	hp = 0;
	mp = 0;

	speedRun;
	speedTurn;

	userControl;

};

HRESULT ActorNode::Load()
{
	(*requestFunction)((void*)this);
	return S_OK;
};

void ActorNode::Stand( bool bResetPosition)
{
	DWORD newTrack = ( currentTrack == 0 ? 1 : 0 );
	LPD3DXANIMATIONCONTROLLER pAC = object->GetAnimationInfo()->pAnimController;
	LPD3DXANIMATIONSET pAS;
	pAC->GetAnimationSet( animLoiter, &pAS );

	pAC->SetTrackAnimationSet( newTrack, pAS );
	pAS->release();

	pAC->UnkeyAllTrackEvents( currentTrack );
	pAC->UnkeyAllTrackEvents( newTrack );

	float curTime = Timer::getGlobalTime();

	pAC->KeyTrackEnable( currentTrack, FALSE, curTime + 0.125f );
	pAC->KeyTrackSpeed( currentTrack, 0.0f, curTime, 0.125f, D3DXTRANSITION_LINEAR );
	pAC->KeyTrackWeight( currentTrack, 0.0f, curTime, 0.125f, D3DXTRANSITION_LINEAR );
	pAC->SetTrackEnable( newTrack, TRUE );
	pAC->KeyTrackSpeed( newTrack, 1.0f, curTime, 0.125f, D3DXTRANSITION_LINEAR );
	pAC->KeyTrackWeight( newTrack, 1.0f, curTime, 0.125f, D3DXTRANSITION_LINEAR );


	if( bResetPosition )
		pAC->SetTrackPosition( newTrack, 0.0 );

	currentTrack = newTrack;
};

void ActorNode::Walk()
{
	DWORD newTrack = ( currentTrack == 0 ? 1 : 0 );
	LPD3DXANIMATIONCONTROLLER pAC = object->GetAnimationInfo()->pAnimController;
	LPD3DXANIMATIONSET pAS;
	pAC->GetAnimationSet( animRun, &pAS );

	pAC->SetTrackAnimationSet( newTrack, pAS );
	pAS->release();

	pAC->UnkeyAllTrackEvents( currentTrack );
	pAC->UnkeyAllTrackEvents( newTrack );

	float curTime = Timer::getGlobalTime();

	pAC->KeyTrackEnable( currentTrack, FALSE, curTime + 0.25f );
	pAC->KeyTrackSpeed( currentTrack, 0.0f, curTime, 0.25f, D3DXTRANSITION_LINEAR );
	pAC->KeyTrackWeight( currentTrack, 0.0f, curTime, 0.25f, D3DXTRANSITION_LINEAR );
	pAC->SetTrackEnable( newTrack, TRUE );
	pAC->KeyTrackSpeed( newTrack, 1.0f, curTime, 0.25f, D3DXTRANSITION_LINEAR );
	pAC->KeyTrackWeight( newTrack, 1.0f, curTime, 0.25f, D3DXTRANSITION_LINEAR );

	currentTrack = newTrack;
};

void ActorNode::Turn()
{
	D3DXVECTOR3* v = pWorldPos->GetRotationVector();
	v->y +=	Input::GetMouseX();
	pWorldPos->SetRotation((*v));
};

void ActorNode::Animate( float timeDelta )
{
	if (!object->isLoaded())
		return;

	bool canMove = true;
	if( userControl && Input::GetKeyState(DIK_UP) )
	{
		D3DXVECTOR3* fac = pWorldPos->GetFacing();
		D3DXVECTOR3* pos = pWorldPos->GetPositionVector();

		D3DXVECTOR3 vMovePos = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
		D3DXMATRIX mm1;
		D3DXVec3TransformCoord(&vMovePos, &vMovePos, D3DXMatrixRotationY( &mm1, fac->y ) );

		D3DXTRACK_DESC td;
		object->GetAnimationInfo()->pAnimController->GetTrackDesc(currentTrack, &td );

		D3DXVec3Scale( &vMovePos, &vMovePos, float( speedRun * td.Speed * timeDelta ) );
		D3DXVec3Add( &vMovePos, &vMovePos, pos );

		UINT m = pRootNode->GetChildCount();
		BaseSGNode* test = NULL;
		for (UINT i=0;i<m;++i)
		{
			test = pRootNode->GetChild(i);
			if (typeid(*test) == typeid(ActorNode))
			{
				ActorNode* an = dynamic_cast<ActorNode*>(test);
				D3DXMATRIX* mm = an->pWorldPos->GetUpdatedWorldMatrix();
				D3DXVECTOR3 m2 = D3DXVECTOR3(mm->_41, mm->_42, mm->_43);
				if (boundingVol->Colision(&vMovePos, an->boundingVol, &m2))
				{
					canMove = false;
					break;
				}
			}
		}
		if (canMove)
		{
			pWorldPos->SetPosition(vMovePos);

			test = NULL;
			for (UINT i=0;i<m;++i)
			{
				test = pRootNode->GetChild(i);
				if (typeid(*test) == typeid(GroundNode))
				{
					GroundNode* an = dynamic_cast<GroundNode*>(test);
					if (an->CheckGrounding(this))
						break;
				}
			}
		}

		if( iddle && canMove )
		{
			Walk();
			iddle = false;
		}
		if( !iddle && !canMove )
		{
			Stand( true );
			iddle = true;
		}

		Turn();
	}

	if (iddle)
	{
		D3DXTRACK_DESC td;
		object->GetAnimationInfo()->pAnimController->GetTrackDesc(currentTrack, &td);
		LPD3DXANIMATIONSET pASTrack;
		object->GetAnimationInfo()->pAnimController->GetTrackAnimationSet( currentTrack, &pASTrack );

		if( td.Position > pASTrack->GetPeriod() - 0.125f )
			Stand(true);
	}

	pWorldPos->GetUpdatedWorldMatrix();
};