char *s = (char*)MALLOC(2048);
uint32 free;
AEERect rect = {0};
void *ptr = NULL, *bmpImage = NULL;
AECHAR wideS[1024] = {0};
IFileMgr *fileSystem = NULL;
AEEDeviceInfo deviceInfo = {0};
FileInfo fileInfo = {0};
IFile *f = NULL, *test = NULL;
IGraphics *graphics = NULL;
ILicense *license = NULL;
ISound *sound = NULL;
ISoundPlayer *player = NULL;
AEEImageInfo imageInfo = {0};
boolean imageReallocated;

IFILEMGR_Release(fileSystem);
LogTest(f, "IFILEMGR_Release", 1);

ISHELL_CreateInstance(app->m_pIShell, AEECLSID_FILEMGR, (void**)&fileSystem);

test = IFILEMGR_OpenFile(fileSystem, "test.txt", IFILEMGR_Test(fileSystem, "test.txt") == SUCCESS ? _OFM_READWRITE : _OFM_CREATE);
LogTest(f, "IFILE_Write", IFILE_Write(test, "AB", 2) == 2);
LogTest(f, "IFILE_Seek", IFILE_Seek(test, _SEEK_START, 0) == SUCCESS);
LogTest(f, "IFILE_Read", IFILE_Read(test, s, 2) == 2);
LogTest(f, "IFILE_Release", !IFILE_Release(test));
IFILEMGR_Remove(fileSystem, "test.txt");

if(LogTest(f, "ISHELL_CreateInstance \"AEECLSID_LICENSE\"", ISHELL_CreateInstance(app->m_pIShell, AEECLSID_LICENSE, (void**)&license) == SUCCESS)) {
	AEELicenseType type; 
	uint32 timeout, sequence;

	switch (LogTest(f, "ILICENSE_GetPurchaseInfo", ILICENSE_GetPurchaseInfo(license, &type, &timeout, &sequence)+1) -1){
		case PT_NONE: Log(f, "\tForma de aquisição: Nenhuma"); break;
		case PT_DEMO: Log(f, "\tForma de aquisição: Demo"); break;
		case PT_PURCHASE: Log(f, "\tForma de aquisição: Compra"); break;
		case PT_SUBSCRIPTION: Log(f, "\tForma de aquisição: Assinatura"); break;
		case PT_UPGRADE: Log(f, "\tForma de aquisição: Atualização"); break;
	}

	LogS(f, "\tTipo de licença: ");
	switch(type){
		case LT_NONE: Log(f, "Não expira"); break;
		case LT_USES: Log(f, "Quantidade de uso"); break;
		case LT_DATE: Log(f, "Data"); break;
		case LT_DAYS: Log(f, "Dias"); break;
		case LT_MINUTES_OF_USE: Log(f, "Minutos"); break;
	}

	SPRINTF(s, "\tExpira em: %lu"NL"\tNúmero de sequência: %lu", timeout, sequence);
	Log(f, s);

	LogTest(f, "ILICENSE_Release", !ILICENSE_Release(license));

}

if(LogTest(f, "ISHELL_CreateInstance \"AEECLSID_SOUNDPLAYER\"", ISHELL_CreateInstance(app->m_pIShell, AEECLSID_SOUNDPLAYER, (void**)&player) == SUCCESS)) {

	AEESoundPlayerInfo playerInfo = {0};

	playerInfo.eInput = SDT_FILE;
	playerInfo.pData = "test.mid";

	LogTest(f, "ISOUNDPLAYER_SetInfo", ISOUNDPLAYER_SetInfo(player, &playerInfo) == AEE_SUCCESS);

	MEMSET(&playerInfo, 0, sizeof(playerInfo));

	LogTest(f, "ISOUNDPLAYER_GetInfo", ISOUNDPLAYER_GetInfo(player, &playerInfo) == AEE_SUCCESS && !STRCMP((char*)playerInfo.pData, "test.mp3"));
	
	ISOUNDPLAYER_RegisterNotify(player, playerCallback, app);
	LogTest(f, "ISOUNDPLAYER_RegisterNotify", 1);

	ISOUNDPLAYER_Play(player);
	LogTest(f, "ISOUNDPLAYER_Play", 1);

	ISOUNDPLAYER_Stop(player);
	LogTest(f, "ISOUNDPLAYER_Stop", 1);

	LogTest(f, "ISOUNDPLAYER_Release", !ISOUNDPLAYER_Release(player));

}


