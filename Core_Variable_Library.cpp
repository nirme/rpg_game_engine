#include "Core_Variable_Library.h"



bool VariableLibrary::b_initialized = false;
Library VariableLibrary::lib_mappedDictionaries;
Dictionary* VariableLibrary::dt_pActualDictionary = NULL;
string VariableLibrary::str_actualDictionaryName;
string VariableLibrary::str_libraryFilename;




WordValue::WordValue()
{};


WordValue::~WordValue()
{};


WordValueBool::WordValueBool( bool _b_value ) : WordValue(), b_value( _b_value )
{};


WordValueBool::~WordValueBool()
{};


WordValue::ValueType WordValueBool::getType()
{
	return VT_BOOL;
};


bool WordValueBool::getBool()
{
	return b_value;
};


int WordValueBool::getInteger()
{
	#ifdef VL_STRICT_CHECK
		throw ApplicationException( error_vl_wrong_type );
	#else
	return b_value ? 1 : 0;
	#endif
};


float WordValueBool::getFloat()
{
	#ifdef VL_STRICT_CHECK
		throw ApplicationException( error_vl_wrong_type );
	#else
		return b_value ? 1.0f : 0.0f;
	#endif
};


string WordValueBool::getString()
{
	#ifdef VL_STRICT_CHECK
		throw ApplicationException( error_vl_wrong_type );
	#else
		return b_value ? "TRUE" : "FALSE";
	#endif
};


WordValueInteger::WordValueInteger( int _i_value ) : WordValue(), i_value( _i_value )
{};


WordValueInteger::~WordValueInteger()
{};


WordValue::ValueType WordValueInteger::getType()
{
	return VT_INTEGER;
};


bool WordValueInteger::getBool()
{
	#ifdef VL_STRICT_CHECK
		throw ApplicationException( error_vl_wrong_type );
	#else
		return i_value ? true : false;
	#endif
};


int WordValueInteger::getInteger()
{
	return i_value;
};


float WordValueInteger::getFloat()
{
	#ifdef VL_STRICT_CHECK
		throw ApplicationException( error_vl_wrong_type );
	#else
		return (float) i_value;
	#endif
};


string WordValueInteger::getString()
{
	#ifdef VL_STRICT_CHECK
		throw ApplicationException( error_vl_wrong_type );
	#else
		ostringstream ostr;
		ostr << i_value;
		return ostr.str();
	#endif
};


WordValueFloat::WordValueFloat( float _f_value ) : WordValue(), f_value( _f_value )
{};


WordValueFloat::~WordValueFloat()
{};


WordValue::ValueType WordValueFloat::getType()
{
	return VT_FLOAT;
};


bool WordValueFloat::getBool()
{
	#ifdef VL_STRICT_CHECK
		throw ApplicationException( error_vl_wrong_type );
	#else
		return f_value ? true : false;
	#endif
};


int WordValueFloat::getInteger()
{
	#ifdef VL_STRICT_CHECK
		throw ApplicationException( error_vl_wrong_type );
	#else
		return (int) f_value;
	#endif
};


float WordValueFloat::getFloat()
{
	return f_value;
};


string WordValueFloat::getString()
{
	#ifdef VL_STRICT_CHECK
		throw ApplicationException( error_vl_wrong_type );
	#else
		ostringstream ostr;
		ostr << fixed << setprecision(3) << f_value;
		return ostr.str();
	#endif
};


WordValueString::WordValueString( const string &_str_value ) : WordValue()
{
	str_value = _str_value;
};


WordValueString::~WordValueString()
{};


WordValue::ValueType WordValueString::getType()
{
	return VT_BOOL;
};


bool WordValueString::getBool()
{
	#ifdef VL_STRICT_CHECK
		throw ApplicationException( error_vl_wrong_type );
	#else
		return str_value.length() ? true : false;
	#endif
};


int WordValueString::getInteger()
{
	#ifdef VL_STRICT_CHECK
		throw ApplicationException( error_vl_wrong_type );
	#else
		return atoi( str_value.c_str() );
	#endif
};


float WordValueString::getFloat()
{
	#ifdef VL_STRICT_CHECK
		throw ApplicationException( error_vl_wrong_type );
	#else
		return atof( str_value.c_str() );
	#endif
};


