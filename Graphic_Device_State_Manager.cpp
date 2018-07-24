#include "Graphic_Device_State_Manager.h"


namespace Renderer
{

	const BYTE DeviceStateManager::by_renderStateUsedIndexes[ /*RENDER_STATES_NUMBER*/ ] = 
	{
		  7,   8,   9,  14,  15,  16,  19,  20,  22,  23,  24,  25,  26,  27,  28,  29,  34,  35,  36,  37,  38,  48,  52,  53,  54,  55, 
		 56,  57,  58,  59,  60, 128, 129, 130, 131, 132, 133, 134, 135, 136, 137, 139, 140, 141, 142, 143, 145, 146, 147, 148, 151, 152, 
		154, 155, 156, 157, 158, 159, 160, 161, 162, 163, 165, 166, 167, 168, 170, 171, 172, 173, 174, 175, 176, 178, 179, 180, 181, 182, 
		183, 184, 185, 186, 187, 188, 189, 190, 191, 192, 193, 194, 195, 198, 199, 200, 201, 202, 203, 204, 205, 206, 207, 208, 209
	};


	const DeviceStateManager::ApplySCMethod DeviceStateManager::applySCMethods[ 2 ][ 3 ] = 
	{
		{
			&DeviceStateManager::applyPixelShaderConstantFloat, 
			&DeviceStateManager::applyPixelShaderConstantInt, 
			&DeviceStateManager::applyPixelShaderConstantBool
		},
		{
			&DeviceStateManager::applyVertexShaderConstantFloat, 
			&DeviceStateManager::applyVertexShaderConstantInt, 
			&DeviceStateManager::applyVertexShaderConstantBool
		}
	};


	const DeviceStateManager::ApplySCMethodAd DeviceStateManager::applySCMethodAd[ 2 ][ 3 ] = 
	{
		{
			&DeviceStateManager::applyPixelShaderConstantFloat, 
			&DeviceStateManager::applyPixelShaderConstantInt, 
			&DeviceStateManager::applyPixelShaderConstantBool
		},
		{
			&DeviceStateManager::applyVertexShaderConstantFloat,
			&DeviceStateManager::applyVertexShaderConstantInt,
			&DeviceStateManager::applyVertexShaderConstantBool
		}
	};


	const DeviceStateManager::SetShaderConstantF DeviceStateManager::applySCMethodsF[ /*2*/ ] = 
	{
		&IDirect3DDevice9::SetPixelShaderConstantF,
		&IDirect3DDevice9::SetVertexShaderConstantF
	};


	const DeviceStateManager::SetShaderConstantI DeviceStateManager::applySCMethodsI[ /*2*/ ] = 
	{
		&IDirect3DDevice9::SetPixelShaderConstantI,
		&IDirect3DDevice9::SetVertexShaderConstantI
	};


	const DeviceStateManager::SetShaderConstantB DeviceStateManager::applySCMethodsB[ /*2*/ ] = 
	{
		&IDirect3DDevice9::SetPixelShaderConstantB,
		&IDirect3DDevice9::SetVertexShaderConstantB
	};




	DeviceStateManager::DeviceStateManager( IDirect3DDevice9* _pD3D9_device ) : pD3D9_device( _pD3D9_device ), pDxVS_vertexShaderActual( NULL ), pDxPS_pixelShaderActual( NULL ), pDxVD_vertexDeclarationActual( NULL ), pDxIB_indexDataActual( NULL )
	{
		for ( UINT i = 0; i < 255; ++i )
		{
			pDw_renderStateBaseValues[ i ] = 0;
			pB_renderStateUsedStates[ i ] = false;
			pDw_renderStateModificationValues[ i ] = 0;
			plBtIt_renderStateModificationFlag[ i ] = lBt_renderStateModifiedIndexes.end();
			dw_renderStateActualValues[ i ] = 0;
		}

		HRESULT hr = S_OK;

		for ( BYTE i = 0; i < RENDER_STATES_NUMBER; ++i )
		{
			pB_renderStateUsedStates[ by_renderStateUsedIndexes[ i ] ] = true;

			if ( FAILED( hr = pD3D9_device->GetRenderState( (D3DRENDERSTATETYPE) by_renderStateUsedIndexes[ i ], &( dw_renderStateActualValues[ by_renderStateUsedIndexes[ i ] ] ) ) ) )
			{
				throw D3DX9Exception( "DeviceStateManager::DeviceStateManager", "IDirect3DDevice9::GetRenderState", hr );
			}

			pDw_renderStateBaseValues[ i ] = dw_renderStateActualValues[ by_renderStateUsedIndexes[ i ] ];
		}

		for ( BYTE si = 0; si < SAMPLERS_NUMBER; ++si )
		{
			for ( BYTE ssi = 0; ssi < SAMPLER_STATES_NUMBER; ++ssi )
			{
				ppDw_samplerStateBaseValues[ si ][ ssi ] = 0;
				ppDw_samplerStateModificationValues[ si ][ ssi ] = 0;
				pplBtIt_samplerStateModifiedIndexes[ si ][ ssi ] = pLBt_samplerStateModifiedIndexes[ si ].end();

				if ( FAILED( hr = pD3D9_device->GetSamplerState( si, (D3DSAMPLERSTATETYPE) (ssi + 1), &( ppDw_samplerStateActualValues[ si ][ ssi ] ) ) ) )
				{
					throw D3DX9Exception( "DeviceStateManager::DeviceStateManager", "IDirect3DDevice9::GetSamplerState", hr );
				}

				ppDw_samplerStateBaseValues[ si ][ ssi ] = ppDw_samplerStateActualValues[ si ][ ssi ];
			}

			plBtIt_samplerModifiedIndexes[ si ] = lBt_samplerModifiedIndexes.end();
			pDxTx_samplerActualTexture[ si ] = NULL;
		}


		for ( UINT i = 0; i < STREAMS_NUMBER; ++i )
		{
			pSD_StreamsDefinitionActual[ i ].ui_streamNumber = i;
			pSD_StreamsDefinitionActual[ i ].pDxVB_streamData = NULL;
			pSD_StreamsDefinitionActual[ i ].ui_stride = 0;
			pSD_StreamsDefinitionActual[ i ].ui_frequencyParameter = 1;
			pSD_StreamsDefinitionActual[ i ].b_geometryIndexed = false;
			pSD_StreamsDefinitionActual[ i ].b_instanceData = false;
			pSD_StreamsDefinitionActual[ i ].ui_frequencySetValue = 1;
		}


		pD3D9_device->AddRef();
	};


	DeviceStateManager::~DeviceStateManager()
	{
		if ( pD3D9_device )
		{
			for ( DWORD si = 0; si < SAMPLERS_NUMBER; ++si )
			{
				if ( pDxTx_samplerActualTexture[ si ] )
				{
					pD3D9_device->SetTexture( si, NULL );
					pDxTx_samplerActualTexture[ si ]->Release();
					pDxTx_samplerActualTexture[ si ] = NULL;
				}
			}

			if ( pDxVS_vertexShaderActual )
			{
				pD3D9_device->SetVertexShader( NULL );
				pDxVS_vertexShaderActual->Release();
				pDxVS_vertexShaderActual = NULL;
			}

			if ( pDxPS_pixelShaderActual )
			{
				pD3D9_device->SetPixelShader( NULL );
				pDxPS_pixelShaderActual->Release();
				pDxPS_pixelShaderActual = NULL;
			}

			if ( pDxVD_vertexDeclarationActual )
			{
				pD3D9_device->SetVertexDeclaration( NULL );
				pDxVD_vertexDeclarationActual->Release();
				pDxVD_vertexDeclarationActual = NULL;
			}

			if ( pDxIB_indexDataActual )
			{
				pD3D9_device->SetIndices( NULL );
				pDxIB_indexDataActual->Release();
				pDxIB_indexDataActual = NULL;
			}

			for ( UINT i = 0; i < STREAMS_NUMBER; ++i )
			{
				if ( pSD_StreamsDefinitionActual[ i ].pDxVB_streamData )
				{
					pD3D9_device->SetStreamSource( i, NULL, 0, 0 );
					pSD_StreamsDefinitionActual[ i ].pDxVB_streamData = NULL;
					pSD_StreamsDefinitionActual[ i ].ui_stride = 0;
				}

				if ( pSD_StreamsDefinitionActual[ i ].ui_frequencySetValue )
				{
					pD3D9_device->SetStreamSourceFreq( i, 1 );
					pSD_StreamsDefinitionActual[ i ].ui_frequencyParameter = 1;
					pSD_StreamsDefinitionActual[ i ].b_geometryIndexed = false;
					pSD_StreamsDefinitionActual[ i ].b_instanceData = false;
					pSD_StreamsDefinitionActual[ i ].ui_frequencySetValue = 1;
				}
			}

			pD3D9_device->Release();
		}
	};


	void DeviceStateManager::setBaseRenderState( D3DRENDERSTATETYPE _dxRst_renderStateType, DWORD _dw_value )
	{
		pDw_renderStateBaseValues[ _dxRst_renderStateType ] = _dw_value;
	};


	void DeviceStateManager::setBaseSamplerState( BYTE _bt_samplerIndex, D3DSAMPLERSTATETYPE _dxSst_samplerStateType, DWORD _dw_value )
	{
		ppDw_samplerStateBaseValues[ _bt_samplerIndex ][ _dxSst_samplerStateType - 1 ] = _dw_value;
	};


	void DeviceStateManager::pushRenderState( BYTE _bt_state, DWORD _dw_value )
	{
		if ( pB_renderStateUsedStates[ _bt_state ] )
		{
			if ( dw_renderStateActualValues[ _bt_state ] != _dw_value )
			{
				pDw_renderStateModificationValues[ _bt_state ] = _dw_value;

				if ( plBtIt_renderStateModificationFlag[ _bt_state ] == lBt_renderStateModifiedIndexes.end() )
				{
					lBt_renderStateModifiedIndexes.push_front( _bt_state );
					plBtIt_renderStateModificationFlag[ _bt_state ] = lBt_renderStateModifiedIndexes.begin();
				}
			}
			else if ( plBtIt_renderStateModificationFlag[ _bt_state ] != lBt_renderStateModifiedIndexes.end() )
			{
				lBt_renderStateModifiedIndexes.erase( plBtIt_renderStateModificationFlag[ _bt_state ] );
				plBtIt_renderStateModificationFlag[ _bt_state ] = lBt_renderStateModifiedIndexes.end();
			}
		}
	};


