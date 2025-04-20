/****************************************************************************
 * Multiboot Script Loader
 * KittyPBoxx 2024
 *
 * ui.h
 * Handle displaying things and controller input
 ***************************************************************************/
#include <ogcsys.h>
#include <gccore.h>
#include <string.h>

void warnError(char *msg)
{
	puts(msg);
	VIDEO_WaitVSync();
	VIDEO_WaitVSync();
	sleep(2);
}
void fatalError(char *msg)
{
	puts(msg);
	VIDEO_WaitVSync();
	VIDEO_WaitVSync();
	sleep(5);
	exit(0);
}

void endproc()
{
	printf("Start pressed, exit\n");
	VIDEO_WaitVSync();
	VIDEO_WaitVSync();
	exit(0);
}