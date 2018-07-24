
// fills array of 1024 elements with cursor image

int fillCursorArrayWhite(UINT* _cursorArrow)
{
	if (!_cursorArrow)
		return -1;

	memset(_cursorArrow, 0x00000000, 4 * 1024);

	_cursorArrow[0] = _cursorArrow[32] = _cursorArrow[33] = _cursorArrow[64] = _cursorArrow[66] = _cursorArrow[96] = _cursorArrow[99] = 
	_cursorArrow[128] = _cursorArrow[132] = _cursorArrow[160] = _cursorArrow[165] = _cursorArrow[192] = _cursorArrow[198] = _cursorArrow[224] = 
	_cursorArrow[231] = _cursorArrow[256] = _cursorArrow[264] = _cursorArrow[288] = _cursorArrow[297] = _cursorArrow[320] = _cursorArrow[330] = 
	_cursorArrow[352] = _cursorArrow[359] = _cursorArrow[360] = _cursorArrow[361] = _cursorArrow[362] = _cursorArrow[363] = _cursorArrow[384] = 
	_cursorArrow[388] = _cursorArrow[391] = _cursorArrow[416] = _cursorArrow[419] = _cursorArrow[420] = _cursorArrow[423] = _cursorArrow[448] = 
	_cursorArrow[450] = _cursorArrow[453] = _cursorArrow[456] = _cursorArrow[480] = _cursorArrow[481] = _cursorArrow[485] = _cursorArrow[488] = 
	_cursorArrow[512] = _cursorArrow[518] = _cursorArrow[521] = _cursorArrow[550] = _cursorArrow[553] = _cursorArrow[583] = _cursorArrow[586] = 
	_cursorArrow[615] = _cursorArrow[618] = _cursorArrow[648] = _cursorArrow[649] = 0xFF000000;

	_cursorArrow[65] = _cursorArrow[97] = _cursorArrow[98] = _cursorArrow[129] = _cursorArrow[130] = _cursorArrow[131] = _cursorArrow[161] = 
	_cursorArrow[162] = _cursorArrow[163] = _cursorArrow[164] = _cursorArrow[193] = _cursorArrow[194] = _cursorArrow[195] = _cursorArrow[196] = 
	_cursorArrow[197] = _cursorArrow[225] = _cursorArrow[226] = _cursorArrow[227] = _cursorArrow[228] = _cursorArrow[229] = _cursorArrow[230] = 
	_cursorArrow[257] = _cursorArrow[258] = _cursorArrow[259] = _cursorArrow[260] = _cursorArrow[261] = _cursorArrow[262] = _cursorArrow[263] = 
	_cursorArrow[289] = _cursorArrow[290] = _cursorArrow[291] = _cursorArrow[292] = _cursorArrow[293] = _cursorArrow[294] = _cursorArrow[295] = 
	_cursorArrow[296] = _cursorArrow[321] = _cursorArrow[322] = _cursorArrow[323] = _cursorArrow[324] = _cursorArrow[325] = _cursorArrow[326] = 
	_cursorArrow[327] = _cursorArrow[328] = _cursorArrow[329] = _cursorArrow[353] = _cursorArrow[354] = _cursorArrow[355] = _cursorArrow[356] = 
	_cursorArrow[357] = _cursorArrow[358] = _cursorArrow[385] = _cursorArrow[386] = _cursorArrow[387] = _cursorArrow[389] = _cursorArrow[390] = 
	_cursorArrow[417] = _cursorArrow[418] = _cursorArrow[421] = _cursorArrow[422] = _cursorArrow[449] = _cursorArrow[454] = _cursorArrow[455] = 
	_cursorArrow[486] = _cursorArrow[487] = _cursorArrow[519] = _cursorArrow[520] = _cursorArrow[551] = _cursorArrow[552] = _cursorArrow[584] = 
	_cursorArrow[585] = _cursorArrow[616] = _cursorArrow[617] = 0xFFFFFFFF;


	return 0;
};


