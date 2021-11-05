#include <TGJGE.h>

int main()
{
	TG_init("Discord Challenge 2 Software Raytracer", 1366, 768, false, 0);

	TG_new_frame(0,0,0);
	SDL_Delay(2000);

	TG_quit();
}