	void DeviceStateManager::pushRenderStates( BYTE _bt_statesNumber, const RENDER_STATE* _pRS_values )
	{
		list<BYTE>::iterator it_end = lBt_renderStateModifiedIndexes.end();

		while ( !lBt_renderStateModifiedIndexes.empty() )
		{
			plBtIt_renderStateModificationFlag[ lBt_renderStateModifiedIndexes.front() ] = it_end;
			lBt_renderStateModifiedIndexes.pop_front();
		}

		for ( BYTE i = 0; i < _bt_statesNumber; ++i )
		{
			if ( dw_renderStateActualValues[ _pRS_values[i].state ] != _pRS_values[i].value )
			{
				pDw_renderStateModificationValues[ _pRS_values[i].state ] = _pRS_values[i].value;
				lBt_renderStateModifiedIndexes.push_front( (BYTE) _pRS_values[i].state );
				plBtIt_renderStateModificationFlag[ _pRS_values[i].state ] = lBt_renderStateModifiedIndexes.begin();
			}
		}
	};


	void DeviceStateManager::pushSamplerState( DWORD _dw_sampler, D3DSAMPLERSTATETYPE _dSS_state, DWORD _dw_value )
	{
		BYTE dw_stateIndex = _dSS_state - 1;

		if ( ppDw_samplerStateActualValues[ _dw_sampler ][ dw_stateIndex ] != _dw_value )
		{
			ppDw_samplerStateModificationValues[ _dw_sampler ][ dw_stateIndex ] = _dw_value;

			if ( plBtIt_samplerModifiedIndexes[ _dw_sampler ] == lBt_samplerModifiedIndexes.end() )
			{
				lBt_samplerModifiedIndexes.push_front( _dw_sampler );
				plBtIt_samplerModifiedIndexes[ _dw_sampler ] == lBt_samplerModifiedIndexes.begin();

				pLBt_samplerStateModifiedIndexes[ _dw_sampler ].push_front( dw_stateIndex );
				pplBtIt_samplerStateModifiedIndexes[ _dw_sampler ][ dw_stateIndex ] = pLBt_samplerStateModifiedIndexes[ _dw_sampler ].begin();
			}
			else if ( pplBtIt_samplerStateModifiedIndexes[ _dw_sampler ][ dw_stateIndex ] == pLBt_samplerStateModifiedIndexes[ _dw_sampler ].end() )
			{
				pLBt_samplerStateModifiedIndexes[ _dw_sampler ].push_front( dw_stateIndex );
				pplBtIt_samplerStateModifiedIndexes[ _dw_sampler ][ dw_stateIndex ] = pLBt_samplerStateModifiedIndexes[ _dw_sampler ].begin();
			}
		}
		else if ( pplBtIt_samplerStateModifiedIndexes[ _dw_sampler ][ dw_stateIndex ] != pLBt_samplerStateModifiedIndexes[ _dw_sampler ].end() )
		{
			pLBt_samplerStateModifiedIndexes[ _dw_sampler ].erase( pplBtIt_samplerStateModifiedIndexes[ _dw_sampler ][ dw_stateIndex ] );
			pplBtIt_samplerStateModifiedIndexes[ _dw_sampler ][ dw_stateIndex ] = pLBt_samplerStateModifiedIndexes[ _dw_sampler ].end();
			if ( pLBt_samplerStateModifiedIndexes[ _dw_sampler ].empty() )
			{
				plBtIt_samplerModifiedIndexes[ _dw_sampler ] = lBt_samplerModifiedIndexes.end();
			}
		}
	};


	void DeviceStateManager::pushSamplerStates( const SAMPLER_DEFINITION* _pSD_samplerDef )
	{
		bool b_changeFlag = false;
		bool b_samplerModSet = plBtIt_samplerModifiedIndexes[ _pSD_samplerDef->dw_samplerIndex ] != lBt_samplerModifiedIndexes.end();

		pLBt_samplerStateModifiedIndexes[ _pSD_samplerDef->dw_samplerIndex ].clear();
		list< BYTE >::iterator it_end = pLBt_samplerStateModifiedIndexes[ _pSD_samplerDef->dw_samplerIndex ].end();

		for ( BYTE i = 0; i < SAMPLER_STATES_NUMBER; ++i )
		{
			if ( _pSD_samplerDef->b_valuesUsed[ i ] && ppDw_samplerStateActualValues[ _pSD_samplerDef->dw_samplerIndex ][ i ] != _pSD_samplerDef->dw_values[i] )
			{
				ppDw_samplerStateModificationValues[ _pSD_samplerDef->dw_samplerIndex ][ i ] = _pSD_samplerDef->dw_values[i];
				pLBt_samplerStateModifiedIndexes[ i ].push_front( i );
				pplBtIt_samplerStateModifiedIndexes[ _pSD_samplerDef->dw_samplerIndex ][ i ] = pLBt_samplerStateModifiedIndexes[ i ].begin();
				b_changeFlag = true;
			}
			else
			{
				pplBtIt_samplerStateModifiedIndexes[ _pSD_samplerDef->dw_samplerIndex ][ i ] = it_end;
			}


			if ( b_samplerModSet && !b_changeFlag )
			{
				lBt_samplerModifiedIndexes.erase( plBtIt_samplerModifiedIndexes[ _pSD_samplerDef->dw_samplerIndex ] );
				plBtIt_samplerModifiedIndexes[ _pSD_samplerDef->dw_samplerIndex ] = lBt_samplerModifiedIndexes.end();
			}
			else if ( !b_samplerModSet && b_changeFlag )
			{
				lBt_samplerModifiedIndexes.push_front( _pSD_samplerDef->dw_samplerIndex );
				plBtIt_samplerModifiedIndexes[ _pSD_samplerDef->dw_samplerIndex ] = lBt_samplerModifiedIndexes.begin();
			}
		}
	};


	void DeviceStateManager::applyRenderStatesToActual( BYTE _bt_statesNumber, const RENDER_STATE* _pRS_values )
	{
		list<BYTE>::iterator it_end = lBt_renderStateModifiedIndexes.end();

		while ( !lBt_renderStateModifiedIndexes.empty() )
		{
			plBtIt_renderStateModificationFlag[ lBt_renderStateModifiedIndexes.front() ] = it_end;
			lBt_renderStateModifiedIndexes.pop_front();
		}

		HRESULT hr = S_OK;

		for ( BYTE i = 0; i < _bt_statesNumber; ++i )
		{
			if ( dw_renderStateActualValues[ _pRS_values[i].state ] != _pRS_values[i].value )
			{
				if ( FAILED( hr = pD3D9_device->SetRenderState( _pRS_values[i].state, _pRS_values[i].value ) ) )
				{
					throw D3DX9Exception( "DeviceStateManager::applyRenderStatesToActual", "IDirect3DDevice9::SetRenderState", hr );
				}

				dw_renderStateActualValues[ _pRS_values[i].state ] = _pRS_values[i].value;
			}
		}
	};


	void DeviceStateManager::applySamplerStatesToActual( const SAMPLER_DEFINITION* _pSD_samplerDef )
	{
		BYTE samplerIndex = _pSD_samplerDef->dw_samplerIndex;
		list<BYTE>::iterator it_end = lBt_samplerModifiedIndexes.end();
		if ( plBtIt_samplerModifiedIndexes[ samplerIndex ] != it_end )
		{
			lBt_samplerModifiedIndexes.erase( plBtIt_samplerModifiedIndexes[ samplerIndex ] );
			plBtIt_samplerModifiedIndexes[ samplerIndex ] = it_end;

			it_end = pLBt_samplerStateModifiedIndexes[ samplerIndex ].end();

			while ( !pLBt_samplerStateModifiedIndexes[ samplerIndex ].empty() )
			{
				pplBtIt_samplerStateModifiedIndexes[ samplerIndex ][ pLBt_samplerStateModifiedIndexes[ samplerIndex ].front() ] = it_end;
				pLBt_samplerStateModifiedIndexes[ samplerIndex ].pop_front();
			}
		}

		HRESULT hr = S_OK;

		for ( BYTE i = 0; i < SAMPLER_STATES_NUMBER; ++i )
		{
			if ( _pSD_samplerDef->b_valuesUsed && ppDw_samplerStateActualValues[ samplerIndex ][ i ] != _pSD_samplerDef->dw_values[ i ] )
			{
				
				if ( FAILED( hr = pD3D9_device->SetSamplerState( samplerIndex, (D3DSAMPLERSTATETYPE) ( i + 1 ), _pSD_samplerDef->dw_values[ i ] ) ) )
				{
					throw D3DX9Exception( "DeviceStateManager::applySamplerStatesToActual", "IDirect3DDevice9::SetSamplerState", hr );
				}
				ppDw_samplerStateActualValues[ samplerIndex ][ i ] = _pSD_samplerDef->dw_values[ i ];
			}
		}
	};


	void DeviceStateManager::applyRenderStatesToBase( BYTE _bt_statesNumber, const RENDER_STATE* _pRS_values )
	{
		list<BYTE>::iterator it_end = lBt_renderStateModifiedIndexes.end();
		while ( !lBt_renderStateModifiedIndexes.empty() )
		{
			plBtIt_renderStateModificationFlag[ lBt_renderStateModifiedIndexes.front() ] = it_end;
			lBt_renderStateModifiedIndexes.pop_front();
		}

		memcpy( pDw_renderStateModificationValues, pDw_renderStateBaseValues, 255 * sizeof( DWORD ) );

		for ( BYTE i = 0; i < _bt_statesNumber; ++i )
		{
			pDw_renderStateModificationValues[ _pRS_values[ i ].state ] = _pRS_values[ i ].value;
		};

		HRESULT hr = S_OK;

		for ( BYTE i = 0; i < RENDER_STATES_NUMBER; ++i )
		{
			if ( dw_renderStateActualValues[ by_renderStateUsedIndexes[ i ] ] != pDw_renderStateModificationValues[ by_renderStateUsedIndexes[ i ] ] )
			{
				if ( FAILED( hr = pD3D9_device->SetRenderState( (D3DRENDERSTATETYPE) by_renderStateUsedIndexes[ i ], dw_renderStateActualValues[ by_renderStateUsedIndexes[ i ] ] ) ) )
				{
					throw D3DX9Exception( "DeviceStateManager::applyRenderStatesToBase", "IDirect3DDevice9::SetRenderState", hr );
				}

				dw_renderStateActualValues[ by_renderStateUsedIndexes[ i ] ] = pDw_renderStateModificationValues[ by_renderStateUsedIndexes[ i ] ];
			}

		};
	};


