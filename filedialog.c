#include "filedilaog.h"

#if defined(_WIN32)
#include <windows.h>
#elif defined(PLATFORM_PSP2)
#endif

FileDialogResult FileDialogOpen(Surface *surface, Font font, char *filename, const char *extfilter, const char *directory)
{
#if defined(_WIN32)
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = GetConsoleWindow();
	ofn.lpstrFile = filename;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = "All\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn) == TRUE)
		return FileDialogResult_Ok;
	DWORD err = CommDlgExtendedError();
	if (err == 0)
		return FileDialogResult_Cancel;
	return FileDialogResult_Error;
#elif defined(PLATFORM_PSP2)

#endif
}