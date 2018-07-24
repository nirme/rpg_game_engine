#pragma once

#ifndef _RESOURCE_GRAPHIC_SHADER_CONSTANT_DESCRIPTION
#define _RESOURCE_GRAPHIC_SHADER_CONSTANT_DESCRIPTION

#include "utility.h"

#include "Core_Defines.h"
#include "d3d9types.h"
#include "Base_Exceptions.h"




namespace Renderer
{
	//Forward declarations
	class Renderer;
	class ShaderConstantDescription;
	class DeviceStateManager;


	typedef	BYTE	SHADER_DESC_TYPE;

	#define	SHADER_DESC_TYPE_PIXEL		0
	#define	SHADER_DESC_TYPE_VERTEX		1
	#define	SHADER_DESC_TYPE_GEOMETRY	2


	#define		SHADER_CONST_TYPE_FLOAT		0x00000040
	#define		SHADER_CONST_OP_FLOAT		0x00
	#define		SHADER_CONST_TYPE_INT		0x00000080
	#define		SHADER_CONST_OP_INT			0x01
	#define		SHADER_CONST_TYPE_BOOL		0x00000100
	#define		SHADER_CONST_OP_BOOL		0x02


	typedef enum SHADER_CONST_TYPE
	{
		raw = 0x0000,

		float1 = 0x0041, //65
		float2 = 0x0042, //66
		float3 = 0x0043, //67
		float4 = 0x0044, //68
		float4x2 = 0x0048, //72
		float4x3 = 0x004C, //76
		float4x4 = 0x0050, //80

		int1 = 0x0081, //129
		int2 = 0x0082, //130
		int3 = 0x0083, //131
		int4 = 0x0084, //132
		int4x2 = 0x0088, //136
		int4x3 = 0x008C, //140
		int4x4 = 0x0090, //144

		bool1 = 0x0101, //257
		bool2 = 0x0102, //258
		bool3 = 0x0103, //259
		bool4 = 0x0104, //260
		bool4x2 = 0x0108, //264
		bool4x3 = 0x010C, //268
		bool4x4 = 0x0110, //272

		SHADER_CONST_ENC_SIZE = 0x80000000,

		FORCE_DWORD = 0xFFFFFFFF
	};
	////


	UINT getShaderConstLengthDword( SHADER_CONST_TYPE _type );
	UINT getShaderConstLengthVec4f( SHADER_CONST_TYPE _type );
	UINT getShaderConstOperationType( SHADER_CONST_TYPE _type );


	class ShaderConstantDescription
	{
		friend Renderer;
		friend DeviceStateManager;

	private:
		Renderer* pR_weakPtrRenderer;

		string str_name;
		UINT ui_refCount;

		BYTE bt_shaderType;
		SHADER_CONST_TYPE en_constType;
		BYTE bt_operationType;

		UINT ui_startRegister;
		UINT ui_lengthDword;
		UINT ui_lengthVec4f;

		DWORD dw_lastUpdateTimestamp;


		~ShaderConstantDescription()
		{};

	public:

		ShaderConstantDescription( Renderer* _pR_weakPtrRenderer, const string& _str_name, BYTE _bt_shaderType, SHADER_CONST_TYPE _en_constType, UINT _ui_startRegister )
		{};

		const string getName();
		const string* getNamePointer();

		void addRef();
		void release();

		inline DWORD getLastUpdateTimestamp()		{	return dw_lastUpdateTimestamp;	};
		inline void setLastUpdateTimestamp( DWORD _dw_lastUpdateTimestamp )		{	dw_lastUpdateTimestamp = _dw_lastUpdateTimestamp;	};
	};

}

#endif //_RESOURCE_GRAPHIC_SHADER_CONSTANT_DESCRIPTION