	void DeviceStateManager::applySamplerStatesToBase( const SAMPLER_DEFINITION* _pSD_samplerDef )
	{
		BYTE samplerIndex = _pSD_samplerDef->dw_samplerIndex;
		if ( plBtIt_samplerModifiedIndexes[ samplerIndex ] != lBt_samplerModifiedIndexes.end() )
		{
			while ( !pLBt_samplerStateModifiedIndexes[ samplerIndex ].empty() )
			{
				pplBtIt_samplerStateModifiedIndexes[ samplerIndex ][ pLBt_samplerStateModifiedIndexes[ samplerIndex ].front() ] = pLBt_samplerStateModifiedIndexes[ samplerIndex ].end();
				pLBt_samplerStateModifiedIndexes[ samplerIndex ].pop_front();
			}
		}


		DWORD value = 0;
		HRESULT hr = S_OK;

		for ( BYTE i = 0; i < SAMPLER_STATES_NUMBER; ++i )
		{
			value = _pSD_samplerDef->b_valuesUsed[ i ] ? _pSD_samplerDef->dw_values[ i ] : ppDw_samplerStateBaseValues[ samplerIndex ][ i ];

			if ( ppDw_samplerStateActualValues[ samplerIndex ][ i ] != value )
			{
				if ( FAILED( hr = pD3D9_device->SetSamplerState( samplerIndex, (D3DSAMPLERSTATETYPE) ( i + 1 ), value ) ) )
				{
					throw D3DX9Exception( "DeviceStateManager::applySamplerStatesToBase", "IDirect3DDevice9::SetSamplerState", hr );
				}

				ppDw_samplerStateActualValues[ samplerIndex ][ i ] = value;
			}
		}
	};


	void DeviceStateManager::applyRenderState( BYTE _bt_state, DWORD _dw_value )
	{
		HRESULT hr = S_OK;

		if ( pB_renderStateUsedStates[ _bt_state ] && dw_renderStateActualValues[ _bt_state ] != _dw_value )
		{
			if ( FAILED( hr = pD3D9_device->SetRenderState( (D3DRENDERSTATETYPE) _bt_state, _dw_value ) ) )
			{
				throw D3DX9Exception( "DeviceStateManager::applyRenderState", "IDirect3DDevice9::SetRenderState", hr );
			}

			dw_renderStateActualValues[ _bt_state ] = _dw_value;
		}
	};


	void DeviceStateManager::applySamplerState( DWORD _dw_sampler, D3DSAMPLERSTATETYPE _dSS_state, DWORD _dw_value )
	{
		HRESULT hr = S_OK;

		if ( ppDw_samplerStateActualValues[ _dw_sampler ][ _dSS_state - 1 ] != _dw_value )
		{
			if ( FAILED( hr = pD3D9_device->SetSamplerState( _dw_sampler, _dSS_state, _dw_value ) ) )
			{
				throw D3DX9Exception( "DeviceStateManager::applySamplerState", "IDirect3DDevice9::SetSamplerState", hr );
			}

			ppDw_samplerStateActualValues[ _dw_sampler ][ _dSS_state - 1 ] = _dw_value;
		}
	};


	void DeviceStateManager::applyRenderStateToActual()
	{
		HRESULT hr = S_OK;

		while ( !lBt_renderStateModifiedIndexes.empty() )
		{
			BYTE index = lBt_renderStateModifiedIndexes.front();

			if ( FAILED( hr = pD3D9_device->SetRenderState( (D3DRENDERSTATETYPE) index, pDw_renderStateModificationValues[ index ] ) ) )
			{
				throw D3DX9Exception( "DeviceStateManager::applyRenderStateToActual", "IDirect3DDevice9::SetRenderState", hr );
			}

			plBtIt_renderStateModificationFlag[ index ] = lBt_renderStateModifiedIndexes.end();
			dw_renderStateActualValues[ index ] = pDw_renderStateModificationValues[ index ];
			lBt_renderStateModifiedIndexes.pop_front();
		}
	};


	void DeviceStateManager::applySamplerStateToActual()
	{
		HRESULT hr = S_OK;

		while ( !lBt_samplerModifiedIndexes.empty() )
		{
			BYTE sampler_index = lBt_samplerModifiedIndexes.front();

			while ( !pLBt_samplerStateModifiedIndexes[ sampler_index ].empty() )
			{
				BYTE index = pLBt_samplerStateModifiedIndexes[ sampler_index ].front();

				if ( FAILED( hr = pD3D9_device->SetSamplerState( (DWORD) sampler_index, (D3DSAMPLERSTATETYPE) ( index + 1 ), ppDw_samplerStateModificationValues[ sampler_index ][ index ] ) ) )
				{
					throw D3DX9Exception( "DeviceStateManager::applySamplerStateToActual", "IDirect3DDevice9::SetSamplerState", hr );
				}

				pplBtIt_samplerStateModifiedIndexes[ sampler_index ][ index ] = pLBt_samplerStateModifiedIndexes[ sampler_index ].end();
				ppDw_samplerStateActualValues[ sampler_index ][ index ] = ppDw_samplerStateModificationValues[ sampler_index ][ index ];
				pLBt_samplerStateModifiedIndexes[ sampler_index ].pop_front();
			}

			plBtIt_samplerModifiedIndexes[ sampler_index ] = lBt_samplerModifiedIndexes.end();
			lBt_samplerModifiedIndexes.pop_front();
		}
	};


	void DeviceStateManager::applyRenderStateToBase()
	{
		HRESULT hr = S_OK;

		for ( BYTE i = 0; i < RENDER_STATES_NUMBER; ++i )
		{
			BYTE index = by_renderStateUsedIndexes[ i ];

			if ( plBtIt_renderStateModificationFlag[ index ] != lBt_renderStateModifiedIndexes.end() )
			{
				if ( dw_renderStateActualValues[ index ] != pDw_renderStateModificationValues[ index ] )
				{
					if ( FAILED( hr = pD3D9_device->SetRenderState( (D3DRENDERSTATETYPE) index, pDw_renderStateModificationValues[ index ] ) ) )
					{
						throw D3DX9Exception( "DeviceStateManager::applyRenderStateToBase", "IDirect3DDevice9::SetRenderState", hr );
					}

					dw_renderStateActualValues[ index ] = pDw_renderStateModificationValues[ index ];
				}

				lBt_renderStateModifiedIndexes.erase( plBtIt_renderStateModificationFlag[ index ] );
				plBtIt_renderStateModificationFlag[ index ] = lBt_renderStateModifiedIndexes.end();
			}
			else if ( dw_renderStateActualValues[ index ] != pDw_renderStateBaseValues[ index ] )
			{
				if ( FAILED( hr = pD3D9_device->SetRenderState( (D3DRENDERSTATETYPE) index, pDw_renderStateBaseValues[ index ] ) ) )
				{
					throw D3DX9Exception( "DeviceStateManager::applyRenderStateToBase", "IDirect3DDevice9::SetRenderState", hr );
				}
				dw_renderStateActualValues[ index ] = pDw_renderStateBaseValues[ index ];
			}
		}

		lBt_renderStateModifiedIndexes.clear();
	};


	void DeviceStateManager::applySamplerStateToBase()
	{
		HRESULT hr = S_OK;

		for ( BYTE sampler_index = 0; sampler_index < SAMPLERS_NUMBER; ++sampler_index )
		{
			if ( plBtIt_samplerModifiedIndexes[ sampler_index ] != lBt_samplerModifiedIndexes.end() )
			{
				for ( BYTE index = 0; index < SAMPLER_STATES_NUMBER; ++index )
				{
					if ( pplBtIt_samplerStateModifiedIndexes[ sampler_index ][ index ] != pLBt_samplerStateModifiedIndexes[ sampler_index ].end() )
					{
						if ( ppDw_samplerStateActualValues[ sampler_index ][ index ] != ppDw_samplerStateModificationValues[ sampler_index ][ index ] )
						{
							if ( FAILED( hr = pD3D9_device->SetSamplerState( (DWORD) sampler_index, (D3DSAMPLERSTATETYPE) ( index + 1 ), ppDw_samplerStateModificationValues[ sampler_index ][ index ] ) ) )
							{
								throw D3DX9Exception( "DeviceStateManager::applySamplerStateToBase", "IDirect3DDevice9::SetSamplerState", hr );
							}

							ppDw_samplerStateActualValues[ sampler_index ][ index ] = ppDw_samplerStateModificationValues[ sampler_index ][ index ];
						}

						pLBt_samplerStateModifiedIndexes[ sampler_index ].erase( pplBtIt_samplerStateModifiedIndexes[ sampler_index ][ index ] );
						pplBtIt_samplerStateModifiedIndexes[ sampler_index ][ index ] = pLBt_samplerStateModifiedIndexes[ sampler_index ].end();
					}
					else if ( ppDw_samplerStateActualValues[ sampler_index ][ index ] != ppDw_samplerStateBaseValues[ sampler_index ][ index ] )
					{
						if ( FAILED( hr = pD3D9_device->SetSamplerState( (DWORD) sampler_index, (D3DSAMPLERSTATETYPE) ( index + 1 ), ppDw_samplerStateBaseValues[ sampler_index ][ index ] ) ) )
						{
							throw D3DX9Exception( "DeviceStateManager::applySamplerStateToBase", "IDirect3DDevice9::SetSamplerState", hr );
						}

						ppDw_samplerStateActualValues[ sampler_index ][ index ] = ppDw_samplerStateBaseValues[ sampler_index ][ index ];
					}
				}

				lBt_samplerModifiedIndexes.erase( plBtIt_samplerModifiedIndexes[ sampler_index ] );
				plBtIt_samplerModifiedIndexes[ sampler_index ] = lBt_samplerModifiedIndexes.end();
			}
			else
			{
				for ( BYTE index = 0; index < SAMPLER_STATES_NUMBER; ++index )
				{
					if ( ppDw_samplerStateActualValues[ sampler_index ][ index ] != ppDw_samplerStateBaseValues[ sampler_index ][ index ] )
					{
						if ( FAILED( hr = pD3D9_device->SetSamplerState( (DWORD) sampler_index, (D3DSAMPLERSTATETYPE) ( index + 1 ), ppDw_samplerStateBaseValues[ sampler_index ][ index ] ) ) )
						{
							throw D3DX9Exception( "DeviceStateManager::applySamplerStateToBase", "IDirect3DDevice9::SetSamplerState", hr );
						}

						ppDw_samplerStateActualValues[ sampler_index ][ index ] = ppDw_samplerStateBaseValues[ sampler_index ][ index ];
					}
				}
			}
		}
	};


	void DeviceStateManager::resetRenderState()
	{
		while ( !lBt_renderStateModifiedIndexes.empty() )
		{
			plBtIt_renderStateModificationFlag[ lBt_renderStateModifiedIndexes.front() ] = lBt_renderStateModifiedIndexes.end();
			lBt_renderStateModifiedIndexes.pop_front();
		}

		HRESULT hr = S_OK;

		for ( BYTE i = 0; i < RENDER_STATES_NUMBER; ++i )
		{
			BYTE index = by_renderStateUsedIndexes[ i ];

			if ( dw_renderStateActualValues[ index ] != pDw_renderStateBaseValues[ index ] )
			{
				if ( FAILED( hr = pD3D9_device->SetRenderState( (D3DRENDERSTATETYPE) index, pDw_renderStateBaseValues[ index ] ) ) )
				{
					throw D3DX9Exception( "DeviceStateManager::resetRenderState", "IDirect3DDevice9::SetRenderState", hr );
				}

				dw_renderStateActualValues[ index ] = pDw_renderStateBaseValues[ index ];
			}
		}
	};


