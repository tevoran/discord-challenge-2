#include <dc.hpp>

void dc::renderer::render()
{
	//prepare the triangles
	int num_tris=triangles.size();
	render_tris.clear();

	for(int i=0; i<num_tris; i++)
	{
		triangles.data()[i].normal=glm::cross(
			glm::vec3(triangles.data()[i].v[1]-triangles.data()[i].v[0]),
			glm::vec3(triangles.data()[i].v[2]-triangles.data()[i].v[0]));
		triangles.data()[i].normal=glm::normalize(triangles.data()[i].normal);

		if(	triangles.data()[i].v[0].z>1 ||
			triangles.data()[i].v[1].z>1 || 
			triangles.data()[i].v[2].z>1)
		{
			render_tris.emplace_back(triangles.data()[i]);
		}
	}

	//naive approach: check for each triangle for each pixel
	num_tris=render_tris.size();
	for(int iy=0; iy<m_res_y; iy++)
	{
		int line_offset=iy*m_res_x;
		int offset=line_offset;
		for(int ix=0; ix<m_res_x; ix++)
		{
			m_render_surface[offset]={0.0,0.0,0.0,1.0};

			glm::vec3& d=m_pixel_direction[offset]; //current pixels direction
			glm::vec3 intersection_point;			
			color color={0.0,0.0,0.0,1.0};
			float depth;
			float current_depth=0;
			glm::vec2 bary;
			glm::vec3 origin=glm::vec3(0,0,0);
			bool no_ray=false;

			//check all the triangles for intersections
			for(int i=0; i<num_tris; i++)
			{

				if(glm::intersectRayTriangle(
					origin,
					d,
					render_tris.data()[i].v[0],
					render_tris.data()[i].v[1],
					render_tris.data()[i].v[2],
					bary,
					depth))
				//if(intersect(triangles.data()[i], d, intersection_point, color, depth))
				{
					if(current_depth==0) //if depth value hasn't been set, set it
					{
						current_depth=depth;
					}
					if(current_depth<=depth)
					{
						//calculating texture coords
						glm::vec2 tex_coords=(render_tris.data()[i].st[0]); //equalds v1
						tex_coords=
							tex_coords
							+bary.x
							*(render_tris.data()[i].st[1] - render_tris.data()[i].st[0])
							+bary.y
							*(render_tris.data()[i].st[2] - render_tris.data()[i].st[0]);
						color=render_tris.data()[i].texture->data[(int)(tex_coords.x*32 + tex_coords.y*32*32)];
					}
				}
			}
			m_render_surface[offset]=color;
			offset++; //calculating the next ray in the next loop run
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
	TG_init("Discord Challenge 2 Software Raytracer", 1920, 1080, false, 0);
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
			m_pixel_direction[offset]=glm::vec3(
				((float)ix/m_res_x)-0.5,
				((float)iy/m_res_y)-0.5,
				1.0);
		}
	}

	//adding triangles
	tri tri;
	tri.v[0].x=-0.5;
	tri.v[0].y=-0.5;
	tri.v[0].z=1.5;
	tri.st[0].x=0.0;
	tri.st[0].y=0.0;

	tri.v[1].x=0.5;
	tri.v[1].y=-0.5;
	tri.v[1].z=1.5;
	tri.st[1].x=1.0;
	tri.st[1].y=0.0;

	tri.v[2].x=0;
	tri.v[2].y=0.5;
	tri.v[2].z=1.5;
	tri.st[2].x=0.5;
	tri.st[2].y=0.5;

	tri.texture=load_texture("assets/smile.png");
	triangles.emplace_back(tri);

	tri.v[0].x=-2.0;
	tri.v[0].y=-1.0;
	tri.v[0].z=3.5;

	tri.v[1].x=1.0;
	tri.v[1].y=-0.5;
	tri.v[1].z=1.1;

	tri.v[2].x=0.8;
	tri.v[2].y=0.5;
	tri.v[2].z=1.1;

	//triangles.emplace_back(tri);
	
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
	glm::vec3& pixel_direction,
	glm::vec3& intersection_point,
	color& color,
	float& depth)
{
	//if ray is parallel to the triangle then return false
	float nd=glm::dot(tri.normal,pixel_direction);
	if(nd==0)
	{
		return false;
	}

	float e=glm::dot(tri.normal,tri.v[0]); //e equals normal dot vetex A
	depth=e/nd;
	intersection_point=depth*pixel_direction;

	//checking if intersection point is inside the triangle
	float a=glm::dot
		(
			glm::cross
			(
				(tri.v[1]-tri.v[0]),
				(intersection_point-tri.v[0])
			),
			tri.normal
		);
	if(a<0)
	{
		return false;
	}
	float b=glm::dot
		(
			glm::cross
			(
				(tri.v[2]-tri.v[1]),
				(intersection_point-tri.v[1])
			),
			tri.normal
		);
	if(b<0)
	{
		return false;
	}
	float c=glm::dot
		(
			glm::cross
			(
				(tri.v[0]-tri.v[2]),
				(intersection_point-tri.v[2])
			),
			tri.normal
		);
	if(c<0)
	{
		return false;
	}


	//intersection
	color.r=0.9;
	color.g=0.7;
	color.b=0.02;
	color.a=1;
	return true;
}