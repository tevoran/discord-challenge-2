#pragma once
#include <TGJGE.h>
#include <glm/glm.hpp>

#include <iostream>
#include <vector>

#define RGBA 4

namespace dc
{
	struct color
	{
		float r;
		float g;
		float b;
		float a;
	};

	struct vertex
	{
		float pos[3]; //3d coordinates
		float tex[2]; //texture coordinates
	};

	struct tri
	{
		glm::vec3 v[3]; //vertices
		glm::vec3 normal; //triangle normal
		void *texture; //texture data
	};

	class renderer
	{
	private:
		TG_texture *m_render_result;
		TG_object *m_render_object;

		int m_res_x;
		int m_res_y;
		color *m_render_surface;
		glm::vec3 *m_pixel_direction;
		std::vector<tri> triangles;

		bool intersect(
			tri& tri,
			glm::vec3& pixel_diretion,
			glm::vec3& intersection_point,
			color& color,
			float& depth);
	public:
		void render();
		renderer(int renderer_res_x, int renderer_res_y);
		~renderer();
	};

}