	void DeviceStateManager::resetSamplerState( DWORD _dw_sampler )
	{
		if ( plBtIt_samplerModifiedIndexes[ _dw_sampler ] != lBt_samplerModifiedIndexes.end() )
		{
			while ( !pLBt_samplerStateModifiedIndexes[ _dw_sampler ].empty() )
			{
				pplBtIt_samplerStateModifiedIndexes[ _dw_sampler ][ pLBt_samplerStateModifiedIndexes[ _dw_sampler ].front() ] = pLBt_samplerStateModifiedIndexes[ _dw_sampler ].end();
				pLBt_samplerStateModifiedIndexes[ _dw_sampler ].pop_front();
			}
		}

		HRESULT hr = S_OK;

		for ( BYTE i = 0; i < SAMPLER_STATES_NUMBER; ++i )
		{
			if ( ppDw_samplerStateActualValues[ _dw_sampler ][ i ] != ppDw_samplerStateBaseValues[ _dw_sampler ][ i ] )
			{
				if ( FAILED( hr = pD3D9_device->SetSamplerState( _dw_sampler, (D3DSAMPLERSTATETYPE) ( i + 1 ), ppDw_samplerStateBaseValues[ _dw_sampler ][ i ] ) ) )
				{
					throw D3DX9Exception( "DeviceStateManager::resetSamplerState", "IDirect3DDevice9::SetSamplerState", hr );
				}

				ppDw_samplerStateActualValues[ _dw_sampler ][ i ] = ppDw_samplerStateBaseValues[ _dw_sampler ][ i ];
			}
		}
	};


	void DeviceStateManager::resetSamplersState()
	{
		while ( !lBt_samplerModifiedIndexes.empty() )
		{
			BYTE sampler_index = lBt_samplerModifiedIndexes.front();

			while ( !pLBt_samplerStateModifiedIndexes[ sampler_index ].empty() )
			{
				pplBtIt_samplerStateModifiedIndexes[ sampler_index ][ pLBt_samplerStateModifiedIndexes[ sampler_index ].front() ] = pLBt_samplerStateModifiedIndexes[ sampler_index ].end();
				pLBt_samplerStateModifiedIndexes[ sampler_index ].pop_front();
			}

			plBtIt_samplerModifiedIndexes[ sampler_index ] = lBt_samplerModifiedIndexes.end();
			lBt_samplerModifiedIndexes.pop_front();
		}

		HRESULT hr = S_OK;

		for ( BYTE sampler_index = 0; sampler_index < SAMPLERS_NUMBER; ++sampler_index )
		{
			for ( BYTE state_index = 0; state_index < SAMPLERS_NUMBER; ++state_index )
			{
				if ( ppDw_samplerStateActualValues[ sampler_index ][ state_index ] != ppDw_samplerStateBaseValues[ sampler_index ][ state_index ] )
				{
					if ( FAILED( hr = pD3D9_device->SetSamplerState( (DWORD) sampler_index, (D3DSAMPLERSTATETYPE) ( state_index + 1 ), ppDw_samplerStateBaseValues[ sampler_index ][ state_index ] ) ) )
					{
						throw D3DX9Exception( "DeviceStateManager::resetSamplersState", "IDirect3DDevice9::SetSamplerState", hr );
					}

					ppDw_samplerStateActualValues[ sampler_index ][ state_index ] = ppDw_samplerStateBaseValues[ sampler_index ][ state_index ];
				}
			}
		}
	};


	void DeviceStateManager::clearQueue()
	{
		while ( !lBt_renderStateModifiedIndexes.empty() )
		{
			plBtIt_renderStateModificationFlag[ lBt_renderStateModifiedIndexes.front() ] = lBt_renderStateModifiedIndexes.end();
			lBt_renderStateModifiedIndexes.pop_front();
		}

		while ( !lBt_samplerModifiedIndexes.empty() )
		{
			BYTE sampler_index = lBt_samplerModifiedIndexes.front();

			while ( !pLBt_samplerStateModifiedIndexes[ sampler_index ].empty() )
			{
				pplBtIt_samplerStateModifiedIndexes[ sampler_index ][ pLBt_samplerStateModifiedIndexes[ sampler_index ].front() ] = pLBt_samplerStateModifiedIndexes[ sampler_index ].end();
				pLBt_samplerStateModifiedIndexes[ sampler_index ].pop_front();
			}

			plBtIt_samplerModifiedIndexes[ sampler_index ] = lBt_samplerModifiedIndexes.end();
			lBt_samplerModifiedIndexes.pop_front();
		}
	};


	void DeviceStateManager::applyRenderPipeState( IDirect3DVertexShader9* _pDxVS_vertexShader, IDirect3DPixelShader9* _pDxPS_pixelShader )
	{
		if ( pDxVS_vertexShaderActual != _pDxVS_vertexShader )
		{
			HRESULT hr = S_OK;

			if ( FAILED( hr = pD3D9_device->SetVertexShader( _pDxVS_vertexShader ) ) )
			{
				throw D3DX9Exception( "DeviceStateManager::applyRenderPipeState", "IDirect3DDevice9::SetVertexShader", hr );
			}

			if ( pDxVS_vertexShaderActual )		pDxVS_vertexShaderActual->Release();
			if ( _pDxVS_vertexShader )			_pDxVS_vertexShader->AddRef();
			pDxVS_vertexShaderActual = _pDxVS_vertexShader;
		}

		if ( pDxPS_pixelShaderActual != _pDxPS_pixelShader )
		{
			HRESULT hr = S_OK;

			if ( FAILED( hr = pD3D9_device->SetPixelShader( _pDxPS_pixelShader ) ) )
			{
				throw D3DX9Exception( "DeviceStateManager::applyRenderPipeState", "IDirect3DDevice9::SetPixelShader", hr );
			}

			if ( pDxPS_pixelShaderActual )		pDxPS_pixelShaderActual->Release();
			if ( _pDxPS_pixelShader )			_pDxPS_pixelShader->AddRef();
			pDxPS_pixelShaderActual = _pDxPS_pixelShader;
		}
	};


	void DeviceStateManager::applyRenderPipeVSState( IDirect3DVertexShader9* _pDxVS_vertexShader )
	{
		if ( pDxVS_vertexShaderActual != _pDxVS_vertexShader )
		{
			HRESULT hr = S_OK;

			if ( FAILED( hr = pD3D9_device->SetVertexShader( _pDxVS_vertexShader ) ) )
			{
				throw D3DX9Exception( "DeviceStateManager::applyRenderPipeVSState", "IDirect3DDevice9::SetVertexShader", hr );
			}

			if ( pDxVS_vertexShaderActual )		pDxVS_vertexShaderActual->Release();
			if ( _pDxVS_vertexShader )			_pDxVS_vertexShader->AddRef();
			pDxVS_vertexShaderActual = _pDxVS_vertexShader;
		}
	};


	void DeviceStateManager::applyRenderPipePSState( IDirect3DPixelShader9* _pDxPS_pixelShader )
	{
		if ( pDxPS_pixelShaderActual != _pDxPS_pixelShader )
		{
			HRESULT hr = S_OK;

			if ( FAILED( hr = pD3D9_device->SetPixelShader( _pDxPS_pixelShader ) ) )
			{
				throw D3DX9Exception( "DeviceStateManager::applyRenderPipePSState", "IDirect3DDevice9::SetPixelShader", hr );
			}

			if ( pDxPS_pixelShaderActual )		pDxPS_pixelShaderActual->Release();
			if ( _pDxPS_pixelShader )			_pDxPS_pixelShader->AddRef();
			pDxPS_pixelShaderActual = _pDxPS_pixelShader;
		}
	};


	void DeviceStateManager::applyTexture( DWORD _dw_sampler, IDirect3DBaseTexture9* _pDxTx_texture )
	{
		if ( pDxTx_samplerActualTexture[ _dw_sampler ] != _pDxTx_texture )
		{
			HRESULT hr = S_OK;

			if ( FAILED( hr = pD3D9_device->SetTexture( _dw_sampler, _pDxTx_texture ) ) )
			{
				throw D3DX9Exception( "DeviceStateManager::applyTexture", "IDirect3DDevice9::SetTexture", hr );
			}

			if ( pDxTx_samplerActualTexture[ _dw_sampler ] )	pDxTx_samplerActualTexture[ _dw_sampler ]->Release();
			if ( _pDxTx_texture )								_pDxTx_texture->AddRef();
			pDxTx_samplerActualTexture[ _dw_sampler ] = _pDxTx_texture;
		}
	};


	void DeviceStateManager::applyVertexShaderConstantFloat( const ShaderConstantDescription* _pSCD_constDesc, const void* _pV_value )
	{
		HRESULT hr = S_OK;

		if ( FAILED( hr = pD3D9_device->SetVertexShaderConstantF( _pSCD_constDesc->ui_startRegister, (const float*) _pV_value, _pSCD_constDesc->ui_lengthVec4f ) ) )
		{
			throw D3DX9Exception( "DeviceStateManager::applyVertexShaderConstantFloat", "IDirect3DDevice9::SetVertexShaderConstantF", hr );
		}
	};


	void DeviceStateManager::applyVertexShaderConstantInt( const ShaderConstantDescription* _pSCD_constDesc, const void* _pV_value )
	{
		HRESULT hr = S_OK;

		if ( FAILED( hr = pD3D9_device->SetVertexShaderConstantI( _pSCD_constDesc->ui_startRegister, (const int*) _pV_value, _pSCD_constDesc->ui_lengthVec4f ) ) )
		{
			throw D3DX9Exception( "DeviceStateManager::applyShaderConstantInt", "IDirect3DDevice9::SetVertexShaderConstantI", hr );
		}
	};


	void DeviceStateManager::applyVertexShaderConstantBool( const ShaderConstantDescription* _pSCD_constDesc, const void* _pV_value )
	{
		HRESULT hr = S_OK;

		if ( FAILED( hr = pD3D9_device->SetVertexShaderConstantB( _pSCD_constDesc->ui_startRegister, (const BOOL*) _pV_value, _pSCD_constDesc->ui_lengthDword ) ) )
		{
			throw D3DX9Exception( "DeviceStateManager::applyShaderConstantBool", "IDirect3DDevice9::SetVertexShaderConstantB", hr );
		}
	};


