#pragma once

#ifndef _BASE_EXCEPTIONS
#define _BASE_EXCEPTIONS


#include "utility.h"


#define		APPLICATION_EXCEPTION_MESAGE_LENGTH		1024



class OutOfVideoMemoryException : public exception
{
public:
	OutOfVideoMemoryException();
};



class ApplicationException : public exception
{
protected:
	char pMessage[ APPLICATION_EXCEPTION_MESAGE_LENGTH ];
	int msgLength;

	ApplicationException();

public:
//	ApplicationException( const char* _pMessage );
	ApplicationException( const char* _pMessage, ...);

	const char* getMsg()	{	return pMessage;	};
	int getMsgLen()			{	return msgLength;	};

};



class ResourceException : public ApplicationException
{
protected:
	const char* pResource;
	const char* pId;

	ResourceException();

public:
	ResourceException( const char* _pResource, const char* _pId );
};



class ResourceNotFoundException : public ResourceException
{
protected:
	ResourceNotFoundException();

public:
	ResourceNotFoundException( const char* _pResource, const char* _pId );
};



class ResourceDuplicateException : public ResourceException
{
protected:
	ResourceDuplicateException();

public:
	ResourceDuplicateException( const char* _pResource, const char* _pId );
};



class ResourceErrorException : public ResourceException
{
protected:
	const char* pErrorMsg;

	ResourceErrorException();

public:
	ResourceErrorException( const char* _pResource, const char* _pId, const char* _pErrorMsg );
};




class OnErrorException : public ApplicationException
{
protected:
	const char* pCaller;
	const char* pFunction;
	UINT returnError;

	OnErrorException();

public:
	OnErrorException( const char* _pCaller, const char* _pFunction, UINT _returnError );

	inline const char* getFunctionName() const	{	return pFunction;	};
	inline const char* getCallerName() const		{	return pCaller;		};
	inline UINT getErrorValue() const			{	return returnError;	};
};



class D3DX9Exception : public OnErrorException
{
public:
	D3DX9Exception( const char* _pCaller, const char* _pFunction, UINT _returnError );
};



class FMODException : public OnErrorException
{
public:
	FMODException( const char* _pCaller, const char* _pFunction, UINT _returnError );
};



class SQLITE3Exception : public OnErrorException
{
public:
	SQLITE3Exception( const char* _pCaller, const char* _pFunction, sqlite3* _database );
};



#endif //_BASIC_EXCEPTIONS
