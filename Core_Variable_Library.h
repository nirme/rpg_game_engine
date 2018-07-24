#pragma once

#ifndef _VARIABLE_LIBRARY_V3
#define _VARIABLE_LIBRARY_V3

#include "utility.h"
#include "Core_Defines.h"
#include "Base_Exceptions.h"




#ifdef VL_STRICT_CHECK
	const char* const error_vl_wrong_type = "VariableLibrary error - wrong value type selected";
#endif


const char* const error_vl_file_open = "VariableLibrary error - can't open file: '%s'";
const char* const error_vl_file_read = "VariableLibrary error - reading file error";
const char* const error_vl_file_write = "VariableLibrary error - writing to file error";
const char* const error_vl_no_dict = "VariableLibrary error - selected dictionary doesn't exist";
const char* const error_vl_no_entry = "VariableLibrary error - selected entry doesn't exist";



class WordValue;
class WordValueBool;
class WordValueInteger;
class WordValueFloat;
class WordValueString;
class VariableLibrary;


typedef map<string, WordValue*, scompare>		Dictionary;
typedef map<string, Dictionary, scompare>		Library;



class WordValue
{
public:
	typedef enum ValueType
	{
		VT_BOOL,
		VT_INTEGER,
		VT_FLOAT,
		VT_STRING,
	};

	WordValue();

public:

	virtual ~WordValue();

	virtual ValueType getType() = 0;

	virtual bool getBool() = 0;
	virtual int getInteger() = 0;
	virtual float getFloat() = 0;
	virtual string getString() = 0;
};
//

class WordValueBool : public WordValue
{
protected:
	bool b_value;


public:

	WordValueBool( bool _b_value );
	~WordValueBool();

	ValueType getType();

	bool getBool();
	int getInteger();
	float getFloat();
	string getString();
};
//

class WordValueInteger : public WordValue
{
protected:
	int i_value;

public:

	WordValueInteger( int _i_value );
	~WordValueInteger();

	ValueType getType();

	bool getBool();
	int getInteger();
	float getFloat();
	string getString();
};
//

class WordValueFloat : public WordValue
{
protected:
	float f_value;

public:

	WordValueFloat( float _f_value );
	~WordValueFloat();

	ValueType getType();

	bool getBool();
	int getInteger();
	float getFloat();
	string getString();
};
//

class WordValueString : public WordValue
{
protected:
	string str_value;

public:

	WordValueString( const string &_str_value );
	~WordValueString();

	ValueType getType();

	bool getBool();
	int getInteger();
	float getFloat();
	string getString();
};
//



class VariableLibrary
{
private:

	static bool b_initialized;

	static Library lib_mappedDictionaries;

	static Dictionary* dt_pActualDictionary;
	static string str_actualDictionaryName;

	static string str_libraryFilename;


	VariableLibrary();
	~VariableLibrary();

	static void setEntry( Dictionary* dt_pDictionary, string _str_key, bool _b_value );
	static void setEntry( Dictionary* dt_pDictionary, string _str_key, int _i_value );
	static void setEntry( Dictionary* dt_pDictionary, string _str_key, float _f_value );
	static void setEntry( Dictionary* dt_pDictionary, string _str_key, string _str_value );


public:

	static void initialize( string _str_libraryFilename );
	static void shutdown();


	static void saveLibrary( string _str_libraryFilename );
	static void saveLibrary();


	static void setValue( string _str_key, bool _b_value );
	static void setValue( string _str_dictionary, string _str_key, bool _b_value );

	static void setValue( string _str_key, int _i_value );
	static void setValue( string _str_dictionary, string _str_key, int _i_value );

	static void setValue( string _str_key, float _f_value );
	static void setValue( string _str_dictionary, string _str_key, float _f_value );

	static void setValue( string _str_key, string _str_value );
	static void setValue( string _str_dictionary, string _str_key, string _str_value );


	static bool getValueExist( string _str_key );
	static bool getValueExist( string _str_dictionary, string _str_key );

	static WordValue::ValueType getValueType( string _str_key );
	static WordValue::ValueType getValueType( string _str_dictionary, string _str_key );


	static bool getValueBool( string _str_key );
	static bool getValueBool( string _str_dictionary, string _str_key );

	static int getValueInt( string _str_key );
	static int getValueInt( string _str_dictionary, string _str_key );

	static float getValueFloat( string _str_key );
	static float getValueFloat( string _str_dictionary, string _str_key );

	static string getValueString( string _str_key );
	static string getValueString( string _str_dictionary, string _str_key );


	static void setUsedDictionary( string _str_dictionary = string("") );
	static string getUsedDictionary();

	static bool getDictionaryExist( string _str_dictionary );
	static void addDictionary( string _str_dictionary );

};

#endif //_VARIABLE_LIBRARY_V3