	void DeviceStateManager::applyPixelShaderConstantFloat( const ShaderConstantDescription* _pSCD_constDesc, const void* _pV_value )
	{
		HRESULT hr = S_OK;

		if ( FAILED( hr = pD3D9_device->SetPixelShaderConstantF( _pSCD_constDesc->ui_startRegister, (const float*) _pV_value, _pSCD_constDesc->ui_lengthVec4f ) ) )
		{
			throw D3DX9Exception( "DeviceStateManager::applyPixelShaderConstantFloat", "IDirect3DDevice9::SetPixelShaderConstantF", hr );
		}
	};


	void DeviceStateManager::applyPixelShaderConstantInt( const ShaderConstantDescription* _pSCD_constDesc, const void* _pV_value )
	{
		HRESULT hr = S_OK;

		if ( FAILED( hr = pD3D9_device->SetPixelShaderConstantI( _pSCD_constDesc->ui_startRegister, (const int*) _pV_value, _pSCD_constDesc->ui_lengthVec4f ) ) )
		{
			throw D3DX9Exception( "DeviceStateManager::applyPixelConstantInt", "IDirect3DDevice9::SetPixelShaderConstantI", hr );
		}
	};


	void DeviceStateManager::applyPixelShaderConstantBool( const ShaderConstantDescription* _pSCD_constDesc, const void* _pV_value )
	{
		HRESULT hr = S_OK;

		if ( FAILED( hr = pD3D9_device->SetPixelShaderConstantB( _pSCD_constDesc->ui_startRegister, (const BOOL*) _pV_value, _pSCD_constDesc->ui_lengthDword ) ) )
		{
			throw D3DX9Exception( "DeviceStateManager::applyPixelConstantBool", "IDirect3DDevice9::SetPixelShaderConstantB", hr );
		}
	};

/////////////////////////////////////////////////////////////////

	void DeviceStateManager::applyVertexShaderConstantFloat( const ShaderConstantDescription* _pSCD_constDesc, const void* _pV_value, UINT _ui_offset, UINT _ui_lengthVec4f )
	{
		HRESULT hr = S_OK;

		if ( FAILED( hr = pD3D9_device->SetVertexShaderConstantF( _pSCD_constDesc->ui_startRegister + _ui_offset, (const float*) _pV_value, ( _ui_lengthVec4f <= ( _pSCD_constDesc->ui_lengthVec4f - _ui_offset ) ? _ui_lengthVec4f : (_pSCD_constDesc->ui_lengthVec4f - _ui_offset ) ) ) ) )
		{
			throw D3DX9Exception( "DeviceStateManager::applyVertexShaderConstantFloat", "IDirect3DDevice9::SetVertexShaderConstantF", hr );
		}
	};


	void DeviceStateManager::applyVertexShaderConstantInt( const ShaderConstantDescription* _pSCD_constDesc, const void* _pV_value, UINT _ui_offset, UINT _ui_lengthVec4f )
	{
		HRESULT hr = S_OK;

		if ( FAILED( hr = pD3D9_device->SetVertexShaderConstantI( _pSCD_constDesc->ui_startRegister + _ui_offset, (const int*) _pV_value, ( _ui_lengthVec4f <= ( _pSCD_constDesc->ui_lengthVec4f - _ui_offset ) ? _ui_lengthVec4f : (_pSCD_constDesc->ui_lengthVec4f - _ui_offset ) ) ) ) )
		{
			throw D3DX9Exception( "DeviceStateManager::applyVertexShaderConstantInt", "IDirect3DDevice9::SetVertexShaderConstantI", hr );
		}
	};


	void DeviceStateManager::applyVertexShaderConstantBool( const ShaderConstantDescription* _pSCD_constDesc, const void* _pV_value, UINT _ui_offset, UINT _ui_lengthVec4f )
	{
		HRESULT hr = S_OK;

		if ( FAILED( hr = pD3D9_device->SetVertexShaderConstantB( _pSCD_constDesc->ui_startRegister + _ui_offset, (const BOOL*) _pV_value, ( _ui_lengthVec4f <= ( _pSCD_constDesc->ui_lengthDword - _ui_offset ) ? _ui_lengthVec4f : (_pSCD_constDesc->ui_lengthDword - _ui_offset ) ) ) ) )
		{
			throw D3DX9Exception( "DeviceStateManager::applyVertexShaderConstantBool", "IDirect3DDevice9::SetVertexShaderConstantB", hr );
		}
	};


	void DeviceStateManager::applyPixelShaderConstantFloat( const ShaderConstantDescription* _pSCD_constDesc, const void* _pV_value, UINT _ui_offset, UINT _ui_lengthVec4f )
	{
		HRESULT hr = S_OK;

		if ( FAILED( hr = pD3D9_device->SetPixelShaderConstantF( _pSCD_constDesc->ui_startRegister + _ui_offset, (const float*) _pV_value, ( _ui_lengthVec4f <= ( _pSCD_constDesc->ui_lengthVec4f - _ui_offset ) ? _ui_lengthVec4f : (_pSCD_constDesc->ui_lengthVec4f - _ui_offset ) ) ) ) )
		{
			throw D3DX9Exception( "DeviceStateManager::applyPixelShaderConstantFloat", "IDirect3DDevice9::SetPixelShaderConstantF", hr );
		}
	};


	void DeviceStateManager::applyPixelShaderConstantInt( const ShaderConstantDescription* _pSCD_constDesc, const void* _pV_value, UINT _ui_offset, UINT _ui_lengthVec4f )
	{
		HRESULT hr = S_OK;

		if ( FAILED( hr = pD3D9_device->SetPixelShaderConstantI( _pSCD_constDesc->ui_startRegister + _ui_offset, (const int*) _pV_value, ( _ui_lengthVec4f <= ( _pSCD_constDesc->ui_lengthVec4f - _ui_offset ) ? _ui_lengthVec4f : (_pSCD_constDesc->ui_lengthVec4f - _ui_offset ) ) ) ) )
		{
			throw D3DX9Exception( "DeviceStateManager::applyPixelShaderConstantInt", "IDirect3DDevice9::SetPixelShaderConstantI", hr );
		}
	};


	void DeviceStateManager::applyPixelShaderConstantBool( const ShaderConstantDescription* _pSCD_constDesc, const void* _pV_value, UINT _ui_offset, UINT _ui_lengthVec4f )
	{
		HRESULT hr = S_OK;

		if ( FAILED( hr = pD3D9_device->SetPixelShaderConstantB( _pSCD_constDesc->ui_startRegister + _ui_offset, (const BOOL*) _pV_value, ( _ui_lengthVec4f <= ( _pSCD_constDesc->ui_lengthDword - _ui_offset ) ? _ui_lengthVec4f : (_pSCD_constDesc->ui_lengthDword - _ui_offset ) ) ) ) )
		{
			throw D3DX9Exception( "DeviceStateManager::applyPixelShaderConstantBool", "IDirect3DDevice9::SetPixelShaderConstantB", hr );
		}
	};

//////////////////////////////////////////////////////////

	void DeviceStateManager::applyShaderConstantFloat( const ShaderConstantDescription* _pSCD_constDesc, const float* _pF_value )
	{
		HRESULT hr = S_OK;

		if ( FAILED( hr = ( pD3D9_device->* applySCMethodsF[ _pSCD_constDesc->bt_shaderType ] ) ( _pSCD_constDesc->ui_startRegister, _pF_value, _pSCD_constDesc->ui_lengthVec4f ) ) )
		{
			switch ( _pSCD_constDesc->bt_shaderType )
			{
				case SHADER_DESC_TYPE_PIXEL:
				{
					throw D3DX9Exception( "DeviceStateManager::applyShaderConstantFloat", "IDirect3DDevice9::SetPixelShaderConstantF", hr );
					break;
				}
				case SHADER_DESC_TYPE_VERTEX:
				default:
				{
					throw D3DX9Exception( "DeviceStateManager::applyShaderConstantFloat", "IDirect3DDevice9::SetVertexShaderConstantF", hr );
					break;
				}
			}
		}
	};


	void DeviceStateManager::applyShaderConstantFloat( const ShaderConstantDescription* _pSCD_constDesc, const float* _pF_value, UINT _ui_offset, UINT _ui_lengthVec4f )
	{
		HRESULT hr = S_OK;

		if ( FAILED( hr = ( pD3D9_device->* applySCMethodsF[ _pSCD_constDesc->bt_shaderType ] ) ( _pSCD_constDesc->ui_startRegister + _ui_offset, _pF_value, ( ( _ui_lengthVec4f && _ui_lengthVec4f >= _pSCD_constDesc->ui_lengthVec4f ) ? _ui_lengthVec4f : _pSCD_constDesc->ui_lengthVec4f ) ) ) )
		{
			switch ( _pSCD_constDesc->bt_shaderType )
			{
				case SHADER_DESC_TYPE_PIXEL:
				{
					throw D3DX9Exception( "DeviceStateManager::applyShaderConstantFloat", "IDirect3DDevice9::SetPixelShaderConstantF", hr );
					break;
				}
				case SHADER_DESC_TYPE_VERTEX:
				default:
				{
					throw D3DX9Exception( "DeviceStateManager::applyShaderConstantFloat", "IDirect3DDevice9::SetVertexShaderConstantF", hr );
					break;
				}
			}
		}
	};


	void DeviceStateManager::applyShaderConstantInt( const ShaderConstantDescription* _pSCD_constDesc, const int* _pI_value )
	{
		HRESULT hr = S_OK;

		if ( FAILED( hr = ( pD3D9_device->* applySCMethodsI[ _pSCD_constDesc->bt_shaderType ] ) ( _pSCD_constDesc->ui_startRegister, _pI_value, _pSCD_constDesc->ui_lengthVec4f ) ) )
		{
			switch ( _pSCD_constDesc->bt_shaderType )
			{
				case SHADER_DESC_TYPE_PIXEL:
				{
					throw D3DX9Exception( "DeviceStateManager::applyShaderConstantInt", "IDirect3DDevice9::SetPixelShaderConstantI", hr );
					break;
				}
				case SHADER_DESC_TYPE_VERTEX:
				default:
				{
					throw D3DX9Exception( "DeviceStateManager::applyShaderConstantInt", "IDirect3DDevice9::SetVertexShaderConstantI", hr );
					break;
				}
			}
		}
	};


