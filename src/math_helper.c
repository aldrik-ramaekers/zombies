#include "../include/math_helper.h"


bool onSegment(vec2f p, vec2f q, vec2f r)
{
    if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
        q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
       return true;
  
    return false;
}

int orientation(vec2f p, vec2f q, vec2f r)
{
    int val = (q.y - p.y) * (r.x - q.x) -
              (q.x - p.x) * (r.y - q.y);
  
    if (val == 0) return 0;  // collinear
  
    return (val > 0)? 1: 2; // clock or counterclock wise
}

vec2f get_dir_of_line(vec2f p1, vec2f p2) {
	float dirx = (p1.x - p2.x);
	float diry = (p1.y - p2.y);
	double length = sqrt(dirx * dirx + diry * diry);
	dirx /= length;
	diry /= length;
	return (vec2f){dirx, diry};
}

bool neg2(float p1, float p2) {
	return p1 < 0.0f && p2 < 0.0f || abs(p1-p2) < 1.0f;
}

bool pos2(float p1, float p2) {
	return (p1 >= 0.0f && p2 >= 0.0f) || abs(p1-p2) < 1.0f;
}

vec3f get_center_of_square(vec3f position, vec3f size) {
	return (vec3f){position.x + size.x/2, position.y + size.y/2, position.z + size.z/2};
}

bool lines_intersect(vec2f p1, vec2f q1, vec2f p2, vec2f q2)
{
    int o1 = orientation(p1, q1, p2);
    int o2 = orientation(p1, q1, q2);
    int o3 = orientation(p2, q2, p1);
    int o4 = orientation(p2, q2, q1);
  
    // General case
    if (o1 != o2 && o3 != o4) {
		vec2f bdir = get_dir_of_line(p1, q1);
		vec2f pdir = get_dir_of_line(p1, p2);
		if (((p1.x <= p2.x && q1.x >= p2.x) || (p1.x >= p2.x && q1.x <= p2.x)) || ((p1.y <= p2.y && q1.y >= p2.y) || (p1.y >= p2.y && q1.y <= p2.y)))
		if ((neg2(bdir.x, pdir.x) || pos2(bdir.x, pdir.x)) && (neg2(bdir.y, pdir.y) || pos2(bdir.y, pdir.y))) return true;
	}
  
    // Special Cases
    // p1, q1 and p2 are collinear and p2 lies on segment p1q1
    if (o1 == 0 && onSegment(p1, p2, q1)) return true;
  
    // p1, q1 and q2 are collinear and q2 lies on segment p1q1
    if (o2 == 0 && onSegment(p1, q2, q1)) return true;
  
    // p2, q2 and p1 are collinear and p1 lies on segment p2q2
    if (o3 == 0 && onSegment(p2, p1, q2)) return true;
  
     // p2, q2 and q1 are collinear and q1 lies on segment p2q2
    if (o4 == 0 && onSegment(p2, q1, q2)) return true;
  
    return false; // Doesn't fall in any of the above cases
}

vec2f get_intersection_point(vec2f A, vec2f B, vec2f C, vec2f D) {
	// Line AB represented as a1x + b1y = c1
    double a1 = B.y - A.y;
    double b1 = A.x - B.x;
    double c1 = a1*(A.x) + b1*(A.y);
 
    // Line CD represented as a2x + b2y = c2
    double a2 = D.y - C.y;
    double b2 = C.x - D.x;
    double c2 = a2*(C.x)+ b2*(C.y);
 
    double determinant = a1*b2 - a2*b1;
 
    if (determinant == 0)
    {
        // The lines are parallel. This is simplified
        // by returning a pair of FLT_MAX
        return (vec2f){__FLT_MAX__, __FLT_MAX__};
    }
    else
    {
        double x = (b2*c1 - b1*c2)/determinant;
        double y = (a1*c2 - a2*c1)/determinant;
        return (vec2f){x, y};
    }
}

box get_box_of_square(vec3f position, vec3f size) {
	vec2f rendertl = (vec2f){position.x, position.y};
	vec2f rendertr = (vec2f){position.x + size.x, position.y};
	vec2f renderbr = (vec2f){position.x + size.x, position.y + size.y};
	vec2f renderbl = (vec2f){position.x, position.y + size.y};

	return (box){rendertl, rendertr, renderbl, renderbr, rendertl, rendertr, renderbl, renderbr};
}

box get_render_box_of_square_without_incline(platform_window* window, vec3f position, vec3f size) {
	map_info info = get_map_info(window);
	float render_x = (info.tile_width * position.x);
	vec2f rendertl = (vec2f){render_x, info.tile_width * position.y - position.z*info.px_raised_per_h};
	vec2f rendertr = (vec2f){render_x + info.tile_width*size.x, info.tile_height * position.y - position.z*info.px_raised_per_h};
	vec2f renderbr = (vec2f){render_x + info.tile_width*size.x, info.tile_height * position.y + info.tile_height*size.y - position.z*info.px_raised_per_h};
	vec2f renderbl = (vec2f){render_x, info.tile_height * position.y + info.tile_height*size.y - position.z*info.px_raised_per_h};

	position.z += size.z;
	vec2f rendertl2 = (vec2f){render_x, info.tile_width * position.y - position.z*info.px_raised_per_h};
	vec2f rendertr2 = (vec2f){render_x + info.tile_width*size.x, info.tile_height * position.y - position.z*info.px_raised_per_h};
	vec2f renderbr2 = (vec2f){render_x + info.tile_width*size.x, info.tile_height * position.y + info.tile_height*size.y - position.z*info.px_raised_per_h};
	vec2f renderbl2 = (vec2f){render_x, info.tile_height * position.y + info.tile_height*size.y - position.z*info.px_raised_per_h};

	return (box){rendertl, rendertr, renderbl, renderbr, rendertl2, rendertr2, renderbl2, renderbr2};
}

box get_render_box_of_square(platform_window* window, vec3f position, vec3f size) {
	map_info info = get_map_info(window);
	float render_x = (info.tile_width * position.x) + (info.px_incline * position.y);
	vec2f rendertl = (vec2f){render_x, info.tile_width * position.y - position.z*info.px_raised_per_h};
	vec2f rendertr = (vec2f){render_x + info.tile_width*size.x, info.tile_height * position.y - position.z*info.px_raised_per_h};
	vec2f renderbr = (vec2f){render_x + (info.px_incline+info.tile_width)*size.x, info.tile_height * position.y + info.tile_height*size.y - position.z*info.px_raised_per_h};
	vec2f renderbl = (vec2f){render_x + info.px_incline*size.x, info.tile_height * position.y + info.tile_height*size.y - position.z*info.px_raised_per_h};

	position.z += size.z;
	vec2f rendertl2 = (vec2f){render_x, info.tile_width * position.y - position.z*info.px_raised_per_h};
	vec2f rendertr2 = (vec2f){render_x + info.tile_width*size.x, info.tile_height * position.y - position.z*info.px_raised_per_h};
	vec2f renderbr2 = (vec2f){render_x + (info.px_incline+info.tile_width)*size.x, info.tile_height * position.y + info.tile_height*size.y - position.z*info.px_raised_per_h};
	vec2f renderbl2 = (vec2f){render_x + info.px_incline*size.x, info.tile_height * position.y + info.tile_height*size.y - position.z*info.px_raised_per_h};

	return (box){rendertl, rendertr, renderbl, renderbr, rendertl2, rendertr2, renderbl2, renderbr2};
}