string WordValueString::getString()
{
	return str_value;
};




VariableLibrary::VariableLibrary()
{};


VariableLibrary::~VariableLibrary()
{
	shutdown();
};


void VariableLibrary::setEntry( Dictionary* dt_pDictionary, string _str_key, bool _b_value )
{
	Dictionary::iterator it = dt_pDictionary->find( _str_key );

	if ( it != dt_pDictionary->end() )
	{
		SAFE_DELETE( it->second );
		it->second = dynamic_cast <WordValue*> ( new WordValueBool( _b_value ) );
	}
	else
	{
		dt_pDictionary->insert( Dictionary::value_type( _str_key, dynamic_cast <WordValue*> ( new WordValueBool( _b_value ) ) ) );
	}
};


void VariableLibrary::setEntry( Dictionary* dt_pDictionary, string _str_key, int _i_value )
{
	Dictionary::iterator it = dt_pDictionary->find( _str_key );

	if ( it != dt_pDictionary->end() )
	{
		SAFE_DELETE( it->second );
		it->second = dynamic_cast <WordValue*> ( new WordValueInteger( _i_value ) );
	}
	else
	{
		dt_pDictionary->insert( Dictionary::value_type( _str_key, dynamic_cast <WordValue*> ( new WordValueInteger( _i_value ) ) ) );
	}
};


void VariableLibrary::setEntry( Dictionary* dt_pDictionary, string _str_key, float _f_value )
{
	Dictionary::iterator it = dt_pDictionary->find( _str_key );

	if ( it != dt_pDictionary->end() )
	{
		SAFE_DELETE( it->second );
		it->second = dynamic_cast <WordValue*> ( new WordValueFloat( _f_value ) );
	}
	else
	{
		dt_pDictionary->insert( Dictionary::value_type( _str_key, dynamic_cast <WordValue*> ( new WordValueFloat( _f_value ) ) ) );
	}
};


void VariableLibrary::setEntry( Dictionary* dt_pDictionary, string _str_key, string _str_value )
{
	Dictionary::iterator it = dt_pDictionary->find( _str_key );

	if ( it != dt_pDictionary->end() )
	{
		SAFE_DELETE( it->second );
		it->second = dynamic_cast <WordValue*> ( new WordValueString( _str_value ) );
	}
	else
	{
		dt_pDictionary->insert( Dictionary::value_type( _str_key, dynamic_cast <WordValue*> ( new WordValueString( _str_value ) ) ) );
	}
};