	void DeviceStateManager::applyShaderConstantInt( const ShaderConstantDescription* _pSCD_constDesc, const int* _pI_value, UINT _ui_offset, UINT _ui_lengthVec4f )
	{
		HRESULT hr = S_OK;

		if ( FAILED( hr = ( pD3D9_device->* applySCMethodsI[ _pSCD_constDesc->bt_shaderType ] ) ( _pSCD_constDesc->ui_startRegister + _ui_offset, _pI_value, ( ( _ui_lengthVec4f && _ui_lengthVec4f >= _pSCD_constDesc->ui_lengthVec4f ) ? _ui_lengthVec4f : _pSCD_constDesc->ui_lengthVec4f ) ) ) )
		{
			switch ( _pSCD_constDesc->bt_shaderType )
			{
				case SHADER_DESC_TYPE_PIXEL:
				{
					throw D3DX9Exception( "DeviceStateManager::applyShaderConstantInt", "IDirect3DDevice9::SetPixelShaderConstantI", hr );
					break;
				}
				case SHADER_DESC_TYPE_VERTEX:
				default:
				{
					throw D3DX9Exception( "DeviceStateManager::applyShaderConstantInt", "IDirect3DDevice9::SetVertexShaderConstantI", hr );
					break;
				}
			}
		}
	};


	void DeviceStateManager::applyShaderConstantBool( const ShaderConstantDescription* _pSCD_constDesc, const BOOL* _pB_value )
	{
		HRESULT hr = S_OK;

		if ( FAILED( hr = ( pD3D9_device->* applySCMethodsB[ _pSCD_constDesc->bt_shaderType ] ) ( _pSCD_constDesc->ui_startRegister, _pB_value, _pSCD_constDesc->ui_lengthDword ) ) )
		{
			switch ( _pSCD_constDesc->bt_shaderType )
			{
				case SHADER_DESC_TYPE_PIXEL:
				{
					throw D3DX9Exception( "DeviceStateManager::applyShaderConstantBool", "IDirect3DDevice9::SetPixelShaderConstantB", hr );
					break;
				}
				case SHADER_DESC_TYPE_VERTEX:
				default:
				{
					throw D3DX9Exception( "DeviceStateManager::applyShaderConstantBool", "IDirect3DDevice9::SetVertexShaderConstantB", hr );
					break;
				}
			}
		}
	};


	void DeviceStateManager::applyVertexDeclaration( IDirect3DVertexDeclaration9* _pDxVD_vertexDeclaration )
	{
		if ( pDxVD_vertexDeclarationActual != _pDxVD_vertexDeclaration )
		{
			HRESULT hr = S_OK;

			if ( FAILED( hr = pD3D9_device->SetVertexDeclaration( _pDxVD_vertexDeclaration ) ) )
			{
				throw D3DX9Exception( "DeviceStateManager::applyVertexDeclaration", "IDirect3DDevice9::SetVertexDeclaration", hr );
			}

			if ( pDxVD_vertexDeclarationActual )	pDxVD_vertexDeclarationActual->Release();
			if ( _pDxVD_vertexDeclaration )			_pDxVD_vertexDeclaration->AddRef();
			pDxVD_vertexDeclarationActual = _pDxVD_vertexDeclaration;
		}
	};


	void DeviceStateManager::applyIndexData( IDirect3DIndexBuffer9* _pDxIB_indexData )
	{
		if ( pDxIB_indexDataActual != _pDxIB_indexData )
		{
			HRESULT hr = S_OK;

			if ( FAILED( hr = pD3D9_device->SetIndices( _pDxIB_indexData ) ) )
			{
				throw D3DX9Exception( "DeviceStateManager::applyIndexData", "IDirect3DDevice9::SetIndices", hr );
			}

			if ( pDxIB_indexDataActual )	pDxIB_indexDataActual->Release();
			if ( _pDxIB_indexData )			_pDxIB_indexData->AddRef();
			pDxIB_indexDataActual = _pDxIB_indexData;
		}
	};


	void DeviceStateManager::resetStreamData( UINT _ui_streamNumber )
	{
		if ( pSD_StreamsDefinitionActual[ _ui_streamNumber ].pDxVB_streamData )
		{
			HRESULT hr = S_OK;

			if ( FAILED( hr = pD3D9_device->SetStreamSource( _ui_streamNumber, NULL, 0, 0 ) ) )
			{
				throw D3DX9Exception( "DeviceStateManager::resetStreamData", "IDirect3DDevice9::SetStreamSource", hr );
			}

			pSD_StreamsDefinitionActual[ _ui_streamNumber ].pDxVB_streamData->Release();
			pSD_StreamsDefinitionActual[ _ui_streamNumber ].pDxVB_streamData = NULL;
			pSD_StreamsDefinitionActual[ _ui_streamNumber ].ui_stride = 0;
		}


		if ( pSD_StreamsDefinitionActual[ _ui_streamNumber ].ui_frequencySetValue != 1 )
		{
			HRESULT hr = S_OK;

			if ( FAILED( hr = pD3D9_device->SetStreamSourceFreq( _ui_streamNumber, 1 ) ) )
			{
				throw D3DX9Exception( "DeviceStateManager::resetStreamData", "IDirect3DDevice9::SetStreamSourceFreq", hr );
			}

			pSD_StreamsDefinitionActual[ _ui_streamNumber ].ui_frequencyParameter = 1;
			pSD_StreamsDefinitionActual[ _ui_streamNumber ].b_geometryIndexed = false;
			pSD_StreamsDefinitionActual[ _ui_streamNumber ].b_instanceData = false;
			pSD_StreamsDefinitionActual[ _ui_streamNumber ].ui_frequencySetValue = 1;
		}
	};


	void DeviceStateManager::applyStreamData( UINT _ui_streamNumber, IDirect3DVertexBuffer9* _pDxVB_streamData, UINT _ui_stride, UINT _ui_frequencyParameter, bool _b_geometryIndexed, bool _b_instanceData )
	{
		if ( pSD_StreamsDefinitionActual[ _ui_streamNumber ].pDxVB_streamData != _pDxVB_streamData )
		{
			HRESULT hr = S_OK;

			if ( FAILED( hr = pD3D9_device->SetStreamSource( _ui_streamNumber, _pDxVB_streamData, 0, _ui_stride ) ) )
			{
				throw D3DX9Exception( "DeviceStateManager::applyStreamData", "IDirect3DDevice9::SetStreamSource", hr );
			}

			if ( pSD_StreamsDefinitionActual[ _ui_streamNumber ].pDxVB_streamData )
				pSD_StreamsDefinitionActual[ _ui_streamNumber ].pDxVB_streamData->Release();
			if ( _pDxVB_streamData)		_pDxVB_streamData->AddRef();

			pSD_StreamsDefinitionActual[ _ui_streamNumber ].pDxVB_streamData = _pDxVB_streamData;
			pSD_StreamsDefinitionActual[ _ui_streamNumber ].ui_stride = _ui_stride;
		}

		UINT value = ( _b_geometryIndexed ? ( _b_instanceData ? D3DSTREAMSOURCE_INSTANCEDATA : D3DSTREAMSOURCE_INDEXEDDATA ) : 0 ) | _ui_frequencyParameter;

		if ( pSD_StreamsDefinitionActual[ _ui_streamNumber ].ui_frequencySetValue != value )
		{
			HRESULT hr = S_OK;

			if ( FAILED( hr = pD3D9_device->SetStreamSourceFreq( _ui_streamNumber, value ) ) )
			{
				throw D3DX9Exception( "DeviceStateManager::applyStreamData", "IDirect3DDevice9::SetStreamSourceFreq", hr );
			}

			pSD_StreamsDefinitionActual[ _ui_streamNumber ].ui_frequencyParameter = _ui_frequencyParameter;
			pSD_StreamsDefinitionActual[ _ui_streamNumber ].b_geometryIndexed = _b_geometryIndexed;
			pSD_StreamsDefinitionActual[ _ui_streamNumber ].b_instanceData = _b_instanceData;
			pSD_StreamsDefinitionActual[ _ui_streamNumber ].ui_frequencySetValue = value;
		}
	};


	void DeviceStateManager::applyGeometry( IDirect3DVertexDeclaration9* _pDxVD_vertexDeclaration, UINT _ui_streamsNumber, IDirect3DVertexBuffer9** _pDxVB_streamData, UINT* _ui_streamStride )
	{
		if ( pDxVD_vertexDeclarationActual != _pDxVD_vertexDeclaration )
		{
			HRESULT hr = S_OK;

			if ( FAILED( hr = pD3D9_device->SetVertexDeclaration( _pDxVD_vertexDeclaration ) ) )
			{
				throw D3DX9Exception( "DeviceStateManager::applyVertexDeclaration", "IDirect3DDevice9::SetVertexDeclaration", hr );
			}

			if ( pDxVD_vertexDeclarationActual )	pDxVD_vertexDeclarationActual->Release();
			if ( _pDxVD_vertexDeclaration )			_pDxVD_vertexDeclaration->AddRef();
			pDxVD_vertexDeclarationActual = _pDxVD_vertexDeclaration;
		}


		UINT maxIndex = min<UINT>( STREAMS_NUMBER, _ui_streamsNumber );
		UINT i = 0;

		while ( i < maxIndex )
		{
			if ( pSD_StreamsDefinitionActual[ i ].pDxVB_streamData != _pDxVB_streamData[ i ] )
			{
				HRESULT hr = S_OK;

				if ( FAILED( hr = pD3D9_device->SetStreamSource( i, _pDxVB_streamData[ i ], 0, _ui_streamStride[ i ] ) ) )
				{
					throw D3DX9Exception( "DeviceStateManager::applyStreamData", "IDirect3DDevice9::SetStreamSource", hr );
				}

				if ( pSD_StreamsDefinitionActual[ i ].pDxVB_streamData )	pSD_StreamsDefinitionActual[ i ].pDxVB_streamData->Release();
				if ( _pDxVB_streamData[ i ] )								_pDxVB_streamData[ i ]->AddRef();

				pSD_StreamsDefinitionActual[ i ].pDxVB_streamData = _pDxVB_streamData[ i ];
				pSD_StreamsDefinitionActual[ i ].ui_stride = _ui_streamStride[ i ];
			}

			if ( pSD_StreamsDefinitionActual[ i ].ui_frequencySetValue != 1 )
			{
				HRESULT hr = S_OK;

				if ( FAILED( hr = pD3D9_device->SetStreamSourceFreq( i, 1 ) ) )
				{
					throw D3DX9Exception( "DeviceStateManager::applyStreamData", "IDirect3DDevice9::SetStreamSourceFreq", hr );
				}

				pSD_StreamsDefinitionActual[ i ].ui_frequencyParameter = 1;
				pSD_StreamsDefinitionActual[ i ].b_geometryIndexed = false;
				pSD_StreamsDefinitionActual[ i ].b_instanceData = false;
				pSD_StreamsDefinitionActual[ i ].ui_frequencySetValue = 1;
			}

			++i;
		}


		while ( i < STREAMS_NUMBER )
		{
			if ( pSD_StreamsDefinitionActual[ i ].pDxVB_streamData )
			{
				HRESULT hr = S_OK;

				if ( FAILED( hr = pD3D9_device->SetStreamSource( i, NULL, 0, 0 ) ) )
				{
					throw D3DX9Exception( "DeviceStateManager::applyStreamData", "IDirect3DDevice9::SetStreamSource", hr );
				}

				pSD_StreamsDefinitionActual[ i ].pDxVB_streamData->Release();
				pSD_StreamsDefinitionActual[ i ].pDxVB_streamData = NULL;
				pSD_StreamsDefinitionActual[ i ].ui_stride = 0;

				if ( pSD_StreamsDefinitionActual[ i ].ui_frequencySetValue != 1 )
				{
					if ( FAILED( hr = pD3D9_device->SetStreamSourceFreq( i, 1 ) ) )
					{
						throw D3DX9Exception( "DeviceStateManager::applyStreamData", "IDirect3DDevice9::SetStreamSourceFreq", hr );
					}

					pSD_StreamsDefinitionActual[ i ].ui_frequencyParameter = 1;
					pSD_StreamsDefinitionActual[ i ].b_geometryIndexed = false;
					pSD_StreamsDefinitionActual[ i ].b_instanceData = false;
					pSD_StreamsDefinitionActual[ i ].ui_frequencySetValue = 1;
				}
			}

			++i;
		}
	};


