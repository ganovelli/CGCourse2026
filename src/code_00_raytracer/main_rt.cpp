#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <math.h>

#include <random>
using namespace std;
 
#define FAR_AWAY 10e20

/*
Simple class to implement an image saved in PPM format
https://netpbm.sourceforge.net/doc/ppm.html
Few applications load it.
One is IrfanView: https://www.irfanview.com/
*/
struct image {
	image(int _w, int _h) :w(_w), h(_h) { data.resize(w * h * 3, 0); } // initialize buffer (RGB per pixel)
	unsigned int w, h;

	std::vector<int>  data;

	// Set a pixel value (values expected in 0..255)
	template <class S>
	void set_pixel(int i, int j, S  r, S  g, S  b) {
		j = h - 1 - j; // flip vertically for image coordinate system
		data[(j * w + i) * 3] = (unsigned char)r;
		data[(j * w + i) * 3 + 1] = (unsigned char)g;
		data[(j * w + i) * 3 + 2] = (unsigned char)b;
	}
	template <class S>
	void additive_pixel(int i, int j, S  r, S  g, S  b) {
		j = h - 1 - j; // flip vertically for image coordinate system
		data[(j * w + i) * 3] = 0.5 * data[(j * w + i) * 3] + (unsigned char)r;
		data[(j * w + i) * 3 + 1] = 0.5 * (data[(j * w + i) * 3 + 1] + (unsigned char)g);
		data[(j * w + i) * 3 + 2] = 0.5 *(data[(j * w + i) * 3 + 2] + (unsigned char)b);
	}

	// Save image as ASCII PPM (P3) file
	void save(const char* filename) {
		ofstream f;
		f.open(filename);
		f << "P3\n";
		f << w << " " << h << std::endl;

		// max color value (use maximum found in buffer)
		f << *(std::max_element(data.begin(), data.end())) << std::endl;

		// write each pixel as "R G B" per line
		for (unsigned int i = 0; i < data.size() / 3; ++i)
			f << data[i * 3] << " " << data[i * 3 + 1] << " " << data[i * 3 + 2] << std::endl;
		f.close();
	}
};


struct p3 {
	p3():x(0.f), y(0.f), z(0.f) {} // default constructor
	p3(float _x, float _y, float _z):x(_x), y(_y), z(_z){} // value constructor

	// vector arithmetic helpers
	p3 operator +(p3 o) { return p3(o.x + x, o.y + y, o.z + z); } // sum
	p3 operator -(p3 o) { return p3(x -o.x , y  - o.y , z - o.z ); } //subtraction
	float operator *(p3 o) { return x * o.x + y * o.y + z * o.z; } // dot product
	p3 operator *(float s) { return p3(x*s, y * s,z* s); } // scalar multiply
	float x, y, z;
};

struct ray {
	ray(p3 o, p3 d):orig(o),dir(d){} // origin and direction

	p3 orig, dir;
};

struct sphere {
	sphere(p3 c, float r, p3 col):center(c),radius(r),color(col){} // center, radius, color

	p3 center, color;
	float radius;
};

struct hit_info {

	hit_info() :hit(false), t(FAR_AWAY) {} // default: no hit, t very large
	float t;   // distance along ray
	p3 p;      // hit position
	p3 n;      // surface normal at hit
	bool hit;  // whether intersection occured
	p3 color;  // object color
};

// Ray-sphere intersection: returns hit_info with nearest positive t if any
hit_info  hit_sphere(ray r, sphere s) {
	// coefficients for quadratic equation A t^2 + B t + C = 0
	float A = r.dir* r.dir;
	float B = r.dir*(r.orig - s.center) * 2;
	float C = (r.orig - s.center)*(r.orig - s.center) - s.radius * s.radius;

	float delta = B * B - 4 * A * C;

	// no real roots -> no intersection
	if (delta < 0)
		return hit_info();

	hit_info hi;
	// choose smaller root first (closest intersection)
	float t = (-B - sqrt(delta)) / (2 * A);
	if( t <= 0 )
		t = (-B + sqrt(delta)) / (2 * A);
	// if still non-positive, intersection is behind ray origin
	if (t <= 0)
		return hit_info();

	// fill hit information
	hi.t = t;
	hi.p =  r.orig + r.dir * t;
	hi.n = hi.p - s.center;					 // unnormalized normal
	hi.n = hi.n * (1.0 / sqrt(hi.n * hi.n)); // normalize normal
	hi.color = s.color;
	hi.hit = true;
	return hi;
}
float randomFloat()
{
	return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}
int main(int args, char** argv) {
	int sx = 800;
	int sy = 800;
	image a(sx, sy); 

	p3 eye = p3(0, 0, 0); // camera position

	// scene setup: two spheres with colors
	std::vector< sphere > scene;
	scene.push_back(sphere(p3(0, 0, -3),   1.0, p3(255,0,0) ) );
	scene.push_back(sphere(p3(0.6, 0.6, -2.0), 0.2, p3(0, 0, 255)));

	p3 Lp = p3(1, 1, -1); // point light position

	// iterate over image pixels (simple pinhole camera)
	for (int i = 0; i < a.w; ++i)
		for (int j = 0; j < a.h; ++j) 
			for (int raysPerPixel = 0; raysPerPixel < 4; raysPerPixel++)
			{

				// compute pixel position on image plane in [-1,1] range
				p3 pixpos(-1 + 2 * (i+0.5 + randomFloat()*0.5f) / float(a.w), -1 + 2 * (j+0.5+randomFloat()*0.5f) / float(a.h), -1);
				ray r = ray(eye, pixpos - eye); // primary ray

				hit_info best_hi = hit_info(); // best intersection so far
				p3 col = p3(0, 0, 0); // background color (black)
				for (int is = 0; is < scene.size(); ++is) {
					hit_info hi = hit_sphere(r, scene[is]); // test intersection
					if (hi.t < best_hi.t) { // closer hit found
						best_hi = hi;
						p3 p = r.orig + r.dir * hi.t; // hit point
						p3 L = Lp - p; // vector to light
						L = L * (1.0 / sqrt(L * L)); // normalize L

						// offset origin slightly to avoid self-intersection (shadow acne)
						ray shadow_ray = ray(p + L*0.001, L);

						// check for occlusion: if any object blocks the light, point is in shadow
						int iss = 0;
						for (; iss < scene.size(); ++iss)
							if (hit_sphere(shadow_ray, scene[iss]).hit)
								break;
					
						if (iss == scene.size()) { // light visible -> simple Lambertian shading
							float cosLN = hi.n * L;
							float al = max(0.f, cosLN); // clamp negative values
							col = hi.color * al; // scale object color by diffuse term
						}
					}
				}
				a.additive_pixel(i, j, col.x, col.y, col.z); // write pixel
			}
		

	a.save("rendering.ppm"); // save to disk
	return 0;
}