void VariableLibrary::initialize( string _str_libraryFilename )
{
	if ( b_initialized )
		shutdown();

	fstream fs_libFile( _str_libraryFilename.c_str(), fstream::in );

	if ( !fs_libFile.is_open() )
		throw ApplicationException( error_vl_file_open, _str_libraryFilename.c_str() );

	Dictionary* pUsedDict = &( lib_mappedDictionaries.insert( Library::value_type( string(""), Dictionary() ) ).first->second );

	char c_line[1024] = {0};

	try
	{
		while ( fs_libFile.eof() )
		{
			fs_libFile.getline( c_line, 1024 );
			string str_line = trim( c_line );

			if ( !str_line.length() || str_line.c_str()[0] == ';' )
				continue;

			if ( str_line.c_str()[0] == '#' )
			{
				string str_dictionaryName = str_line.substr( 1, str_line.length() - 1 );
				Library::iterator lib_it = lib_mappedDictionaries.find( str_dictionaryName );

				if ( lib_it != lib_mappedDictionaries.end() )
					pUsedDict = &( lib_it->second );
				else
					pUsedDict = &( lib_mappedDictionaries.insert( Library::value_type( str_dictionaryName, Dictionary() ) ).first->second );
			}
			else
			{
				UINT t = str_line.find( ':' );
				if ( t == string::npos )
					continue;

				string key = trim( string( str_line.begin(), str_line.begin() + t ) );
				string value = trim( string( str_line.begin() + t + 1, str_line.end() ) );

				if ( !key.length() || !value.length() )
					continue;

				WordValue* wordVal = NULL;

				if ( !value.compare("TRUE") || !value.compare("true") )
				{
					wordVal = new WordValueBool( true );
				}
				else if ( !value.compare("FALSE") || !value.compare("false") )
				{
					wordVal = new WordValueBool( false );
				}
				else if ( value.find_first_not_of( "+-0123456789." ) == string::npos )
				{
					if ( value.find( '.' )  == string::npos )
						wordVal = new WordValueInteger( atoi( value.c_str() ) );
					else
						wordVal = new WordValueFloat( atof( value.c_str() ) );
				}
				else
				{
					if ( (*(value.begin())) == '"' && (*(value.rbegin())) == '"' )
						value = value.substr( 1, value.length() - 2 );

					wordVal = new WordValueString( value );
				}


				pUsedDict->insert( Dictionary::value_type( key, wordVal ) );
			}
		}
	}
	catch ( exception e )
	{
		fs_libFile.close();

		for ( Library::iterator lib_it = lib_mappedDictionaries.begin(); lib_it != lib_mappedDictionaries.end(); ++lib_it )
		{
			for ( Dictionary::iterator dic_it = lib_it->second.begin(); dic_it != lib_it->second.end(); ++dic_it )
			{
				SAFE_DELETE( dic_it->second );
			}
			lib_it->second.clear();
		}

		lib_mappedDictionaries.clear();

		throw ApplicationException( error_vl_file_read );
	}

	fs_libFile.close();

	dt_pActualDictionary = &( lib_mappedDictionaries.find("")->second );
	str_actualDictionaryName = "";

	str_libraryFilename = _str_libraryFilename;

	b_initialized = true;
};


void VariableLibrary::shutdown()
{
	if ( b_initialized )
	{
		for ( Library::iterator lib_it = lib_mappedDictionaries.begin(); lib_it != lib_mappedDictionaries.end(); ++lib_it )
		{
			for ( Dictionary::iterator dic_it = lib_it->second.begin(); dic_it != lib_it->second.end(); ++dic_it )
			{
				SAFE_DELETE( dic_it->second );
			}

			lib_it->second.clear();
		}

		lib_mappedDictionaries.clear();

		dt_pActualDictionary = NULL;
		str_actualDictionaryName = "";
		str_libraryFilename = "";

		b_initialized = false;
	}
};


void VariableLibrary::saveLibrary( string _str_libraryFilename )
{
	fstream fs_outFile( _str_libraryFilename.c_str(), fstream::out | fstream::trunc );
	if ( !fs_outFile.is_open() )
		throw ApplicationException( error_vl_file_open, _str_libraryFilename.c_str() );

	try
	{
		for ( Library::iterator lib_it = lib_mappedDictionaries.begin(); lib_it != lib_mappedDictionaries.end(); ++lib_it )
		{
			fs_outFile << "#" << lib_it->first << endl;

			for ( Dictionary::iterator dic_it = lib_it->second.begin(); dic_it != lib_it->second.end(); ++dic_it )
			{
				fs_outFile << "\t" << dic_it->first << "\t:\t";
				switch ( dic_it->second->getType() )
				{
					case WordValue::VT_BOOL:
					{
						if ( dic_it->second->getBool() )
							fs_outFile << "TRUE";
						else
							fs_outFile << "FALSE";

						break;
					}
					
					case WordValue::VT_INTEGER:
					{
						fs_outFile << dic_it->second->getInteger();

						break;
					}

					case WordValue::VT_FLOAT:
					{
						fs_outFile.unsetf( std::ios::floatfield );
						fs_outFile.precision(14);
						fs_outFile << dic_it->second->getFloat();

						break;
					}

					case WordValue::VT_STRING:
					{
						fs_outFile << "\"" << dic_it->second->getString() << "\"";

						break;
					}

					default:
						break;
				}

				fs_outFile << endl;
			}
		}
	}
	catch ( exception e )
	{
		throw ApplicationException( error_vl_file_write );
	}
};


void VariableLibrary::saveLibrary()
{
	saveLibrary( str_libraryFilename );
};


void VariableLibrary::setValue( string _str_key, bool _b_value )
{
	if ( !dt_pActualDictionary )
		throw ApplicationException( error_vl_no_dict );
	setEntry( dt_pActualDictionary, _str_key, _b_value );
};


