#include "Effect_Include_Menager.h"




int EffectIncludeMenager::IncludeFile::setup(const char* _pFileName, void* _fileInMem, UINT _filesize)
{
	refCount = 0;
	filename = _fileInMem;
	file.resize(_filesize);
	memcpy(&(file[0]), _fileInMemory, _filesize);
	pFile = &(file[0]);

	return 0;
};


int EffectIncludeMenager::loadFile(const char* _pFileName, IncludeFile** _includedFile)
{
	string fileName = _pFileName;

	list<IncludeFile>::iterator it_end = includeFiles.end();
	for (list<IncludeFile>::iterator it = includeFiles.begin(); it != it_end; it++)
	{
		if (!(*it).filename.compare(fileName))
		{
			*_includedFile = &(*it);
			return 0;
		}
	}

	// file isn't there so we need to load it from database
	sqlite3_stmt* stmt = NULL;
	ostringstream sql;
	sql << "SELECT `filedata` FROM `effect_include` WHERE `name` LIKE '" << _pFileName << "'";

	int result = 0;

	if ((result = sqlite3_prepare_v2(_db, sql.str().c_str(), -1, &stmt, NULL)) != SQLITE_OK)
	{
		sqlite3_finalize(stmt);
		showError(false, "sqlite3_prepare_v2 failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
		return -1;
	}

	result = sqlite3_step(stmt);

	if (result != SQLITE_ROW)
	{
		sqlite3_finalize(stmt);

		if (result == SQLITE_DONE)
		{
			showError(false, "File '%s' not found", _pFileName);
			return -2;
		}

		showError(false, "sqlite3_step failed with error code %d:\n\"%s\"", result, sqlite3_errmsg(_db));
		return -3;
	}


	includeFiles.push_back(IncludeFile());
	IncludeFile* incfile = &(*includeFiles.rbegin());

	incfile->setup(_pFileName, sqlite3_column_blob(stmt, 0), sqlite3_column_bytes(stmt, 0));

	*_includedFile = incfile;
	return 0;
};


int EffectIncludeMenager::releaseFile(void* _dataPointer)
{
	list<IncludeFile>::iterator it_end = includeFiles.end();
	for (list<IncludeFile>::iterator it = includeFiles.begin(); it != it_end; it++)
	{
		if (_dataPointer == (*it).pFile)
		{
			(*it).refCount --;
			if (!(*it).refCount)
				includeFiles.erase(it);
			return 0;
		}
	}

	showError(false, "File not found");
	return -1;
};



int EffectIncludeMenager::initialize(sqlite3* _pSqlDatabase)
{
	if (!_pSqlDatabase)
		return 1;

	pSqlDatabase = _pSqlDatabase;
	includeFiles.clear();

	return 0;
};


HRESULT EffectIncludeMenager::Open(D3DXINCLUDE_TYPE _includeType, const char* _pFileName, void* _pParentData, void** _ppData, UINT* _pBytes)
{
	IncludeFile* file;

	if (loadFile(_pFileName, &file))
		return D3DXERR_INVALIDDATA;

	file->refCount++;

	*_ppData = file->pFile;
	*_pBytes = file->file.size();

	return S_OK;
};


HRESULT EffectIncludeMenager::Close(void* _pData)
{
	if (findFile(_pData))
		return D3DXERR_INVALIDDATA;

	return S_OK;
};
