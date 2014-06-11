//native LoadPNG(name[]); /* LoadPNG - Return Index | name[] = Name of the .png in Scriptfiles Folder */
//native FreePNG(index);	/* FreePNG - Return 1 - Success, 0 - IMG Not Found | index = The Return of LoadPNG */
//native ReadPixelInfo(index, x, y, &r, &g, &b); /* ReadPixelInfo - Return 1 - Success, 0 - IMG Not Found | index = The Return of LoadPNG | INFO: Reads out the R,G,B Values of given Pixel Coordinates. */
//native ReadImageInfo(index, &w, &h); /* ReadImageInfo - Return 1 - Success, 0 - IMG Not Found | index = The Return of LoadPNG */
//native RefreshPNG(index); /* RefreshPNG - Return 1 - Success, 0 - IMG Not Found | index = The Return of LoadPNG | INFO: It does infact cache the old Pixel Values, so while it's loading the Image you can still read the old info so it's a seamless transaction. */
//forward OnPNGLoad(index, bool:success);

#include "main.h"
logprintf_t logprintf;
void **ppPluginData;
extern void *pAMXFunctions;


pthread_t workthread;
short iTerminationTicks = 0;
CPNGManager* pngManger = new CPNGManager;
std::vector<SLOT> loadqueue;
std::list<AMX *> amx_list;

void *ProcessPNGS(void *threadid)
{
	int amx_idx = 0;
	int amxerr = 0;
	while ( true ) 
	{

		for(std::vector<SLOT>::size_type iIterator = 0; iIterator != loadqueue.size(); iIterator++) 
		{
			if ( loadqueue[iIterator] != -1 )
			{

				CPNG *png = pngManger->GetPNG(loadqueue[iIterator]);

				if ( png ) 
				{

					if ( png->GetRefreshing() ) // we're in refresh mode (almost the same as load but anyways)
					{
						bool bSuccess = png->Refresh();
						AMX *amx = png->ref;
						if ( amx ) 
						{
							amxerr = amx_FindPublic(amx, "OnPNGLoad", &amx_idx);
							if (amxerr == AMX_ERR_NONE)
							{
								amx_Push(amx, static_cast<cell>(bSuccess));
								amx_Push(amx, static_cast<cell>(loadqueue[iIterator]));
								amx_Exec(amx, NULL, amx_idx); 
							}
						}
						if ( bSuccess )
							png->SetRefresh(false);
						else
							pngManger->FreePNG(loadqueue[iIterator]); // well, bye.
					}
					else
					{
						bool bSuccess = png->Load();
						AMX *amx = png->ref;
						if ( amx ) 
						{
							amxerr = amx_FindPublic(amx, "OnPNGLoad", &amx_idx);
							if (amxerr == AMX_ERR_NONE)
							{
								amx_Push(amx, static_cast<cell>(bSuccess));
								amx_Push(amx, static_cast<cell>(loadqueue[iIterator]));
								amx_Exec(amx, NULL, amx_idx); 
							}
						}
						if ( !bSuccess )
							pngManger->FreePNG(loadqueue[iIterator]); // well, bye.
					}
				}

				loadqueue[iIterator] = -1;
			}
		}

		iTerminationTicks ++;

		if ( iTerminationTicks >= 25 )
			break;

		SLP ( 500 );
	}

	iTerminationTicks = 0;
	pthread_exit(NULL);
	return NULL;
}

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports() 
{
	return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES;
}

char szScriptFilesFLD [ MAX_PATH ];