void VariableLibrary::setValue( string _str_dictionary, string _str_key, bool _b_value )
{
	Library::iterator it = lib_mappedDictionaries.find( _str_dictionary );
	if ( it == lib_mappedDictionaries.end() )
		throw ApplicationException( error_vl_no_dict );
	setEntry( &( it->second ), _str_key, _b_value );
};


void VariableLibrary::setValue( string _str_key, int _i_value )
{
	if ( !dt_pActualDictionary )
		throw ApplicationException( error_vl_no_dict );
	setEntry( dt_pActualDictionary, _str_key, _i_value );
};


void VariableLibrary::setValue( string _str_dictionary, string _str_key, int _i_value )
{
	Library::iterator it = lib_mappedDictionaries.find( _str_dictionary );
	if ( it == lib_mappedDictionaries.end() )
		throw ApplicationException( error_vl_no_dict );
	setEntry( &( it->second ), _str_key, _i_value );
};


void VariableLibrary::setValue( string _str_key, float _f_value )
{
	if ( !dt_pActualDictionary )
		throw ApplicationException( error_vl_no_dict );
	setEntry( dt_pActualDictionary, _str_key, _f_value );
};


void VariableLibrary::setValue( string _str_dictionary, string _str_key, float _f_value )
{
	Library::iterator it = lib_mappedDictionaries.find( _str_dictionary );
	if ( it == lib_mappedDictionaries.end() )
		throw ApplicationException( error_vl_no_dict );
	setEntry( &( it->second ), _str_key, _f_value );
};


void VariableLibrary::setValue( string _str_key, string _str_value )
{
	if ( !dt_pActualDictionary )
		throw ApplicationException( error_vl_no_dict );
	setEntry( dt_pActualDictionary, _str_key, _str_value );
};


void VariableLibrary::setValue( string _str_dictionary, string _str_key, string _str_value )
{
	Library::iterator it = lib_mappedDictionaries.find( _str_dictionary );
	if ( it == lib_mappedDictionaries.end() )
		throw ApplicationException( error_vl_no_dict );
	setEntry( &( it->second ), _str_key, _str_value );
};


bool VariableLibrary::getValueExist( string _str_key )
{
	if ( !dt_pActualDictionary )
		throw ApplicationException( error_vl_no_dict );
	return dt_pActualDictionary->find( _str_key ) != dt_pActualDictionary->end();
};


bool VariableLibrary::getValueExist( string _str_dictionary, string _str_key )
{
	Library::iterator it = lib_mappedDictionaries.find( _str_dictionary );
	if ( it == lib_mappedDictionaries.end() )
		throw ApplicationException( error_vl_no_dict );
	return it->second.find( _str_key ) != it->second.end();
};


WordValue::ValueType VariableLibrary::getValueType( string _str_key )
{
	if ( !dt_pActualDictionary )
		throw ApplicationException( error_vl_no_dict );

	Dictionary::iterator it = dt_pActualDictionary->find( _str_key );
	
	if ( it == dt_pActualDictionary->end() )
		throw ApplicationException( error_vl_no_entry );

	return it->second->getType();
};


WordValue::ValueType VariableLibrary::getValueType( string _str_dictionary, string _str_key )
{
	Library::iterator lib_it = lib_mappedDictionaries.find( _str_dictionary );
	if ( lib_it == lib_mappedDictionaries.end() )
		throw ApplicationException( error_vl_no_dict );
	Dictionary::iterator it = lib_it->second.find( _str_key );
	
	if ( it == lib_it->second.end() )
		throw ApplicationException( error_vl_no_entry );

	return it->second->getType();
};


bool VariableLibrary::getValueBool( string _str_key )
{
	if ( !dt_pActualDictionary )
		throw ApplicationException( error_vl_no_dict );

	Dictionary::iterator it = dt_pActualDictionary->find( _str_key );
	
	if ( it == dt_pActualDictionary->end() )
		throw ApplicationException( error_vl_no_entry );

	return it->second->getBool();
};


