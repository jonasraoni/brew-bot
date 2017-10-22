/*
 * BrewBot: API tester for devices using the platform Qualcomm BREW.
 * Jonas Raoni Soares da Silva <http://raoni.org>
 * https://github.com/jonasraoni/brew-bot
 */

#include "AEEAppGen.h"
#include "api-test.c"
#include "bot.bid"
#include "memory-block.c"
#include "AEEFile.h"

typedef struct {
	AEEApplet brewData;
	IFileMgr *fileSystem;
	IDisplay *display;
	IShell *shell;
	IModule *module;
	IFile *logFile;
} Bot;

static boolean botEventHandler(Bot *data, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static void initBot(Bot *bot);
static void freeBot(Bot *bot);
void alert(IDisplay *display, const char *const s);

//////////////////////////////////////////////////

static void initBot(Bot *bot){
	bot->display = bot->brewData.m_pIDisplay;
	bot->module = bot->brewData.m_pIModule;
	bot->shell = bot->brewData.m_pIShell;
}

static void freeBot(Bot *bot){ }

int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *pMod, void **ppObj){
	*ppObj = NULL;
	if(AEEApplet_New(sizeof(Bot), ClsId, pIShell, pMod, (IApplet **)ppObj, (AEEHANDLER)botEventHandler, (PFNFREEAPPDATA)freeBot)){
		initBot((Bot *)*ppObj);
		return AEE_SUCCESS;
	}
	return EFAILED;
}

static boolean botEventHandler(Bot *bot, AEEEvent eCode, uint16 wParam, uint32 dwParam){  
	switch (eCode){
		case EVT_APP_START: {
			AEEApplet *app = &bot->brewData;
			MemoryBlock map = getFunctions();
			TTestFunction **curFunction = (TTestFunction **)map->data;
			//TODO: Test only the used apis and resume on fail
			while(map->used--){
				if(!(*curFunction)->callback(app)) {
					alert(bot->display, (*curFunction)->name);
					return 1;
				}
				alert(bot->display, (*curFunction)->name);
				FREE(*curFunction);
				++curFunction;
			}
			freeMemoryBlock(map);
			return TRUE;
		}	
		case EVT_APP_STOP:
			return TRUE;
		default:
			break;
	}
	return FALSE;
}

void alert(IDisplay *display, const char *const s){
	AECHAR *buffer = (AECHAR *)MALLOC(2048 * sizeof(AECHAR));
	UTF8TOWSTR(s, STRLEN(s) + 1, buffer, 2048 * sizeof(AECHAR));
	IDISPLAY_ClearScreen(display);
	IDISPLAY_DrawText(display, AEE_FONT_NORMAL, buffer, -1, 0, 0, NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update(display);
}
