#include "Base_Exceptions.h"



OutOfVideoMemoryException::OutOfVideoMemoryException() : exception( "out of video memory" )
{};


ApplicationException::ApplicationException() : exception( pMessage )
{};

/*
ApplicationException::ApplicationException( const char* _pMessage ) : exception( pMessage )
{
	msgLength = sprintf_s( pMessage, APPLICATION_EXCEPTION_MESAGE_LENGTH, "%s\0", _pMessage );
};
*/

ApplicationException::ApplicationException( const char* _pMessage, ...)
{
	va_list valist;
	va_start( valist, _pMessage );
	msgLength = vsprintf_s( pMessage, APPLICATION_EXCEPTION_MESAGE_LENGTH, _pMessage, valist);
	va_end(valist);
};


ResourceException::ResourceException() : ApplicationException(), pResource(NULL), pId(NULL)
{};

ResourceException::ResourceException( const char* _pResource, const char* _pId ) : ApplicationException(), pResource(NULL), pId(NULL)
{
	msgLength = sprintf_s( pMessage, APPLICATION_EXCEPTION_MESAGE_LENGTH, "%s resource by id \'%s\'", pResource, pId );
};



ResourceNotFoundException::ResourceNotFoundException() : ResourceException()
{};

ResourceNotFoundException::ResourceNotFoundException( const char* _pResource, const char* _pId ) : ResourceException( _pResource, _pId )
{
	msgLength += sprintf_s( &(pMessage[ msgLength ]), APPLICATION_EXCEPTION_MESAGE_LENGTH - msgLength, " not found" );
};



ResourceDuplicateException::ResourceDuplicateException() : ResourceException()
{};

ResourceDuplicateException::ResourceDuplicateException( const char* _pResource, const char* _pId ) : ResourceException( _pResource, _pId )
{
	msgLength += sprintf_s( &(pMessage[ msgLength ]), APPLICATION_EXCEPTION_MESAGE_LENGTH - msgLength, " duplication" );
};



ResourceErrorException::ResourceErrorException() : ResourceException(), pErrorMsg(NULL)
{};

ResourceErrorException::ResourceErrorException( const char* _pResource, const char* _pId, const char* _pErrorMsg ) : ResourceException( _pResource, _pId ), pErrorMsg(_pErrorMsg)
{
	msgLength += sprintf_s( &(pMessage[ msgLength ]), APPLICATION_EXCEPTION_MESAGE_LENGTH - msgLength, " error: %s", pErrorMsg );
};






OnErrorException::OnErrorException() : ApplicationException(), pCaller(NULL), pFunction(NULL), returnError(0)
{};

OnErrorException::OnErrorException( const char* _pCaller, const char* _pFunction, UINT _returnError ) : ApplicationException(), pCaller(_pCaller), pFunction(_pFunction), returnError(_returnError)
{
	msgLength = sprintf_s( pMessage, APPLICATION_EXCEPTION_MESAGE_LENGTH, "%s called by %s returned error 0x%08X\0", pFunction, pCaller, returnError );
};


D3DX9Exception::D3DX9Exception( const char* _pCaller, const char* _pFunction, UINT _returnError ) : OnErrorException( _pCaller, _pFunction, _returnError )
{
	msgLength += sprintf_s( &(pMessage[ msgLength ]), APPLICATION_EXCEPTION_MESAGE_LENGTH - msgLength, " : %s - %s\0", DXGetErrorStringA(returnError), DXGetErrorDescriptionA(returnError) );
};


FMODException::FMODException( const char* _pCaller, const char* _pFunction, UINT _returnError ) : OnErrorException( _pCaller, _pFunction, _returnError )
{
	msgLength += sprintf_s( &(pMessage[ msgLength ]), APPLICATION_EXCEPTION_MESAGE_LENGTH - msgLength, " : %s\0", FMOD_ErrorString( (FMOD_RESULT) returnError ) );
};


SQLITE3Exception::SQLITE3Exception( const char* _pCaller, const char* _pFunction, sqlite3* _database ) : OnErrorException( _pCaller, _pFunction, sqlite3_extended_errcode( _database ) )
{
	msgLength += sprintf_s( &(pMessage[ msgLength ]), APPLICATION_EXCEPTION_MESAGE_LENGTH - msgLength, " : %s\0", sqlite3_errmsg( _database ) );
};
