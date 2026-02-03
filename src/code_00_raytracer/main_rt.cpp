#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>

using namespace std;
 
#define FAR_AWAY 10e20

/*
Simple class to implement an image saved in PPM format
https://netpbm.sourceforge.net/doc/ppm.html
Few applications load it.
One is IrfanView: https://www.irfanview.com/
*/
struct image {
	image(int _w, int _h) :w(_w), h(_h) { data.resize(w * h * 3, 0); }
	unsigned int w, h;

	std::vector<int>  data;

	template <class S>
	void set_pixel(int i, int j, S  r, S  g, S  b) {
		j = h - 1 - j;
		data[(j * w + i) * 3] = (unsigned char)r;
		data[(j * w + i) * 3 + 1] = (unsigned char)g;
		data[(j * w + i) * 3 + 2] = (unsigned char)b;
	}

	void save(const char* filename) {
		ofstream f;
		f.open(filename);
		f << "P3\n";
		f << w << " " << h << std::endl;

		f << *(std::max_element(data.begin(), data.end())) << std::endl;

		for (unsigned int i = 0; i < data.size() / 3; ++i)
			f << data[i * 3] << " " << data[i * 3 + 1] << " " << data[i * 3 + 2] << std::endl;
		f.close();
	}
};


struct p3 {
	p3(){}
	p3(float _x, float _y, float _z):x(_x), y(_y), z(_z){}

	p3 operator -(p3 o) { return p3(o.x - x, o.y - y, o.z - z); }
	float operator *(p3 o) { return x * o.x + y * o.y + z * o.z; }
	float x, y, z;
};

struct ray {
	ray(p3 o, p3 d):orig(o),dir(d){}

	p3 orig, dir;
};

struct sphere {
	sphere(p3 c, float r, p3 col):center(c),radius(r),color(col){}

	p3 center, color;
	float radius;
};

float  hit_sphere(ray r, sphere s) {
	float A = r.dir* r.dir;
	float B = r.dir*(r.orig - s.center) * 2;
	float C = (r.orig - s.center)*(r.orig - s.center) - s.radius * s.radius;

	float delta = B * B - 4 * A * C;

	if (delta < 0)
		return FAR_AWAY;

	float t = (-B - sqrt(B * B - 4 * A * C)) / (2 * A);

	return t;
}

int main(int args, char** argv) {
	int sx = 800;
	int sy = 800;
	image a(sx, sy);

	p3 eye = p3(0, 0, 0);

	std::vector< sphere > scene;
	scene.push_back(sphere(p3(0, 0, -3),   1.0, p3(255,0,0) ) );
	scene.push_back(sphere(p3(0, 0, -4.0), 0.2, p3(0, 0, 255)));

	for (int i = 0; i < a.w; ++i)
		for (int j = 0; j < a.h; ++j) {
			p3 pixpos(-1 + 2 * i / float(a.w), -1 + 2 * j / float(a.h), -1);
			ray r = ray(eye, pixpos - eye);

			float min_t = FAR_AWAY;
			p3 col = p3(0, 0, 0);
			for (int is = 0; is < scene.size(); ++is) {
				float t = hit_sphere(r, scene[is]);
				if (t < min_t) {
					min_t = t;
					col = scene[is].color;
				}
			}
			a.set_pixel(i, j, col.x, col.y, col.z);
		}

	a.save("rendering.ppm");
	return 0;
}