	void DeviceStateManager::applyIndexedGeometry( IDirect3DVertexDeclaration9* _pDxVD_vertexDeclaration, IDirect3DIndexBuffer9* _pDxIB_indexData, UINT _ui_streamsNumber, IDirect3DVertexBuffer9** _pDxVB_streamData, UINT* _ui_streamStride )
	{
		if ( pDxVD_vertexDeclarationActual != _pDxVD_vertexDeclaration )
		{
			HRESULT hr = S_OK;

			if ( FAILED( hr = pD3D9_device->SetVertexDeclaration( _pDxVD_vertexDeclaration ) ) )
			{
				throw D3DX9Exception( "DeviceStateManager::applyVertexDeclaration", "IDirect3DDevice9::SetVertexDeclaration", hr );
			}

			if ( pDxVD_vertexDeclarationActual )	pDxVD_vertexDeclarationActual->Release();
			if ( _pDxVD_vertexDeclaration )			_pDxVD_vertexDeclaration->AddRef();
			pDxVD_vertexDeclarationActual = _pDxVD_vertexDeclaration;
		}


		if ( pDxIB_indexDataActual != _pDxIB_indexData )
		{
			HRESULT hr = S_OK;

			if ( FAILED( hr = pD3D9_device->SetIndices( _pDxIB_indexData ) ) )
			{
				throw D3DX9Exception( "DeviceStateManager::applyIndexData", "IDirect3DDevice9::SetIndices", hr );
			}

			if ( pDxIB_indexDataActual )	pDxIB_indexDataActual->Release();
			if ( _pDxIB_indexData )			_pDxIB_indexData->AddRef();
			pDxIB_indexDataActual = _pDxIB_indexData;
		}


		UINT maxIndex = min<UINT>( STREAMS_NUMBER, _ui_streamsNumber );
		UINT i = 0;

		while ( i < maxIndex )
		{
			if ( pSD_StreamsDefinitionActual[ i ].pDxVB_streamData != _pDxVB_streamData[ i ] )
			{
				HRESULT hr = S_OK;

				if ( FAILED( hr = pD3D9_device->SetStreamSource( i, _pDxVB_streamData[ i ], 0, _ui_streamStride[ i ] ) ) )
				{
					throw D3DX9Exception( "DeviceStateManager::applyStreamData", "IDirect3DDevice9::SetStreamSource", hr );
				}

				if ( pSD_StreamsDefinitionActual[ i ].pDxVB_streamData )	pSD_StreamsDefinitionActual[ i ].pDxVB_streamData->Release();
				if ( _pDxVB_streamData[ i ] )								_pDxVB_streamData[ i ]->AddRef();

				pSD_StreamsDefinitionActual[ i ].pDxVB_streamData = _pDxVB_streamData[ i ];
				pSD_StreamsDefinitionActual[ i ].ui_stride = _ui_streamStride[ i ];
			}

			if ( pSD_StreamsDefinitionActual[ i ].ui_frequencySetValue != 1 )
			{
				HRESULT hr = S_OK;

				if ( FAILED( hr = pD3D9_device->SetStreamSourceFreq( i, 1 ) ) )
				{
					throw D3DX9Exception( "DeviceStateManager::applyStreamData", "IDirect3DDevice9::SetStreamSourceFreq", hr );
				}

				pSD_StreamsDefinitionActual[ i ].ui_frequencyParameter = 1;
				pSD_StreamsDefinitionActual[ i ].b_geometryIndexed = false;
				pSD_StreamsDefinitionActual[ i ].b_instanceData = false;
				pSD_StreamsDefinitionActual[ i ].ui_frequencySetValue = 1;
			}

			++i;
		}


		while ( i < STREAMS_NUMBER )
		{
			if ( pSD_StreamsDefinitionActual[ i ].pDxVB_streamData )
			{
				HRESULT hr = S_OK;

				if ( FAILED( hr = pD3D9_device->SetStreamSource( i, NULL, 0, 0 ) ) )
				{
					throw D3DX9Exception( "DeviceStateManager::applyStreamData", "IDirect3DDevice9::SetStreamSource", hr );
				}

				pSD_StreamsDefinitionActual[ i ].pDxVB_streamData->Release();
				pSD_StreamsDefinitionActual[ i ].pDxVB_streamData = NULL;
				pSD_StreamsDefinitionActual[ i ].ui_stride = 0;

				if ( pSD_StreamsDefinitionActual[ i ].ui_frequencySetValue != 1 )
				{
					if ( FAILED( hr = pD3D9_device->SetStreamSourceFreq( i, 1 ) ) )
					{
						throw D3DX9Exception( "DeviceStateManager::applyStreamData", "IDirect3DDevice9::SetStreamSourceFreq", hr );
					}

					pSD_StreamsDefinitionActual[ i ].ui_frequencyParameter = 1;
					pSD_StreamsDefinitionActual[ i ].b_geometryIndexed = false;
					pSD_StreamsDefinitionActual[ i ].b_instanceData = false;
					pSD_StreamsDefinitionActual[ i ].ui_frequencySetValue = 1;
				}
			}

			++i;
		}
	};


	void DeviceStateManager::applyInstancedGeometry( IDirect3DVertexDeclaration9* _pDxVD_vertexDeclaration, UINT _ui_instancesNumber, UINT _ui_streamsNumber, IDirect3DVertexBuffer9** _pDxVB_streamData, UINT* _ui_streamStride, bool* _b_geometryData )
	{
		if ( pDxVD_vertexDeclarationActual != _pDxVD_vertexDeclaration )
		{
			HRESULT hr = S_OK;

			if ( FAILED( hr = pD3D9_device->SetVertexDeclaration( _pDxVD_vertexDeclaration ) ) )
			{
				throw D3DX9Exception( "DeviceStateManager::applyVertexDeclaration", "IDirect3DDevice9::SetVertexDeclaration", hr );
			}

			if ( pDxVD_vertexDeclarationActual )	pDxVD_vertexDeclarationActual->Release();
			if ( _pDxVD_vertexDeclaration )			_pDxVD_vertexDeclaration->AddRef();
			pDxVD_vertexDeclarationActual = _pDxVD_vertexDeclaration;
		}


		UINT maxIndex = min<UINT>( STREAMS_NUMBER, _ui_streamsNumber );
		UINT i = 0;

		while ( i < maxIndex )
		{
			if ( pSD_StreamsDefinitionActual[ i ].pDxVB_streamData != _pDxVB_streamData[ i ] )
			{
				HRESULT hr = S_OK;

				if ( FAILED( hr = pD3D9_device->SetStreamSource( i, _pDxVB_streamData[ i ], 0, _ui_streamStride[ i ] ) ) )
				{
					throw D3DX9Exception( "DeviceStateManager::applyStreamData", "IDirect3DDevice9::SetStreamSource", hr );
				}

				if ( pSD_StreamsDefinitionActual[ i ].pDxVB_streamData )	pSD_StreamsDefinitionActual[ i ].pDxVB_streamData->Release();
				if ( _pDxVB_streamData[ i ] )								_pDxVB_streamData[ i ]->AddRef();

				pSD_StreamsDefinitionActual[ i ].pDxVB_streamData = _pDxVB_streamData[ i ];
				pSD_StreamsDefinitionActual[ i ].ui_stride = _ui_streamStride[ i ];
			}

			if ( _b_geometryData[ i ] )
			{
				if ( pSD_StreamsDefinitionActual[ i ].ui_frequencySetValue != _ui_instancesNumber )
				{
					HRESULT hr = S_OK;

					if ( FAILED( hr = pD3D9_device->SetStreamSourceFreq( i, _ui_instancesNumber ) ) )
					{
						throw D3DX9Exception( "DeviceStateManager::applyStreamData", "IDirect3DDevice9::SetStreamSourceFreq", hr );
					}

					pSD_StreamsDefinitionActual[ i ].ui_frequencyParameter = _ui_instancesNumber;
					pSD_StreamsDefinitionActual[ i ].b_geometryIndexed = false;
					pSD_StreamsDefinitionActual[ i ].b_instanceData = false;
					pSD_StreamsDefinitionActual[ i ].ui_frequencySetValue = _ui_instancesNumber;
				}
			}
			else
			{
				if ( pSD_StreamsDefinitionActual[ i ].ui_frequencySetValue != 1 )
				{
					HRESULT hr = S_OK;

					if ( FAILED( hr = pD3D9_device->SetStreamSourceFreq( i, 1 ) ) )
					{
						throw D3DX9Exception( "DeviceStateManager::applyStreamData", "IDirect3DDevice9::SetStreamSourceFreq", hr );
					}

					pSD_StreamsDefinitionActual[ i ].ui_frequencyParameter = 1;
					pSD_StreamsDefinitionActual[ i ].b_geometryIndexed = false;
					pSD_StreamsDefinitionActual[ i ].b_instanceData = true;
					pSD_StreamsDefinitionActual[ i ].ui_frequencySetValue = 1;
				}
			}

			++i;
		}


		while ( i < STREAMS_NUMBER )
		{
			if ( pSD_StreamsDefinitionActual[ i ].pDxVB_streamData )
			{
				HRESULT hr = S_OK;

				if ( FAILED( hr = pD3D9_device->SetStreamSource( i, NULL, 0, 0 ) ) )
				{
					throw D3DX9Exception( "DeviceStateManager::applyStreamData", "IDirect3DDevice9::SetStreamSource", hr );
				}

				pSD_StreamsDefinitionActual[ i ].pDxVB_streamData->Release();
				pSD_StreamsDefinitionActual[ i ].pDxVB_streamData = NULL;
				pSD_StreamsDefinitionActual[ i ].ui_stride = 0;

				if ( pSD_StreamsDefinitionActual[ i ].ui_frequencySetValue != 1 )
				{
					if ( FAILED( hr = pD3D9_device->SetStreamSourceFreq( i, 1 ) ) )
					{
						throw D3DX9Exception( "DeviceStateManager::applyStreamData", "IDirect3DDevice9::SetStreamSourceFreq", hr );
					}

					pSD_StreamsDefinitionActual[ i ].ui_frequencyParameter = 1;
					pSD_StreamsDefinitionActual[ i ].b_geometryIndexed = false;
					pSD_StreamsDefinitionActual[ i ].b_instanceData = false;
					pSD_StreamsDefinitionActual[ i ].ui_frequencySetValue = 1;
				}
			}

			++i;
		}
	};


