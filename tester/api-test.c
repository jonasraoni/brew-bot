/*
 * BrewBot: API tester for devices using the platform Qualcomm BREW.
 * Jonas Raoni Soares da Silva <http://raoni.org>
 * https://github.com/jonasraoni/brew-bot
 */

#include "AEEAppGen.h"
#include "AEEStdLib.h"
#include "AEELicense.h"
#include "AEEFile.h"
#include "AEEGraphics.h"

#include "memory-block.c"

//-- functions to test the API ----------------------------------------------
typedef struct {
	const char *name;
	int ( *callback )( AEEApplet *app );
} TTestFunction;

#define FUNCTION( name ) int test##name ( AEEApplet *app )


void playerCallback( void *data, AEESoundPlayerCmd eCBType, AEESoundPlayerStatus eSPStatus, uint32 dwParam ){ }
void timerCallback( void* data ){ }


FUNCTION( ISHELL_CloseApplet ){
	return ISHELL_CloseApplet( app->m_pIShell, FALSE ) == SUCCESS;
}

FUNCTION( STRNCPY ){
	char s[6];
	return STRNCMP( STRNCPY( s, "abc\0d", 5 ), "abc\03", 5 );
}

FUNCTION( MEMSET ){
	char s[10];
	return !STRNCMP( MEMSET( s, '#', 10 ), "##########", 10 );
}

FUNCTION( MEMCPY ){
	char s[6];
	return STRNCMP( MEMCPY( s, "abc\0d", 5 ), "abc\03", 5 );
}

FUNCTION( SPRINTF ){
	char s[12];
	return SPRINTF( s, "%s:%s", "texte", "texto" ) == 11 && !STRCMP( s, "texte:texto" );
}

FUNCTION( STRNCMP ){
	return STRNCMP( "JoNaS123555", "JoNaS124", 8 ) < 0 && STRNCMP( "JoNaS123", "JoNaS1215", 8 ) > 0 && !STRNCMP( "JoNaS1234321", "JoNaS1234321", 8 );
}

FUNCTION( STRLEN ){
	return STRLEN( "Jonas\n\0Jonas" ) == 6;
}

FUNCTION( STRCMP ){
	return STRNCMP( "JoNaS123555", "JoNaS124", 8 ) < 0 && STRNCMP( "JoNaS123", "JoNaS1215", 8 ) > 0 && !STRNCMP( "JoNaS1234321", "JoNaS1234321", 8 );
}

FUNCTION( MALLOC ){
	void *ptr;
	if( ptr = MALLOC( 32 ) )
		FREE( ptr );
	return ptr != NULL;
}

FUNCTION( FREE ){
	void *ptr;
	uint32 size;

	if( ptr = MALLOC( 256 ) ){
		size = GETRAMFREE( NULL, NULL );
		FREE( ptr );
		return GETRAMFREE( NULL, NULL ) - size >= 256;
	}
	return 0;
}

FUNCTION( ISHELL_GetUpTimeMS ){
	return ISHELL_GetUpTimeMS( app->m_pIShell );
}

FUNCTION( ISHELL_GetDeviceInfo ){
	AEEDeviceInfo *deviceInfo = (AEEDeviceInfo*)MALLOC( sizeof( AEEDeviceInfo ) );
	ISHELL_GetDeviceInfo( app->m_pIShell, deviceInfo );
	FREE( deviceInfo );
	return 1;
}

FUNCTION( IDISPLAY_UpdateEx ){
	IDISPLAY_UpdateEx( app->m_pIDisplay, 0 );
	return 1;
}

FUNCTION( ISHELL_SetTimer ){
	return ISHELL_SetTimer( app->m_pIShell, 10, timerCallback, app->m_pIDisplay ) == SUCCESS;
}

FUNCTION( ISHELL_CancelTimer ){
	return ISHELL_CancelTimer( app->m_pIShell, timerCallback, NULL ) == SUCCESS;
}

FUNCTION( IDISPLAY_DrawHLine ){
	IDISPLAY_DrawHLine( app->m_pIDisplay, 0, 0, 10 );
	return 1;
}

FUNCTION( IDISPLAY_DrawVLine ){
	IDISPLAY_DrawVLine( app->m_pIDisplay, 0, 0, 10 );
	return 1;
}

FUNCTION( IDISPLAY_DrawRect ){
	IDISPLAY_DrawRect( app->m_pIDisplay, NULL, 0, 0, IDF_RECT_FILL );
	return 1;
}