int fillCursorArrayBlack(UINT* _cursorArrow)
{
	if (!_cursorArrow)
		return -1;

	memset(_cursorArrow, 0x00000000, 4 * 1024);

	_cursorArrow[65] = _cursorArrow[97] = _cursorArrow[98] = _cursorArrow[129] = _cursorArrow[130] = _cursorArrow[131] = _cursorArrow[161] = 
	_cursorArrow[162] = _cursorArrow[163] = _cursorArrow[164] = _cursorArrow[193] = _cursorArrow[194] = _cursorArrow[195] = _cursorArrow[196] = 
	_cursorArrow[197] = _cursorArrow[225] = _cursorArrow[226] = _cursorArrow[227] = _cursorArrow[228] = _cursorArrow[229] = _cursorArrow[230] = 
	_cursorArrow[257] = _cursorArrow[258] = _cursorArrow[259] = _cursorArrow[260] = _cursorArrow[261] = _cursorArrow[262] = _cursorArrow[263] = 
	_cursorArrow[289] = _cursorArrow[290] = _cursorArrow[291] = _cursorArrow[292] = _cursorArrow[293] = _cursorArrow[294] = _cursorArrow[295] = 
	_cursorArrow[296] = _cursorArrow[321] = _cursorArrow[322] = _cursorArrow[323] = _cursorArrow[324] = _cursorArrow[325] = _cursorArrow[326] = 
	_cursorArrow[327] = _cursorArrow[328] = _cursorArrow[329] = _cursorArrow[353] = _cursorArrow[354] = _cursorArrow[355] = _cursorArrow[356] = 
	_cursorArrow[357] = _cursorArrow[358] = _cursorArrow[385] = _cursorArrow[386] = _cursorArrow[387] = _cursorArrow[389] = _cursorArrow[390] = 
	_cursorArrow[417] = _cursorArrow[418] = _cursorArrow[421] = _cursorArrow[422] = _cursorArrow[449] = _cursorArrow[454] = _cursorArrow[455] = 
	_cursorArrow[486] = _cursorArrow[487] = _cursorArrow[519] = _cursorArrow[520] = _cursorArrow[551] = _cursorArrow[552] = _cursorArrow[584] = 
	_cursorArrow[585] = _cursorArrow[616] = _cursorArrow[617] = 0xFF000000;

	_cursorArrow[0] = _cursorArrow[32] = _cursorArrow[33] = _cursorArrow[64] = _cursorArrow[66] = _cursorArrow[96] = _cursorArrow[99] = 
	_cursorArrow[128] = _cursorArrow[132] = _cursorArrow[160] = _cursorArrow[165] = _cursorArrow[192] = _cursorArrow[198] = _cursorArrow[224] = 
	_cursorArrow[231] = _cursorArrow[256] = _cursorArrow[264] = _cursorArrow[288] = _cursorArrow[297] = _cursorArrow[320] = _cursorArrow[330] = 
	_cursorArrow[352] = _cursorArrow[359] = _cursorArrow[360] = _cursorArrow[361] = _cursorArrow[362] = _cursorArrow[363] = _cursorArrow[384] = 
	_cursorArrow[388] = _cursorArrow[391] = _cursorArrow[416] = _cursorArrow[419] = _cursorArrow[420] = _cursorArrow[423] = _cursorArrow[448] = 
	_cursorArrow[450] = _cursorArrow[453] = _cursorArrow[456] = _cursorArrow[480] = _cursorArrow[481] = _cursorArrow[485] = _cursorArrow[488] = 
	_cursorArrow[512] = _cursorArrow[518] = _cursorArrow[521] = _cursorArrow[550] = _cursorArrow[553] = _cursorArrow[583] = _cursorArrow[586] = 
	_cursorArrow[615] = _cursorArrow[618] = _cursorArrow[648] = _cursorArrow[649] = 0xFFFFFFFF;


	return 0;
};


