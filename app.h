#pragma once

#ifndef _ACTUAL_APP
#define _ACTUAL_APP



#include "utility.h"

#include "Core_Defines.h"
#include "Core_Variable_Library.h"
#include "Core_Logger.h"

#include "Core_Application.h"

#include "Core_Graphic.h"

//#include "Core_Timer.h"
#include "Core_Timer_v2.h"

#include "Core_World_Position_v3.h"
#include "Core_Camera.h"

#include "Core_Graphic_Resource.h"
#include "Core_Graphic_Object.h"
#include "Core_Light.h"

#include "Particle_System_v4.h"
#include "Particle_Processor_Particle_Fall.h"
#include "Particle_Processor_Particle_Gun.h"
#include "Particle_Processor_Eyes.h"


#include "Base_Node.h"
#include "Geo_Node.h"
#include "Camera_Node.h"
#include "Light_Node.h"

#include "Graphic_Node.h"


#include "Core_SoundSystem.h"


//#include "Loader_v2.h"
#include "LOADFUNCTION.h"

#include "Core_Input.h"


#include "MusicMixer.h"




class ActualApp : public Application
{
protected:

	Camera* camera;
	CameraNode* cameraNode;

	GraphicObject* graphicObject;
	GraphicNode* graphicNode;
	Texture* tex;
	Texture* front;
	Texture* life;
	Texture* mana;
	ID3DXFont* font;
	ID3DXFont* wfont;

	RECT ma, li;

	Sound* sound;
	Sound* voices[7];
	Channel* channel;
	Channel* channelp;

	D3DXVECTOR3 forward, right, top;


	WorldPosition particleSystemPosition;
	ParticleSystem_v4 particle;


public:
	ActualApp() : Application()
	{
		camera = NULL;
		cameraNode = NULL;
		graphicObject = NULL;
		graphicNode = NULL;
		tex = NULL;
		font = NULL;
		wfont = NULL;
	};

