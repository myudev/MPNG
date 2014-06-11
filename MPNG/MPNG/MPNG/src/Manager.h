#ifndef _H_MANGER
#define _H_MANGER
typedef png::image<png::rgb_pixel> PNGIMG;

#define SAFE_DELETE(p)	if(p) { delete p; p = NULL; } 

class CPNG {
public:
	CPNG ( const char *szPath );
	PNGIMG *GetImage() { if ( !bIsRefreshing ) return image; else return tempimage; } 
	bool Load ( );
	bool Refresh ( );
	void SetRefresh (bool bVal) { if ( bVal ) { bIsRefreshing = true; tempimage = image; image = NULL; } else { bIsRefreshing = false; SAFE_DELETE(tempimage); }  }
	bool GetRefreshing() {  return bIsRefreshing; } 

	void Free() { SAFE_DELETE(image); SAFE_DELETE(tempimage); }  /* uh no, dont release the kraken! */
	AMX *ref;

private:
	bool bIsRefreshing;
	std::string path;
	PNGIMG *image;
	PNGIMG *tempimage;
};

/*
	So, before I get questions why hardcoded SLOTS of PNG's ?
	It's bascially like this:
	No one would actually open more than 32767 PNG's the same time, mostly just for fucking something up.
	But even if we get numbers over 32767 we can easly expand it so there's no need for hassle.
	Oh and it makes thing's just a lil bit easier.
*/
#define HARDCODED_MAX_PNGS	(32767)
typedef signed SLOT; // Use SLOT, so we can expand it everytime we want without messing too much with the highest number of identifiers.

enum ERR
{
	NO_FREE_SLOT = -1,
	INVALID_HANDLE = -2,
	NONE = -3
};

class CPNGManager {
	public:
		CPNGManager();
		SLOT Create ( const char* szPath ); /* Slot OR ERROR. */
		bool FreePNG ( SLOT iSlot );
		CPNG* GetPNG ( SLOT iSlot );		
		void FreeByRef ( AMX *ref );

	private:
		SLOT GetFreeSlot();
		CPNG *pnglist [ HARDCODED_MAX_PNGS ];
};

#endif