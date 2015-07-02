#ifndef _FILEDIALOG_H
#define _FILEDIALOG_H

#include "draw.h"

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

typedef enum
{
	FileDialogResult_Error = -1,
	FileDialogResult_Ok = 0,
	FileDialogResult_Cancel = 1,
} FileDialogResult;

//! \brief open a window where it's possible to select a single file
//! \param[in] surface where the file dialog will be displayed, if necessary
//! \param[in] font used by file dialog, if necessary
//! \param[out] filename that was selected; it's recommended to use an array not shorter than MAX_PATH
//! \param[in] extfilter RESERVED FOR FUTURE USE. Set it to NULL
//! \param[in] directory RESERVED FOR FUTURE USE. Set it to NULL
//! \return result of operation
FileDialogResult FileDialogOpen(Surface *surface, Font font, char *filename, const char *extfilter, const char *directory);

#endif