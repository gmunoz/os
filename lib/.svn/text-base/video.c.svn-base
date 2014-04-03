#ifndef _VIDEO_C
#define _VIDEO_C

#include <video.h>

void clear_screen(char clear_to, char attrib)
{
	char *text_video = (char*)0xB8000;
	int pos=0;

	while (pos < (80*25*2)) {
		*text_video = clear_to;
		*text_video++;
		*text_video = attrib;
		//*str++;
		pos++;
	}
}

#endif
