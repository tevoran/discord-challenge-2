#include <dc.hpp>

void dc::renderer::render()
{
	//naive approach: check for each triangle for each pixel
	for(int iy=0; iy<m_res_y; iy++)
	{
		for(int ix=0; ix<m_res_x; ix++)
		{
			int offset=ix+iy*m_res_x;
			glm::vec3& d=m_pixel_direction[offset]; //current pixels direction
			glm::vec3 intersection_point;			
			color color;
			float depth;
			float current_depth=0;

			//check all the triangles for intersections
			for(int i=0; i<triangles.size(); i++)
			{
				if(intersect(triangles[i], d, intersection_point, color, depth))
				{
					if(current_depth==0) //if depth value hasn't been set, set it
					{
						current_depth=depth;
					}
					m_render_surface[offset]=color;
				}
			}

		}
	}


	//send results to the GPU to render them
	m_render_result=TG_new_texture_custom(
		m_render_surface,
		m_res_x,
		m_res_y,
		1,
		false);
	TG_object_use_texture(m_render_object, m_render_result);
	TG_render_object(m_render_object);
	TG_destroy_texture(m_render_result);

	TG_new_frame(0,0,0);
	std::cout << "frame time: " << TG_delta_time() << "ms" << std::endl;
}

dc::renderer::renderer(int renderer_res_x, int renderer_res_y)
{
	TG_init("Discord Challenge 2 Software Raytracer", 1920, 1080, true, 0);
	m_render_object=TG_new_object(1.0, 1.0, 0.0, 0.0);
	m_res_x=renderer_res_x;
	m_res_y=renderer_res_y;
	m_render_surface=new color[m_res_x*m_res_y];
	m_pixel_direction=new glm::vec3[m_res_x*m_res_y];

	//precalculate pixel directions
	for(int iy=0; iy<m_res_y; iy++)
	{
		for(int ix=0; ix<m_res_x; ix++)
		{
			int offset=ix+iy*m_res_x;
			m_pixel_direction[offset].z=1.0; //always same distance from player
			m_pixel_direction[offset].x=((float)ix/m_res_x)-0.5;
			m_pixel_direction[offset].y=((float)iy/m_res_y)-0.5;
		}
	}

	//adding triangles
	tri tri;
	tri.v[0].x=-0.5;
	tri.v[0].y=-0.5;
	tri.v[0].z=1.5;

	tri.v[1].x=0.5;
	tri.v[1].y=-0.5;
	tri.v[1].z=1.5;

	tri.v[2].x=0;
	tri.v[2].y=0.5;
	tri.v[2].z=1.5;
	triangles.emplace_back(tri);
	
}

dc::renderer::~renderer()
{
	delete [] m_pixel_direction;
	delete [] m_render_surface;
	TG_destroy_object(m_render_object);
	TG_quit();
}

bool dc::renderer::intersect(
	tri& tri,
	glm::vec3& pixel_diretion,
	glm::vec3& intersection_point,
	color& color,
	float& depth)
{
	//calculate normal
	glm::vec3 n=glm::cross(glm::vec3(tri.v[1]-tri.v[0]),glm::vec3(tri.v[2]-tri.v[0]));
	n=glm::normalize(n);

	glm::vec3& d=pixel_diretion;

	//if ray is parallel to the triangle then return false
	float nd=glm::dot(n,d);
	if(nd==0)
	{
		return false;
	}

	float e=glm::dot(n,tri.v[0]); //e equals normal dot vetex A
	depth=e/nd;
	intersection_point=depth*pixel_diretion;

	//checking if intersection point is inside the triangle
	float a=glm::dot
		(
			glm::cross
			(
				glm::vec3(tri.v[1]-tri.v[0]),
				glm::vec3(intersection_point-tri.v[0])
			),
			n
		);
	float b=glm::dot
		(
			glm::cross
			(
				glm::vec3(tri.v[2]-tri.v[1]),
				glm::vec3(intersection_point-tri.v[1])
			),
			n
		);
	float c=glm::dot
		(
			glm::cross
			(
				glm::vec3(tri.v[0]-tri.v[2]),
				glm::vec3(intersection_point-tri.v[2])
			),
			n
		);
	if(a>=0 && b>=0 && c>=0) //intersection
	{
		color.r=0.9;
		color.g=0.7;
		color.b=0.02;
		color.a=1;
		return true;
	}
	return false;
}