if(LogTest(f, "ISHELL_CreateInstance \"AEECLSID_SOUND\"", ISHELL_CreateInstance(app->m_pIShell, AEECLSID_SOUND, (void**)&sound) == SUCCESS)) {

	ISOUND_Vibrate(sound, 1000);
	LogTest(f, "ISOUND_Vibrate", 1);

	ISOUND_StopVibrate(sound);
	LogTest(f, "ISOUND_StopVibrate", 1);

	LogTest(f, "ISOUND_Release", !ISOUND_Release(sound));
}


if(LogTest(f, "ISHELL_CreateInstance \"AEECLSID_GRAPHICS\"", ISHELL_CreateInstance(app->m_pIShell, AEECLSID_GRAPHICS, (void**)&graphics) == SUCCESS)) {
	AEEArc arc = {0};
	AEEEllipse ellipse = {0};
	AEELine line = {0};
	AEETriangle triangle = {0};


	arc.arcAngle = 30;
	arc.r = 30;

	LogTest(f, "IGRAPHICS_DrawTriangle", IGRAPHICS_DrawTriangle(graphics, &triangle) == SUCCESS);
	LogTest(f, "IGRAPHICS_DrawLine", IGRAPHICS_DrawLine(graphics, &line) == SUCCESS);
	LogTest(f, "IGRAPHICS_DrawEllipse", IGRAPHICS_DrawEllipse(graphics, &ellipse) == SUCCESS);
	LogTest(f, "IGRAPHICS_DrawArc", IGRAPHICS_DrawArc(graphics, &arc) == SUCCESS);
	LogTest(f, "IGRAPHICS_SetClip", IGRAPHICS_SetClip(graphics, NULL, AEE_GRAPHICS_FRAME));
	LogTest(f, "IGRAPHICS_SetColor", IGRAPHICS_SetColor(graphics, 4, 5, 6, 100));
	LogTest(f, "IGRAPHICS_SetFillColor", IGRAPHICS_SetFillColor(graphics, 4, 5, 6, 100));
	LogTest(f, "IGRAPHICS_SetFillMode", IGRAPHICS_SetFillMode(graphics, 1));
	
	LogTest(f, "IGRAPHICS_Release", !IGRAPHICS_Release(graphics));
}


test = IFILEMGR_OpenFile(fileSystem, "test.bmp", _OFM_READ);

if(LogTest(f, "IFILE_GetInfo", IFILE_GetInfo(test, &fileInfo) == SUCCESS)){
	SPRINTF(s, "\tTamanho da imagem: %lu bytes", fileInfo.dwSize);
	Log(f, s);
}

ptr = MALLOC(fileInfo.dwSize);
IFILE_Read(test, ptr, fileInfo.dwSize);
IFILE_Release(test);
//ptr = ISHELL_LoadResData(app->m_pIShell, RES_RES_FILE, IMAGE, RESTYPE_IMAGE);
if(LogTest(f, "CONVERTBMP", (bmpImage = CONVERTBMP(ptr, &imageInfo, &imageReallocated)) != NULL)){
	IDISPLAY_BitBlt(app->m_pIDisplay, 0, 0, deviceInfo.cxScreen, deviceInfo.cyScreen, bmpImage, 0, 0, AEE_RO_OR);
	SPRINTF(s, "\tDimensões da imagem: %lux%lu", imageInfo.cx, imageInfo.cy);
	Log(f, s);
	LogTest(f, "IDISPLAY_BitBlt", 1);
	if(imageReallocated){
		SYSFREE(bmpImage);
		LogTest(f, "SYSFREE", 1);
	}

}
//ISHELL_FreeResData(app->m_pIShell, ptr);
FREE(ptr);


if(LogTest(f, "IFILEMGR_GetLastError", IFILEMGR_GetLastError(fileSystem) || 1)){
	SPRINTF(s, "\tResultado da última operação: %d", IFILEMGR_GetLastError(fileSystem));
	Log(f, s);
}			


IFILE_Release(f);
IFILEMGR_Release(fileSystem);