FUNCTION( IDISPLAY_FillRect ){
	AEERect *rect = (AEERect*)MALLOC( sizeof( AEERect ) );
	IDISPLAY_FillRect( app->m_pIDisplay, rect, 0 );
	FREE( rect );
	return 1;
}

FUNCTION( IDISPLAY_InvertRect ){
	AEERect *rect = (AEERect*)MALLOC( sizeof( AEERect ) );
	IDISPLAY_InvertRect( app->m_pIDisplay, rect );
	FREE( rect );
	return 1;
}

FUNCTION( WSTRTOSTR ){
	AECHAR s[] = { '@','!','#','J','o','n','a','s','#','$','%','\0' };
	char *s2 = (char*)MALLOC( 32 * sizeof( char ) );
	WSTRTOSTR( s, s2, 32 * sizeof( char ) );
	FREE( s2 );
	return 1;
}

FUNCTION( IDISPLAY_DrawText ){
 	AECHAR s[] = { 'J','o','n','a','s','\0' };
	return IDISPLAY_DrawText( app->m_pIDisplay, AEE_FONT_NORMAL, s, -1, 0, 0, NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE ) == SUCCESS;
}

FUNCTION( IDISPLAY_MeasureText ){
 	AECHAR s[] = { 'J','o','n','a','s','\0' };
	return IDISPLAY_MeasureText( app->m_pIDisplay, AEE_FONT_NORMAL, s );
}

FUNCTION( ISHELL_CreateInstance_IFileMgr ){
	IFileMgr *fileSystem = NULL;
	int result = ISHELL_CreateInstance( app->m_pIShell, AEECLSID_FILEMGR, (void**)&fileSystem ) == SUCCESS;
	if( result )
		IFILEMGR_Release( fileSystem );
	return result;
}

FUNCTION( IFILEMGR_OpenFile ){
	IFileMgr *fileSystem = NULL;
	IFile *f = NULL;
	int result = ISHELL_CreateInstance( app->m_pIShell, AEECLSID_FILEMGR, (void**)&fileSystem ) == SUCCESS;
	if( result ) {
		if( result = NULL != ( f = IFILEMGR_OpenFile( fileSystem, "test.txt", IFILEMGR_Test( fileSystem, "test.txt" ) == SUCCESS ? _OFM_READWRITE : _OFM_CREATE ) ) )
			IFILE_Release( f );
		IFILEMGR_Release( fileSystem );
	}
	return result;
}

FUNCTION( IFILEMGR_OpenFile ){
	IFILEMGR_Release
}

MemoryBlock getFunctions( void ){

	#define ADDFUNCTION( apiName ) \
			tf = *((TTestFunction**)functions->data + functions->used++) = (TTestFunction*)MALLOC( sizeof( TTestFunction ) ); \
			tf->name = #apiName; \
			tf->callback = test##apiName; \
			incMemoryBlock( functions );


	TTestFunction *tf;
	MemoryBlock functions = getMemoryBlock( sizeof( TTestFunction* ), 40 );

	ADDFUNCTION( IFILEMGR_OpenFile )
	ADDFUNCTION( ISHELL_CreateInstance_IFileMgr )
	ADDFUNCTION( MALLOC )
	ADDFUNCTION( FREE )
	ADDFUNCTION( STRCMP )
	ADDFUNCTION( STRLEN )
	ADDFUNCTION( STRNCMP )
	ADDFUNCTION( SPRINTF )
	ADDFUNCTION( MEMCPY )
	ADDFUNCTION( MEMSET )
	ADDFUNCTION( STRNCPY )
	ADDFUNCTION( IDISPLAY_MeasureText )
	ADDFUNCTION( IDISPLAY_DrawText )
	ADDFUNCTION( WSTRTOSTR )
	ADDFUNCTION( IDISPLAY_InvertRect )
	ADDFUNCTION( IDISPLAY_FillRect )
	ADDFUNCTION( IDISPLAY_DrawRect )
	ADDFUNCTION( IDISPLAY_DrawVLine )
	ADDFUNCTION( IDISPLAY_DrawHLine )
	ADDFUNCTION( ISHELL_SetTimer )
	ADDFUNCTION( ISHELL_CancelTimer )
	ADDFUNCTION( IDISPLAY_UpdateEx )
	ADDFUNCTION( ISHELL_GetDeviceInfo )
	ADDFUNCTION( ISHELL_GetUpTimeMS )
	ADDFUNCTION( ISHELL_CloseApplet )

	return functions;
}
