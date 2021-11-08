#include <dc.hpp>

int main()
{
	dc::renderer renderer(192,108);

	while(!TG_is_key_pressed(SDL_SCANCODE_ESCAPE))
	{
		renderer.render();
	}
}