int fillCursorArrayGrey(UINT* _cursorArrow)
{
	if (!_cursorArrow)
		return -1;

	memset(_cursorArrow, 0x00000000, 4 * 1024);

	_cursorArrow[0] = _cursorArrow[33] = _cursorArrow[66] = _cursorArrow[99] = _cursorArrow[132] = _cursorArrow[165] = _cursorArrow[198] = 
	_cursorArrow[231] = _cursorArrow[264] = _cursorArrow[297] = _cursorArrow[330] = _cursorArrow[357] = _cursorArrow[358] = _cursorArrow[359] = 
	_cursorArrow[360] = _cursorArrow[361] = _cursorArrow[362] = _cursorArrow[363] = _cursorArrow[388] = _cursorArrow[391] = _cursorArrow[419] = 
	_cursorArrow[423] = _cursorArrow[450] = _cursorArrow[456] = _cursorArrow[481] = _cursorArrow[488] = _cursorArrow[512] = _cursorArrow[521] = 
	_cursorArrow[553] = _cursorArrow[586] = _cursorArrow[618] = _cursorArrow[648] = _cursorArrow[649] = 0xFF000000;

	_cursorArrow[32] = _cursorArrow[64] = _cursorArrow[96] = _cursorArrow[128] = _cursorArrow[160] = _cursorArrow[192] = _cursorArrow[224] = 
	_cursorArrow[256] = _cursorArrow[288] = _cursorArrow[320] = _cursorArrow[352] = _cursorArrow[384] = _cursorArrow[416] = _cursorArrow[420] = 
	_cursorArrow[448] = _cursorArrow[453] = _cursorArrow[480] = _cursorArrow[485] = _cursorArrow[518] = _cursorArrow[550] = _cursorArrow[583] = 
	_cursorArrow[615] = 0xFF808080;

	_cursorArrow[65] = _cursorArrow[97] = _cursorArrow[98] = _cursorArrow[130] = _cursorArrow[131] = _cursorArrow[162] = _cursorArrow[163] = 
	_cursorArrow[164] = _cursorArrow[195] = _cursorArrow[196] = _cursorArrow[197] = _cursorArrow[227] = _cursorArrow[228] = _cursorArrow[229] = 
	_cursorArrow[230] = _cursorArrow[260] = _cursorArrow[261] = _cursorArrow[262] = _cursorArrow[263] = _cursorArrow[292] = _cursorArrow[293] = 
	_cursorArrow[294] = _cursorArrow[295] = _cursorArrow[296] = _cursorArrow[325] = _cursorArrow[326] = _cursorArrow[327] = _cursorArrow[328] = 
	_cursorArrow[329] = _cursorArrow[356] = _cursorArrow[387] = _cursorArrow[390] = _cursorArrow[418] = _cursorArrow[422] = _cursorArrow[449] = 
	_cursorArrow[455] = _cursorArrow[487] = _cursorArrow[520] = _cursorArrow[552] = _cursorArrow[585] = _cursorArrow[616] = _cursorArrow[617] = 
	0xFFC0C0C0;

	_cursorArrow[129] = _cursorArrow[161] = _cursorArrow[193] = _cursorArrow[194] = _cursorArrow[225] = _cursorArrow[226] = _cursorArrow[257] = 
	_cursorArrow[258] = _cursorArrow[259] = _cursorArrow[289] = _cursorArrow[290] = _cursorArrow[291] = _cursorArrow[321] = _cursorArrow[322] = 
	_cursorArrow[323] = _cursorArrow[324] = _cursorArrow[353] = _cursorArrow[354] = _cursorArrow[355] = _cursorArrow[385] = _cursorArrow[386] = 
	_cursorArrow[389] = _cursorArrow[417] = _cursorArrow[421] = _cursorArrow[454] = _cursorArrow[486] = _cursorArrow[519] = _cursorArrow[551] = 
	_cursorArrow[584] = 0xFFFFFFFF;


	return 0;
};
