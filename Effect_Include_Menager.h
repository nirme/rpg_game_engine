#pragma once

#ifndef _EFFECT_INCLUDE_MENAGER
#define _EFFECT_INCLUDE_MENAGER

#include "utility.h"
#include "thread_safe_queue.h"

#include "Core_Logger.h"
#include "Core_Variable_Library.h"
#include "Core_Defines.h"


/*
	sqlite3_stmt* stmt = NULL;
	ostringstream sql;
	sql << "SELECT `filedata` FROM `texture` WHERE `id` = " << pTexture->getId();

	int result = 0;

	if ((result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL)) != SQLITE_OK ||
		((result = sqlite3_step(stmt)) != SQLITE_ROW && result != SQLITE_DONE))
	{
		sqlite3_finalize(stmt);
		showError(true, "sqlite3_prepare_v2/sqlite3_step failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
		return 1;
	}

	//set texture
	IDirect3DTexture9* texture = NULL;
	HRESULT hr = D3DXCreateTextureFromFileInMemory(	Graphic::getDevice(),
													sqlite3_column_blob(stmt,0),
													sqlite3_column_bytes(stmt,0),
													(IDirect3DTexture9**) &texture);
	if (hr != S_OK)
	{
		sqlite3_finalize(stmt);
		showError(true, "Error: %s error description: %s\n", DXGetErrorStringA(hr), DXGetErrorDescriptionA(hr));
		return 3;
	}
	sqlite3_finalize(stmt);

	pTexture->setResource(texture);
	if (pTexture->getRefCount() == 0)
		delete pTexture;

	return 0;
*/



class EffectIncludeMenager : public ID3DXInclude
{
private:

	struct IncludeFile
	{
		UINT refCount;
		string filename;
		vector<BYTE> file;
		void* pFile;

		IncludeFile() : refCount(0), pFile(NULL) {};

		int setup(const char* _pFileName, void* _fileInMem, UINT _filesize);
	};


	sqlite3* pSqlDatabase;

	list<IncludeFile> includeFiles;


	int loadFile(const char* _pFileName, IncludeFile** _includedFile);
	int releaseFile(void* _dataPointer);


public:

	EffectIncludeMenager() : pSqlDatabase(NULL) {};

	int initialize(sqlite3* _pSqlDatabase);

	HRESULT Open(D3DXINCLUDE_TYPE _includeType, const char* _pFileName, void* _pParentData, void** _ppData, UINT* _pBytes);
	HRESULT Close(void* _pData);

};


#endif //_EFFECT_INCLUDE_MENAGER