bool VariableLibrary::getValueBool( string _str_dictionary, string _str_key )
{
	Library::iterator lib_it = lib_mappedDictionaries.find( _str_dictionary );
	if ( lib_it == lib_mappedDictionaries.end() )
		throw ApplicationException( error_vl_no_dict );
	Dictionary::iterator it = lib_it->second.find( _str_key );
	
	if ( it == lib_it->second.end() )
		throw ApplicationException( error_vl_no_entry );

	return it->second->getBool();
};


int VariableLibrary::getValueInt( string _str_key )
{
	if ( !dt_pActualDictionary )
		throw ApplicationException( error_vl_no_dict );

	Dictionary::iterator it = dt_pActualDictionary->find( _str_key );
	
	if ( it == dt_pActualDictionary->end() )
		throw ApplicationException( error_vl_no_entry );

	return it->second->getInteger();
};


int VariableLibrary::getValueInt( string _str_dictionary, string _str_key )
{
	Library::iterator lib_it = lib_mappedDictionaries.find( _str_dictionary );
	if ( lib_it == lib_mappedDictionaries.end() )
		throw ApplicationException( error_vl_no_dict );
	Dictionary::iterator it = lib_it->second.find( _str_key );
	
	if ( it == lib_it->second.end() )
		throw ApplicationException( error_vl_no_entry );

	return it->second->getInteger();
};


float VariableLibrary::getValueFloat( string _str_key )
{
	if ( !dt_pActualDictionary )
		throw ApplicationException( error_vl_no_dict );

	Dictionary::iterator it = dt_pActualDictionary->find( _str_key );
	
	if ( it == dt_pActualDictionary->end() )
		throw ApplicationException( error_vl_no_entry );

	return it->second->getFloat();
};


float VariableLibrary::getValueFloat( string _str_dictionary, string _str_key )
{
	Library::iterator lib_it = lib_mappedDictionaries.find( _str_dictionary );
	if ( lib_it == lib_mappedDictionaries.end() )
		throw ApplicationException( error_vl_no_dict );
	Dictionary::iterator it = lib_it->second.find( _str_key );
	
	if ( it == lib_it->second.end() )
		throw ApplicationException( error_vl_no_entry );

	return it->second->getFloat();
};


string VariableLibrary::getValueString( string _str_key )
{
	if ( !dt_pActualDictionary )
		throw ApplicationException( error_vl_no_dict );

	Dictionary::iterator it = dt_pActualDictionary->find( _str_key );
	
	if ( it == dt_pActualDictionary->end() )
		throw ApplicationException( error_vl_no_entry );

	return it->second->getString();
};


string VariableLibrary::getValueString( string _str_dictionary, string _str_key )
{
	Library::iterator lib_it = lib_mappedDictionaries.find( _str_dictionary );
	if ( lib_it == lib_mappedDictionaries.end() )
		throw ApplicationException( error_vl_no_dict );
	Dictionary::iterator it = lib_it->second.find( _str_key );
	
	if ( it == lib_it->second.end() )
		throw ApplicationException( error_vl_no_entry );

	return it->second->getString();
};


void VariableLibrary::setUsedDictionary( string _str_dictionary )
{
	Library::iterator lib_it = lib_mappedDictionaries.find( _str_dictionary );
	if ( lib_it == lib_mappedDictionaries.end() )
		throw ApplicationException( error_vl_no_dict );

	str_actualDictionaryName = lib_it->first;
	dt_pActualDictionary = &( lib_it->second );
};


string VariableLibrary::getUsedDictionary()
{
	if ( !dt_pActualDictionary )
		throw ApplicationException( error_vl_no_dict );
	return str_actualDictionaryName;
};


bool VariableLibrary::getDictionaryExist( string _str_dictionary )
{
	Library::iterator lib_it = lib_mappedDictionaries.find( _str_dictionary );
	return lib_it != lib_mappedDictionaries.end();
};


void VariableLibrary::addDictionary( string _str_dictionary )
{
	Library::iterator lib_it = lib_mappedDictionaries.find( _str_dictionary );
	if ( lib_it == lib_mappedDictionaries.end() )
	{
		lib_mappedDictionaries.insert( Library::value_type( _str_dictionary, Dictionary() ) );
	}
};