	int initialize()
	{
		int r = VariableLibrary::initialize("vars.lbr");
		Logger::initialize();
		Input::initialize(gethInst(), gethWnd());
		int i = Graphic::initialize(gethWnd());

		Graphic::setBackgroundColor(0xFF000000);

		SoundSystem::setResampleMethod(FMOD_DSP_RESAMPLER_LINEAR);
		SoundSystem::setSampleRate(48000);
		SoundSystem::setSoundFormat(FMOD_SOUND_FORMAT_PCM16);
		SoundSystem::setDefaultRolloff(FMOD_3D_INVERSEROLLOFF);
		SoundSystem::setRolloffScale(0.2f);
		SoundSystem::setDistanceFactor(200.00f);
		SoundSystem::setMinimumAudibleDistance(1.0f);
		SoundSystem::setMaximumAudibleDistance(6000.0f);
		SoundSystem::setHRTFLowpass(true);
		SoundSystem::setHRTFFrequency(16000.0f);
		SoundSystem::setHRTFMaxAngle(200.0f);

		i = SoundSystem::initialize();

		if (i)
		{
			showError(true, L"SoundSystem::initialize error: %d\n", i);
			return 1;
		}
		sound = Sound::getResource(0);
		voices[0] = Sound::getResource(7);
		voices[1] = Sound::getResource(8);
		voices[2] = Sound::getResource(9);
		voices[3] = Sound::getResource(10);
		voices[4] = Sound::getResource(11);
		voices[5] = Sound::getResource(12);
		voices[6] = Sound::getResource(13);

		i = SoundSystem::get2DChannel(&channel);
		channel->setVolume(0.5f);

		if (i)
		{
			showError(true, L"SoundSystem::get2DChannel error: %d\n", i);
			return 1;
		}


		Timer::initialize();

		WorldPosition* cwp = new WorldPosition;
		camera = new Camera(cwp, D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXToRadian(90), 5.0f/4.0f, 1.0f, 10000.0f);


		cameraNode = new CameraNode("camera", camera);
		cameraNode->getWorldPosition()->setPosition(D3DXVECTOR3(0.0f, 0.0f, -20.0f));

		graphicObject = new GraphicObject();
		Mesh* mesh = Mesh::getResource(0);
		tex = Texture::getResource(0);
		Effect* eff = Effect::getResource(0);

		front = Texture::getResource(1);
		life = Texture::getResource(2);
		mana = Texture::getResource(3);

		Material* m = new Material;
		m->setEffect(eff);
		m->setTexture("g_txScene", tex);
		bool w = m->isLoaded();
		vector<Material*> v_pMaterial;
		v_pMaterial.push_back(m);
		int dwRenderPass = -1;
		RenderChunk* rc = new RenderChunk(graphicObject, 0, 0);
		vector<RenderChunk*> renderChunks;
		renderChunks.push_back(rc);
		graphicObject->setName("tiny");
		graphicObject->setResources(mesh, &v_pMaterial, dwRenderPass, &renderChunks);
		graphicNode = new GraphicNode("tiny", graphicObject);
		graphicNode->getWorldPosition()->setPosition(D3DXVECTOR3(0.0f, 0.0f, 40.0f));
		graphicNode->getWorldPosition()->setScale(D3DXVECTOR3(0.12f, 0.12f, 0.12f));
//		graphicNode->getWorldPosition()->setRotationX(-1.5707f);

		cameraNode->updateNode();




		particleSystemPosition.setPosition(D3DXVECTOR3(0.0f,0.0f,0.0f));
		particleSystemPosition.setRotation(D3DXVECTOR3(0.0f,0.0f,0.0f));
//		particleSystemPosition.setScale(D3DXVECTOR3(1.0f,1.0f,1.0f));


		ParticleProcessor_ParticleFall* particleProcessor = new ParticleProcessor_ParticleFall;
		particleProcessor->setBaseRotationSpeed(D3DXVECTOR3(0.2f,0.6f,0.6f));
		particleProcessor->setBaseRotationSpeedDeviation(D3DXVECTOR3(0.1f,0.2f,0.3f));
		particleProcessor->setBaseSize(2.45f);
		particleProcessor->setBaseSizeDeviation(0.04f);
		particleProcessor->setBaseVelocity(D3DXVECTOR3(0.0f,-4.0f,0.0f));
		particleProcessor->setBaseVelocityDeviation(D3DXVECTOR3(1.0f,2.0f,1.0f));
		particleProcessor->setCylinderHeight(100.0f);
		particleProcessor->setCylinderRadius(100.0f);
		particleProcessor->setParticleStartPoint(D3DXVECTOR3(0.0f,0.0f,0.0f));
		particleProcessor->setStartCircleRadius(90.0f);
		particleProcessor->setParticlesEmitedPerSecond(50.0f);

		particle.initialize(Graphic::getDevice(), &particleSystemPosition, particleProcessor, Effect::getResource(10), Texture::getResource(10), 4, 1, 10000, PARTICLE_SORTING_PARTIAL, BILLBOARDING_NONE, 0, true);


/*
ParticleProcessor_ParticleGun* pgFountain = new ParticleProcessor_ParticleGun;

pgFountain->setStartPoint(&D3DXVECTOR3(0.0f, 0.0f, 0.0f));
pgFountain->setStartPointDeviation(&D3DXVECTOR3(4.0f, 0.0f, 4.0f));
pgFountain->setDirection(0.0f, 0.0f);
//pgFountain->setDirectionFromNormal(D3DXVECTOR3* _direction);
pgFountain->setEjection(D3DXToRadian(00.00f), D3DXToRadian(1.0f));
pgFountain->setVelocity(20.0f, 3.0f);
pgFountain->setLifetime(8.0f, 1.0f);
pgFountain->setSize(1.0f, 0.2f);
pgFountain->setSizeChange(0.01f, 0.005f);
pgFountain->setColor(&D3DXCOLOR(0.500f, 0.500f, 0.500f, 1.00f), &D3DXCOLOR(0.500f, 0.500f, 0.500f, 0.00f));
pgFountain->setBoundingSphereRadius(200.0f);
pgFountain->setParticlesPerSecond(200.0f);
pgFountain->setMediumResistance(0.00001f);
pgFountain->setGravity(&particleSystemPosition, 10.0f);


particle.initialize(Graphic::getDevice(), &particleSystemPosition, pgFountain, Effect::getResource(10), Texture::getResource(12), 1, 1, 20000, PARTICLE_SORTING_PARTIAL, BILLBOARDING_VIEWPLANE, 0, true);
*/



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


		D3DXVec3TransformNormal(&right, &D3DXVECTOR3(1.0f, 0.0f, 0.0f), cameraNode->getWorldPosition()->getWorldPosition());
		D3DXVec3TransformNormal(&top, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), cameraNode->getWorldPosition()->getWorldPosition());
		D3DXVec3TransformNormal(&forward, &D3DXVECTOR3(0.0f, 0.0f, 1.0f), cameraNode->getWorldPosition()->getWorldPosition());


