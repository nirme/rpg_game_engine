#include "Resource_Graphic_Shader_Constant_Description.h"
#include "Graphic_Renderer.h"



namespace Renderer
{

	UINT getShaderConstLengthDword( SHADER_CONST_TYPE _type )
	{
		if ( _type & SHADER_CONST_ENC_SIZE )
			return (UINT)( _type & (~SHADER_CONST_ENC_SIZE) );

		if ( _type )
			return (UINT)( _type & 0x3F );

		return 0;
	};


	UINT getShaderConstLengthVec4f( SHADER_CONST_TYPE _type )
	{
		if ( _type & SHADER_CONST_ENC_SIZE )
			return ( (UINT)( _type & (~SHADER_CONST_ENC_SIZE) ) + 3 ) / 4;

		if ( _type )
			return ( (UINT)( _type & 0x3F ) + 3 ) / 4;

		return 0;
	};


	UINT getShaderConstOperationType( SHADER_CONST_TYPE _type )
	{
		if ( _type & SHADER_CONST_ENC_SIZE || _type & SHADER_CONST_TYPE_FLOAT )
			return SHADER_CONST_OP_FLOAT;

		if ( _type & SHADER_CONST_TYPE_INT )
			return SHADER_CONST_OP_INT;

		if ( _type & SHADER_CONST_TYPE_BOOL )
			return SHADER_CONST_OP_BOOL;

		return 0;
	};



	ShaderConstantDescription::ShaderConstantDescription( Renderer* _pR_weakPtrRenderer, const string& _str_name, BYTE _bt_shaderType, SHADER_CONST_TYPE _en_constType, UINT _ui_startRegister ) : pR_weakPtrRenderer( _pR_weakPtrRenderer ), str_name( _str_name ), us_refCount( 1 ), bt_shaderType( _bt_shaderType ), en_constType( _en_constType ), bt_operationType( getShaderConstOperationType( _en_constType ) ), ui_startRegister( _ui_startRegister ), ui_lengthDword( getShaderConstLengthDword( _en_constType ) ), ui_lengthVec4f( getShaderConstLengthVec4f( _en_constType ) ), dw_lastUpdateTimestamp( -1 )
	{};



	const string ShaderConstantDescription::getName()
	{
		return str_name;
	};


	const string* ShaderConstantDescription::getNamePointer()
	{
		return &str_name;
	};


	void ShaderConstantDescription::addRef()
	{
		++ui_refCount;
	};


	void ShaderConstantDescription::release()
	{
		if ( !( --ui_refCount ) )
		{
			pR_weakPtrRenderer->release( this );
			delete this;
		}
	};


	DWORD ShaderConstantDescription::getLastUpdateTimestamp;
	{
		return dw_lastUpdateTimestamp;
	};


	void ShaderConstantDescription::setLastUpdateTimestamp( DWORD _dw_lastUpdateTimestamp )
	{
		dw_lastUpdateTimestamp = _dw_lastUpdateTimestamp;
	};

}