	void DeviceStateManager::applyInstancedIndexedGeometry( IDirect3DVertexDeclaration9* _pDxVD_vertexDeclaration, UINT _ui_instancesNumber, IDirect3DIndexBuffer9* _pDxIB_indexData, UINT _ui_streamsNumber, IDirect3DVertexBuffer9** _pDxVB_streamData, UINT* _ui_streamStride, bool* _b_geometryData )
	{
		if ( pDxVD_vertexDeclarationActual != _pDxVD_vertexDeclaration )
		{
			HRESULT hr = S_OK;

			if ( FAILED( hr = pD3D9_device->SetVertexDeclaration( _pDxVD_vertexDeclaration ) ) )
			{
				throw D3DX9Exception( "DeviceStateManager::applyVertexDeclaration", "IDirect3DDevice9::SetVertexDeclaration", hr );
			}

			if ( pDxVD_vertexDeclarationActual )	pDxVD_vertexDeclarationActual->Release();
			if ( _pDxVD_vertexDeclaration )			_pDxVD_vertexDeclaration->AddRef();
			pDxVD_vertexDeclarationActual = _pDxVD_vertexDeclaration;
		}


		if ( pDxIB_indexDataActual != _pDxIB_indexData )
		{
			HRESULT hr = S_OK;

			if ( FAILED( hr = pD3D9_device->SetIndices( _pDxIB_indexData ) ) )
			{
				throw D3DX9Exception( "DeviceStateManager::applyIndexData", "IDirect3DDevice9::SetIndices", hr );
			}

			if ( pDxIB_indexDataActual )	pDxIB_indexDataActual->Release();
			if ( _pDxIB_indexData )			_pDxIB_indexData->AddRef();
			pDxIB_indexDataActual = _pDxIB_indexData;
		}


		UINT maxIndex = min<UINT>( STREAMS_NUMBER, _ui_streamsNumber );
		UINT i = 0;

		while ( i < maxIndex )
		{
			if ( pSD_StreamsDefinitionActual[ i ].pDxVB_streamData != _pDxVB_streamData[ i ] )
			{
				HRESULT hr = S_OK;

				if ( FAILED( hr = pD3D9_device->SetStreamSource( i, _pDxVB_streamData[ i ], 0, _ui_streamStride[ i ] ) ) )
				{
					throw D3DX9Exception( "DeviceStateManager::applyStreamData", "IDirect3DDevice9::SetStreamSource", hr );
				}

				if ( pSD_StreamsDefinitionActual[ i ].pDxVB_streamData )	pSD_StreamsDefinitionActual[ i ].pDxVB_streamData->Release();
				if ( _pDxVB_streamData[ i ] )								_pDxVB_streamData[ i ]->AddRef();

				pSD_StreamsDefinitionActual[ i ].pDxVB_streamData = _pDxVB_streamData[ i ];
				pSD_StreamsDefinitionActual[ i ].ui_stride = _ui_streamStride[ i ];
			}

			if ( _b_geometryData[ i ] )
			{
				UINT value = D3DSTREAMSOURCE_INDEXEDDATA | _ui_instancesNumber;

				if ( pSD_StreamsDefinitionActual[ i ].ui_frequencySetValue != value )
				{
					HRESULT hr = S_OK;

					if ( FAILED( hr = pD3D9_device->SetStreamSourceFreq( i, value ) ) )
					{
						throw D3DX9Exception( "DeviceStateManager::applyStreamData", "IDirect3DDevice9::SetStreamSourceFreq", hr );
					}

					pSD_StreamsDefinitionActual[ i ].ui_frequencyParameter = _ui_instancesNumber;
					pSD_StreamsDefinitionActual[ i ].b_geometryIndexed = true;
					pSD_StreamsDefinitionActual[ i ].b_instanceData = false;
					pSD_StreamsDefinitionActual[ i ].ui_frequencySetValue = value;
				}
			}
			else
			{
				UINT value = D3DSTREAMSOURCE_INSTANCEDATA | 1;

				if ( pSD_StreamsDefinitionActual[ i ].ui_frequencySetValue != value )
				{
					HRESULT hr = S_OK;

					if ( FAILED( hr = pD3D9_device->SetStreamSourceFreq( i, value ) ) )
					{
						throw D3DX9Exception( "DeviceStateManager::applyStreamData", "IDirect3DDevice9::SetStreamSourceFreq", hr );
					}

					pSD_StreamsDefinitionActual[ i ].ui_frequencyParameter = 1;
					pSD_StreamsDefinitionActual[ i ].b_geometryIndexed = true;
					pSD_StreamsDefinitionActual[ i ].b_instanceData = true;
					pSD_StreamsDefinitionActual[ i ].ui_frequencySetValue = 1;
				}
			}

			++i;
		}


		while ( i < STREAMS_NUMBER )
		{
			if ( pSD_StreamsDefinitionActual[ i ].pDxVB_streamData )
			{
				HRESULT hr = S_OK;

				if ( FAILED( hr = pD3D9_device->SetStreamSource( i, NULL, 0, 0 ) ) )
				{
					throw D3DX9Exception( "DeviceStateManager::applyStreamData", "IDirect3DDevice9::SetStreamSource", hr );
				}

				pSD_StreamsDefinitionActual[ i ].pDxVB_streamData->Release();
				pSD_StreamsDefinitionActual[ i ].pDxVB_streamData = NULL;
				pSD_StreamsDefinitionActual[ i ].ui_stride = 0;

				if ( pSD_StreamsDefinitionActual[ i ].ui_frequencySetValue != 1 )
				{
					if ( FAILED( hr = pD3D9_device->SetStreamSourceFreq( i, 1 ) ) )
					{
						throw D3DX9Exception( "DeviceStateManager::applyStreamData", "IDirect3DDevice9::SetStreamSourceFreq", hr );
					}

					pSD_StreamsDefinitionActual[ i ].ui_frequencyParameter = 1;
					pSD_StreamsDefinitionActual[ i ].b_geometryIndexed = false;
					pSD_StreamsDefinitionActual[ i ].b_instanceData = false;
					pSD_StreamsDefinitionActual[ i ].ui_frequencySetValue = 1;
				}
			}

			++i;
		}
	};


	void DeviceStateManager::purgeRenderPipeState()
	{
		HRESULT hr = S_OK;

		for ( DWORD i = 0; i < SAMPLERS_NUMBER; ++i )
		{
			if ( pDxTx_samplerActualTexture[ i ] )
			{
				if ( FAILED( hr = pD3D9_device->SetTexture( i, NULL ) ) )
				{
					throw D3DX9Exception( "DeviceStateManager::purgeRenderPipeState", "IDirect3DDevice9::SetTexture", hr );
				}

				pDxTx_samplerActualTexture[ i ]->Release();
				pDxTx_samplerActualTexture[ i ] = NULL;
			}
		}


		if ( pDxVS_vertexShaderActual )
		{
			if ( FAILED( hr = pD3D9_device->SetVertexShader( NULL ) ) )
			{
				throw D3DX9Exception( "DeviceStateManager::purgeRenderPipeState", "IDirect3DDevice9::SetVertexShader", hr );
			}

			pDxVS_vertexShaderActual->Release();
			pDxVS_vertexShaderActual = NULL;
		}


		if ( pDxPS_pixelShaderActual )
		{
			if ( FAILED( hr = pD3D9_device->SetPixelShader( NULL ) ) )
			{
				throw D3DX9Exception( "DeviceStateManager::purgeRenderPipeState", "IDirect3DDevice9::SetPixelShader", hr );
			}

			pDxPS_pixelShaderActual->Release();
			pDxPS_pixelShaderActual = NULL;
		}


		if ( pDxVD_vertexDeclarationActual )
		{
			if ( FAILED( hr = pD3D9_device->SetVertexDeclaration( NULL ) ) )
			{
				throw D3DX9Exception( "DeviceStateManager::purgeRenderPipeState", "IDirect3DDevice9::SetVertexDeclaration", hr );
			}

			pDxVD_vertexDeclarationActual->Release();
			pDxVD_vertexDeclarationActual = NULL;
		}



		if ( pDxIB_indexDataActual )
		{
			if ( FAILED( hr = pD3D9_device->SetIndices( NULL ) ) )
			{
				throw D3DX9Exception( "DeviceStateManager::purgeRenderPipeState", "IDirect3DDevice9::SetIndices", hr );
			}

			pDxIB_indexDataActual->Release();
			pDxIB_indexDataActual = NULL;
		}


		UINT i = 0;

		while ( i < STREAMS_NUMBER )
		{
			if ( pSD_StreamsDefinitionActual[ i ].pDxVB_streamData )
			{
				if ( FAILED( hr = pD3D9_device->SetStreamSource( i, NULL, 0, 0 ) ) )
				{
					throw D3DX9Exception( "DeviceStateManager::applyStreamData", "IDirect3DDevice9::SetStreamSource", hr );
				}

				pSD_StreamsDefinitionActual[ i ].pDxVB_streamData->Release();
				pSD_StreamsDefinitionActual[ i ].pDxVB_streamData = NULL;
				pSD_StreamsDefinitionActual[ i ].ui_stride = 0;
			}

			if ( pSD_StreamsDefinitionActual[ i ].ui_frequencySetValue != 1 )
			{
				if ( FAILED( hr = pD3D9_device->SetStreamSourceFreq( i, 1 ) ) )
				{
					throw D3DX9Exception( "DeviceStateManager::applyStreamData", "IDirect3DDevice9::SetStreamSourceFreq", hr );
				}

				pSD_StreamsDefinitionActual[ i ].ui_frequencyParameter = 1;
				pSD_StreamsDefinitionActual[ i ].b_geometryIndexed = false;
				pSD_StreamsDefinitionActual[ i ].b_instanceData = false;
				pSD_StreamsDefinitionActual[ i ].ui_frequencySetValue = 1;
			}
		}
	};


}