		SoundSystem::set3DAttributes(	reinterpret_cast <FMOD_VECTOR*> (cameraNode->getWorldPosition()->getWorldPositionVector()), 
										NULL, 
										reinterpret_cast <FMOD_VECTOR*> (&forward), 
										reinterpret_cast <FMOD_VECTOR*> (&top));

		i = SoundSystem::get3DChannel(SOUNDSYSTEM_MASTER_CHANNELGROUP_ID, reinterpret_cast <FMOD_VECTOR*> (graphicNode->getWorldPosition()->getWorldPositionVector()), NULL, &channelp);



		li.top = ma.top = 0.0f;
		li.left = ma.left = 0.0f;
		li.bottom = ma.bottom = 24.0f;
		li.right = ma.right = 160.0f;

		Graphic::setupRenderStates();
		//Graphic::setClearBeforeRendering(false);
		//Graphic::showCursor(true);


		D3DXCreateFontA(Graphic::getDevice(), 14, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Lucida Console", &font);
		D3DXCreateFontW(Graphic::getDevice(), 14, 0, FW_NORMAL, 1, false, DEFAULT_CHARSET, OUT_TT_ONLY_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Lucida Console", &wfont);

		return 0;
	};

	int shutdown()
	{
		delete cameraNode;
		delete graphicNode;

		particle.shutdown();

		Texture::releaseAll();

		Timer::shutdown();

		Channel::releaseAll();
		Sound::releaseAll();
		SoundSystem::shutdown();

		Graphic::shutdown();
		Logger::shutdown();
		VariableLibrary::shutdown();

		return 0;
	};

	int frame()
	{
		Timer::update();
		Input::update();

		cameraNode->updateNode();


		if (Input::getKeyState(DIK_ESCAPE))
				return 10000;

		if (Input::getKeyClicked(DIK_E))
			Graphic::showCursor(!Graphic::isCursorVisible());

		DIMOUSESTATE st = Input::getMouseState();
		WorldPosition* p = cameraNode->getWorldPosition();
		float rotX = p->getRotationX() + (float(st.lY)/250.f);
		if (rotX > 1.57079f)
			rotX = 1.57079f;
		if (rotX < -1.57079f)
			rotX = -1.57079f;
		p->setRotationX(rotX);
		p->setRotationY(p->getRotationY() + (float(st.lX)/250.f));

		if (Input::getKeyState(DIK_NUMPAD7))
			p->setRotationZ(p->getRotationZ() + (0.8f * Timer::getTimeDelta()));
		if (Input::getKeyState(DIK_NUMPAD9))
			p->setRotationZ(p->getRotationZ() - (0.8f * Timer::getTimeDelta()));

		if (Input::getKeyClicked(DIK_NUMPAD8))
			p->setRotationZ(0.0f);
		if (Input::getKeyClicked(DIK_NUMPAD6))
			p->setRotationZ(4.712388f);
		if (Input::getKeyClicked(DIK_NUMPAD2))
			p->setRotationZ(3.141592f);
		if (Input::getKeyClicked(DIK_NUMPAD4))
			p->setRotationZ(1.570796f);

		D3DXVec3TransformNormal(&right, &D3DXVECTOR3(1.0f, 0.0f, 0.0f), p->getWorldPosition());
		D3DXVec3TransformNormal(&top, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), p->getWorldPosition());
		D3DXVec3TransformNormal(&forward, &D3DXVECTOR3(0.0f, 0.0f, 1.0f), p->getWorldPosition());

		int f=0, r=0, t=0;
		if (Input::getKeyState(DIK_W))
			f += 1;
		if (Input::getKeyState(DIK_S))
			f -= 1;
		if (Input::getKeyState(DIK_D))
			r += 1;
		if (Input::getKeyState(DIK_A))
			r -= 1;
		if (Input::getKeyState(DIK_SPACE))
			t += 1;
		if (Input::getKeyState(DIK_LCONTROL))
			t -= 1;

		D3DXVECTOR3 out = forward * float(f) + right * float(r) + top * float(t);

		out *= 44.0f * Timer::getTimeDelta();

		p->setPosition(*(p->getPosition()) + out);



		if (Input::getKeyClicked(DIK_P))
		{
			bool paused = false;
			if (!channel->getPaused(&paused))
				channel->setPaused(!paused);
		}
		else
		{
			bool paused = false;
			channel->getPaused(&paused);
			if (Input::getKeyClicked(DIK_PRIOR) && !Input::getKeyClicked(DIK_NEXT))
			{
				channel->stop();
				UINT id = sound->getId();
				if (id)	id --;
				id = id % 7;
				sound->release();
				sound = Sound::getResource(id);
				channel->playSound(sound);
			}
			if (!Input::getKeyClicked(DIK_PRIOR) && Input::getKeyClicked(DIK_NEXT))
			{
				channel->stop();
				UINT id = sound->getId() + 1;
				id = id % 7;
				sound->release();
				sound = Sound::getResource(id);
				channel->playSound(sound);
			}
/*
			bool isPlaying = false;
			if (channel->isPlaying(&isPlaying))
			{
				if (!isPlaying)
				{
					channel->playSound(sound, paused);
				}
			}
*/
		}



		if (Input::getKeyClicked(DIK_1))
			channelp->playSound(voices[0]);
		if (Input::getKeyClicked(DIK_2))
			channelp->playSound(voices[1]);
		if (Input::getKeyClicked(DIK_3))
			channelp->playSound(voices[2]);
		if (Input::getKeyClicked(DIK_4))
			channelp->playSound(voices[3]);
		if (Input::getKeyClicked(DIK_5))
			channelp->playSound(voices[4]);
		if (Input::getKeyClicked(DIK_6))
			channelp->playSound(voices[5]);
		if (Input::getKeyClicked(DIK_7))
			channelp->playSound(voices[6]);



		if (Input::getKeyState(DIK_RSHIFT))
		{
			camera->setProjectionMatrixFOVHorizontalLH(camera->getFOVHorizontal() + (0.2f * Timer::getTimeDelta()), 5.0f/4.0f);
		}
		if (Input::getKeyState(DIK_RCONTROL))
		{
			camera->setProjectionMatrixFOVHorizontalLH(camera->getFOVHorizontal() - (0.2f * Timer::getTimeDelta()), 5.0f/4.0f);
		}


		if (Graphic::beginBasicRendering())
		{
			showError(true, L"Graphic::beginBasicRendering failed!");
			return 3;
		}

		//graphicNode->getWorldPosition()->setRotationY(graphicNode->getWorldPosition()->getRotationY() + 0.01f);

		static bool q;
		static float i = 0.0f;
		if (q)
			i += 0.01f;
		else
			i -= 0.01f;
		if (i >= 1.0f)
			q = false;
		else if (i < 0.0f)
		{
			q = true;
			i = 0.0f;
		}
		ma.right = 160.0f * i;
		li.right = 160.0f - ma.right;
//		Graphic::drawSprite(life->getResourcePointer(), &(D3DXVECTOR3(88.0f, 950.0f, 0.0f)), &li);
//		Graphic::drawSprite(mana->getResourcePointer(), &(D3DXVECTOR3(88.0f, 982.0f, 0.0f)), &ma);
//		Graphic::drawSprite(front->getResourcePointer());

		//if (cameraNode->nodeInsideFrustum(graphicNode) > 0)
			graphicNode->renderNode(cameraNode->getCamera());

		float x = p->getRotationY();
		D3DXVECTOR3 cameraNormal = D3DXVECTOR3(sin(x), 0.0f, cos(x));


		char str[1024];
		sprintf(str, "FPS: %2.2f\n\ncamera normal:\n%+.4f,\t%+.4f,\t%+.4f\n\n\0", Graphic::getFPS(), cameraNormal.x, cameraNormal.y, cameraNormal.z);

		RECT rect;

		SetRect(&rect, 10, 10, 200, 100);
		font->DrawTextA(Graphic::getSprite(), str, -1, &rect, DT_NOCLIP | DT_LEFT, 0xFFFFFFFF);

		SetRect(&rect, 10, 100, 200, 200);
		wfont->DrawTextW(Graphic::getSprite(), sound->getTextPointer(), -1, &rect, DT_NOCLIP | DT_LEFT, 0xFFFFFFFF);




//////////////////////////////////////////////////////////////////////
/*
		ParticleProcessor_ParticleGun* pgFountain = reinterpret_cast <ParticleProcessor_ParticleGun*> (particle.getParticleProcessor());


if (Input::getKeyState(DIK_UP))
	pgFountain->setDirection(pgFountain->getDirectionRotationX() + 4.0f * Timer::getTimeDelta(), pgFountain->getDirectionRotationY());
if (Input::getKeyState(DIK_DOWN))
	pgFountain->setDirection(pgFountain->getDirectionRotationX() - 4.0f * Timer::getTimeDelta(), pgFountain->getDirectionRotationY());
if (Input::getKeyState(DIK_RIGHT))
	pgFountain->setDirection(pgFountain->getDirectionRotationX(), pgFountain->getDirectionRotationY() + 4.0f * Timer::getTimeDelta());
if (Input::getKeyState(DIK_LEFT))
	pgFountain->setDirection(pgFountain->getDirectionRotationX(), pgFountain->getDirectionRotationY() - 4.0f * Timer::getTimeDelta());


if (Input::getKeyState(DIK_H))
	pgFountain->setEjection(pgFountain->getEjectionInnerAngle() - Timer::getTimeDelta(), pgFountain->getEjectionOuterAngle());
if (Input::getKeyState(DIK_N))
	pgFountain->setEjection(pgFountain->getEjectionInnerAngle() + Timer::getTimeDelta(), pgFountain->getEjectionOuterAngle());
if (Input::getKeyState(DIK_J))
	pgFountain->setEjection(pgFountain->getEjectionInnerAngle(), pgFountain->getEjectionOuterAngle() - Timer::getTimeDelta());
if (Input::getKeyState(DIK_M))
	pgFountain->setEjection(pgFountain->getEjectionInnerAngle(), pgFountain->getEjectionOuterAngle() + Timer::getTimeDelta());

if (Input::getKeyState(DIK_ADD))
	pgFountain->setParticlesPerSecond(pgFountain->getParticlesPerSecond() + 100.0f * Timer::getTimeDelta());
if (Input::getKeyState(DIK_SUBTRACT))
	pgFountain->setParticlesPerSecond(pgFountain->getParticlesPerSecond() - 100.0f * Timer::getTimeDelta());

if (Input::getKeyState(DIK_HOME))
	pgFountain->setVelocity(pgFountain->getVelocity() + 10.0f * Timer::getTimeDelta(), 3.0f);
if (Input::getKeyState(DIK_END))
	pgFountain->setVelocity(pgFountain->getVelocity() - 10.0f * Timer::getTimeDelta(), 3.0f);
*/


		int tt = 0;
		tt = particle.update(Timer::getTimeDelta(), camera);
		if (tt)
		{
			showError(false, L"particle.update error: %d\n", tt);
			//return 1;
		}

		tt = particle.draw(Graphic::getDevice(), camera);
		if (tt)
		{
			showError(false, L"particle.draw error: %d\n", tt);
//			return 1;
		}


/*
{//particles info
	char pstr[1024];

	sprintf(pstr, "particles emited per second: %f\nbase velocity: %f\n\0", pgFountain->getParticlesPerSecond(), pgFountain->getVelocity());

	RECT prect;

	SetRect(&prect, 10, 320, 200, 320);
	font->DrawTextA(Graphic::getSprite(), pstr, -1, &prect, DT_NOCLIP | DT_LEFT, 0xFFFFFFFF);
	
}
*/


///////////////////////////////////////////////////////////////////////////

		{//particles info
			char pstr[1024];

			sprintf(pstr, "particles: %d\nparticles rendered: %d\ntime: %f\n\0", particle.getAliveParticlesNumber(), particle.getParticlesBeforeCameraNumber(), particle.getTotalTime());

			RECT prect;

			SetRect(&prect, 10, 220, 200, 320);
			font->DrawTextA(Graphic::getSprite(), pstr, -1, &prect, DT_NOCLIP | DT_LEFT, 0xFFFFFFFF);
			
		}


		if (Graphic::endBasicRendering())
		{
			showError(true, L"Graphic::endBasicRendering failed!");
			return 6;
		}


		if (Graphic::presentBasicSceen() < 0)
		{
			showError(true, L"Graphic::presentBasicSceen failed!");
			return 5;
		}



		if (SoundSystem::update())
		{
			showError(true, L"Updating Sound failed!");
			return 1;
		}


		Graphic::updateTime(Timer::getTimeDelta());

		//Graphic
/**/
		return 0;
	};

};



#endif //_ACTUAL_APP