PLUGIN_EXPORT bool PLUGIN_CALL Load( void **ppData ) 
{
	pAMXFunctions = ppData[PLUGIN_DATA_AMX_EXPORTS];
	logprintf = (logprintf_t)ppData[PLUGIN_DATA_LOGPRINTF];
	logprintf("MPNG by MyU loaded.");

	#ifdef WIN32
		GetCurrentDirectory(MAX_PATH, szScriptFilesFLD);
		strcat(szScriptFilesFLD, "\\scriptfiles\\");
	#else
		getcwd(szScriptFilesFLD, MAX_PATH);
		strcat(szScriptFilesFLD, "scriptfiles/");
	#endif

	return true;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload()
{
	logprintf("Send Bullet by MyU unloaded.");
}

SLOT CreatePNG(const char *szPath)
{
	SLOT iSlot = pngManger->Create(szPath);
	loadqueue.push_back(iSlot);
	iTerminationTicks = 0;

	if(pthread_kill(workthread, 0) != 0)
	{
		pthread_create(&workthread, NULL, ProcessPNGS, (void*)NULL);
	}
	return iSlot;
}

void RefreshPNG(SLOT iSlot)
{
	loadqueue.push_back(iSlot);
	iTerminationTicks = 0;

	if(pthread_kill(workthread, 0) != 0)
	{
		pthread_create(&workthread, NULL, ProcessPNGS, (void*)NULL);
	}
}

// native LoadPNG(index);
cell AMX_NATIVE_CALL LoadPNG(AMX* amx, cell* params) 
{
	CHECK_PARAMS(1, "LoadPNG");
		
	int iTextLen;
	cell *cell_text;
	amx_GetAddr(amx, params[1], &cell_text); amx_StrLen(cell_text, &iTextLen);

	if ( iTextLen ) {
		iTextLen++;
		char* szName = new char[ iTextLen ];
		amx_GetString(szName, cell_text, 0, iTextLen);

		char *szPath = new char [ iTextLen + MAX_PATH ];
		memset(szPath, 0, iTextLen + MAX_PATH);
		strncpy(szPath, szScriptFilesFLD, MAX_PATH);
		strncpy(szPath, szName, MAX_PATH);

		SLOT iSlot = CreatePNG(szPath);
		pngManger->GetPNG(iSlot)->ref = amx; // for callback
		SAFE_DELETE(szPath);

		return iSlot;
	} 

	return -1;
}

// native FreePNG(index);
cell AMX_NATIVE_CALL FreePNG(AMX* amx, cell* params) 
{
	CHECK_PARAMS(1, "FreePNG");
		
	SLOT iSlot = static_cast<SLOT>(params[1]);

	if ( iSlot < 0 || iSlot > HARDCODED_MAX_PNGS )
		return 0;

	CPNG *png = pngManger->GetPNG(iSlot);

	if ( png == NULL )
		return 0;

	if ( png->GetImage() == NULL )
		return 0;

	pngManger->FreePNG(iSlot);

	return 1;
}

// native RefreshPNG(index);
cell AMX_NATIVE_CALL RefreshPNG(AMX* amx, cell* params) 
{
	CHECK_PARAMS(1, "RefreshPNG");
		
	SLOT iSlot = static_cast<SLOT>(params[1]);

	if ( iSlot < 0 || iSlot > HARDCODED_MAX_PNGS )
		return 0;

	CPNG *png = pngManger->GetPNG(iSlot);

	if ( png == NULL )
		return 0;

	if ( png->GetImage() == NULL )
		return 0;

	RefreshPNG(iSlot);

	return 1;
}

// native ReadPixelInfo(index, x, y, &r, &g, &b);
cell AMX_NATIVE_CALL ReadPixelInfo(AMX* amx, cell* params) 
{
	CHECK_PARAMS(4, "ReadPixelInfo");
		
	SLOT iSlot = static_cast<SLOT>(params[1]);

	if ( iSlot < 0 || iSlot > HARDCODED_MAX_PNGS )
		return 0;

	CPNG *png = pngManger->GetPNG(iSlot);

	if ( png == NULL )
		return 0;

	if ( png->GetImage() == NULL )
		return 0;

	cell* cell_addrs[3] = {NULL, NULL, NULL};
    
    amx_GetAddr(amx, params[4], &cell_addrs[0]);
	amx_GetAddr(amx, params[5], &cell_addrs[1]);
	amx_GetAddr(amx, params[6], &cell_addrs[2]);


	if ( cell_addrs[0] == NULL || cell_addrs[1] == NULL || cell_addrs[1] == NULL )
		return 0;

	png::rgb_pixel pixel = png->GetImage()->get_pixel(static_cast<int>(params[2]), static_cast<int>(params[3]));

	*cell_addrs[0] = static_cast<cell>(pixel.red);
	*cell_addrs[1] = static_cast<cell>(pixel.green);
	*cell_addrs[2] = static_cast<cell>(pixel.blue);

	return 1;
}

// native ReadImageInfo(index, &w, &h);
cell AMX_NATIVE_CALL ReadImageInfo(AMX* amx, cell* params) 
{
	CHECK_PARAMS(3, "ReadImageInfo");
		
	SLOT iSlot = static_cast<SLOT>(params[1]);

	if ( iSlot < 0 || iSlot > HARDCODED_MAX_PNGS )
		return 0;

	CPNG *png = pngManger->GetPNG(iSlot);

	if ( png == NULL )
		return 0;

	if ( png->GetImage() == NULL )
		return 0;

	cell* cell_addrs[2] = {NULL, NULL};
    
    amx_GetAddr(amx, params[2], &cell_addrs[0]);
	amx_GetAddr(amx, params[3], &cell_addrs[1]);

	if ( cell_addrs[0] == NULL || cell_addrs[1] == NULL )
		return 0;

	*cell_addrs[0] = static_cast<cell>(png->GetImage()->get_width());
	*cell_addrs[1] = static_cast<cell>(png->GetImage()->get_height());

	return 1;
}

AMX_NATIVE_INFO pluginNatives[] =
{
	{ "LoadPNG",		LoadPNG			},
	{ "FreePNG",		FreePNG			},
	{ "RefreshPNG",		RefreshPNG		},
	{ "ReadPixelInfo",	ReadPixelInfo	},
	{ "ReadImageInfo",	ReadImageInfo	},
	{ NULL,				NULL			}
};

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx) 
{	
	int amx_idx = 0;
	if (amx_FindPublic(amx, "OnPNGLoad", &amx_idx) == AMX_ERR_NONE)
	{
		amx_list.push_back(amx);
	}
	return amx_Register( amx, pluginNatives, -1 );
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx) 
{
	pngManger->FreeByRef(amx);
	amx_list.remove(amx);
	return AMX_ERR_NONE;
}