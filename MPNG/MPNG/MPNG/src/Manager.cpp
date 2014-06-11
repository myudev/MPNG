#include "main.h"


CPNGManager::CPNGManager()
{
	int i_SLOT = 0;
	for ( ; i_SLOT != HARDCODED_MAX_PNGS; i_SLOT++ )
		pnglist[i_SLOT] = NULL;
}

CPNG::CPNG ( const char *szPath )
{
	path.append(szPath);
	bIsRefreshing = NULL;
	image = NULL;
	tempimage = NULL;
}

bool CPNG::Load ( )
{

	FILE* fHandle = fopen(path.c_str(), "rb");
	if (!fHandle) return false;
	fclose(fHandle);

	image = new PNGIMG(path);
	if ( GetImage() == NULL ) return false; // Do some own checking later.

	return true;
}

bool CPNG::Refresh ( )
{
	SAFE_DELETE(tempimage); // free the old image 
	FILE* fHandle = fopen(path.c_str(), "rb");
	if (!fHandle) return false;
	fclose(fHandle);

	image = new PNGIMG(path);
	if ( GetImage() == NULL ) return false; // Do some own checking later.

	return true;
}


SLOT CPNGManager::Create ( const char* szPath )
{
	if ( !szPath ) return INVALID_HANDLE;

	SLOT i_Slot = GetFreeSlot();
	if ( i_Slot == -1 )	return NO_FREE_SLOT;
	CPNG *png = new CPNG( szPath );
	
	pnglist[i_Slot] = png;
	return i_Slot;
}

/*
SLOT CPNGManager::LoadPNG ( const char* szPath )
{
	if ( !szPath ) return ERR::INVALID_HANDLE;

	SLOT i_Slot = GetFreeSlot();
	if ( i_Slot == -1 )	return ERR::NO_FREE_SLOT;

	CPNG *png = new CPNG( szPath );
	if ( png->GetImage() == NULL ) return ERR::INVALID_HANDLE; // Do some own checking later.
	pnglist[i_Slot] = png;
	return i_Slot;
}
*/

bool CPNGManager::FreePNG ( SLOT iSlot ) 
{
	if ( iSlot < 0 || iSlot > HARDCODED_MAX_PNGS )	return false;
	if ( pnglist[iSlot] == NULL )					return false;

	pnglist[iSlot]->Free();
	SAFE_DELETE(pnglist[iSlot]);
	return true;
}

CPNG* CPNGManager::GetPNG ( SLOT iSlot ) 
{
	if ( iSlot < 0 || iSlot > HARDCODED_MAX_PNGS )	return NULL;
	if ( pnglist[iSlot] == NULL )					return NULL;

	return pnglist[iSlot];
}
SLOT CPNGManager::GetFreeSlot( ) 
{
	for ( SLOT i_SLOT = 0; i_SLOT != HARDCODED_MAX_PNGS; i_SLOT++ )
	{
		if ( pnglist[i_SLOT] != NULL ) continue;
		return i_SLOT;
	}
	return -1;
}

void CPNGManager::FreeByRef ( AMX *ref )
{
	for ( SLOT i_SLOT = 0; i_SLOT != HARDCODED_MAX_PNGS; i_SLOT++ )
	{
		if ( pnglist[i_SLOT] == NULL ) continue;
		if ( GetPNG(i_SLOT)->ref != ref ) continue;

		FreePNG(i_SLOT);
	}
}