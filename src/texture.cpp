#include <dc.hpp>

struct dc::texture* dc::load_texture(const char* path)
{
	int size_x, size_y, channels;
	unsigned char* texture_u8=stbi_load(path, &size_x, &size_y, &channels, 0);
	if(!texture_u8) //print error message
	{
		std::cout << "ERROR: couldn't load texture from " << path << std::endl;
		return nullptr;
	}

	//convert image
	dc::texture* texture=new dc::texture;
	dc::color* data=new dc::color[size_x*size_y];
	texture->data=data;
	texture->size_x=size_x;
	texture->size_y=size_y;

	for(int iy=0; iy<size_y; iy++)
	{
		for(int ix=0; ix<size_x; ix++)
		{
			int offset=ix+iy*size_x;
			data[offset].r=(float)texture_u8[offset*channels]/255;
			data[offset].g=(float)texture_u8[offset*channels+1]/255;
			data[offset].b=(float)texture_u8[offset*channels+2]/255;
			if(channels==4)
			{
				data[offset].a=(float)texture_u8[offset*channels+3]/255;
			}
			else
			{
				data[offset].a=1.0;
			}
		}
	}
	stbi_image_free(texture_u8);

	std::cout << size_x << "x" << size_y << "x" << channels << std::endl;
	return texture;
}
