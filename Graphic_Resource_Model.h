#pragma once

#ifndef _CORE_GRAPHIC_MODEL
#define _CORE_GRAPHIC_MODEL

#include "utility.h"
#include "Skin_Info.h"
#include "HierarchicalAnimationController.h"





class Model : public Resource <ID3DXMesh*>
{
private:
	~Model();

protected:
	DWORD subsetsNumber;
	SkinInfo* pSkinInfo;


public:
	#ifndef RESOURCE_MAP_WITH_STRINGS
		Model ( UINT _id ) : subsets( 0 ), pSkinInfo( NULL ), Resource( _id, NULL ) {};
	#else //RESOURCE_MAP_WITH_STRINGS
		Model ( string _id ) : subsets( 0 ), pSkinInfo( NULL ), Resource( _id, NULL ) {};
	#endif //RESOURCE_MAP_WITH_STRINGS



	void setResource( ID3DXMesh* _mesh, SkinInfo* _pSkinInfo )
	{
		#ifdef( MULTI_THREADED_LOADING )
			boost::mutex::scoped_lock lock( mutex );
		#endif //MULTI_THREADED_LOADING

		if ( loaded )
			return;

		if ( _mesh )
		{
			resource = _mesh;
			resource->GetAttributeTable( NULL, &subsetsNumber );
			pSkinInfo = pSkinInfo;

			loaded = true;
		}
		else
		{
			resource = NULL;
			pSkinInfo = NULL;
			loaded = false;
		}

		#ifdef( MULTI_THREADED_LOADING )
			conditionVar.notify_all();
		#endif //MULTI_THREADED_LOADING
	};


	#ifndef RESOURCE_MAP_WITH_STRINGS
		static Model* getResource( UINT _id )
	#else //RESOURCE_MAP_WITH_STRINGS
		static Model* getResource( string _id )
	#endif //RESOURCE_MAP_WITH_STRINGS
	{
		return reinterpret_cast <Model*> ( getBaseResource( _id ) );
	};


	#ifdef(MULTI_THREADED_LOADING)
		#ifndef RESOURCE_MAP_WITH_STRINGS
			static Model* getResourceMultiThreaded( UINT _id )
		#else //RESOURCE_MAP_WITH_STRINGS
			static Model* getResourceMultiThreaded( string _id )
		#endif //RESOURCE_MAP_WITH_STRINGS
		{
			return reinterpret_cast <Model*> ( getBaseResourceMultiThread( _id ) );
		};
	#endif //MULTI_THREADED_LOADING


	int onLostDevice(){ return 0; };
	int onResetDevice(){ return 0; };


	inline DWORD getSubsetsNumber()				{	return subsetsNumber;	};

	inline bool isSkinned()						{	return pSkinInfo ? true : false;	}:
	inline const SkinInfo* getSkinInfo()		{	return pSkinInfo;	}:
	inline const ID3DXMesh* getSkinnedMesh()	{	return pSkinInfo ? pSkinInfo->getBlendedMesh() : NULL;	};
	inline DWORD getSkinnedMeshSubsetsNumber()	{	return pSkinInfo ? pSkinInfo->getSubsetsNumber() : 0;	};



	HierarchicalAnimationController* createAnimationController( BYTE _maxTracksNumber, BYTE _maxEventsNumber )
	{
		ControllableSkinInfo
		ControllableSkinInfo* csi = NULL;

		if ( !( csi = dynamic_cast<ControllableSkinInfo*> (pSkinInfo) ) )
		{
		}

		HierarchicalAnimationController* hAC = new HierarchicalAnimationController(

HierarchicalAnimationController( Model* _pModel, HAnimationSet* _pResourceAnimationSet, ID3DXSkinInfo* _pSkinInfo, BYTE _maxTracksNumber, BYTE _maxEventsNumber );


	};


};




#endif //_CORE_GRAPHIC_MODEL
