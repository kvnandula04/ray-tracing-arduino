#include <stdio.h>
#include <math.h>

#define INF_DIST 1000
#define CAM_DIST 50 // This means we are looking backwards on x - bottom right is (-ve x -ve y)
#define MOVE_DIST 2

#define SCREENWIDTH 60
#define SCREENHEIGHT 60

#define HALF_WIDTH (SCREENWIDTH / 2)
#define HALF_HEIGHT (SCREENHEIGHT / 2)

#define num_spheres 3

#define LIGHT_X 100
#define LIGHT_Y 100
#define LIGHT_Z 50

#define UP 'z'
#define DOWN 'x'
#define LEFT 'a'
#define RIGHT 'd'
#define FORWARD 'w'
#define BACK 's'

typedef struct {
    int x;
    int y;
    int z;
    int rad;
} sphere;


float Q_rsqrt( float number )
// THE ONE AND ONLY
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;                       // evil floating point bit level hacking
	i  = 0x5f3759df - ( i >> 1 );               // what the fuck? 
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

	return y;
}


sphere create_sphere(int x, int y, int z, int rad) {
    sphere out;
    out.x = x;
    out.y = y;
    out.z = z;
    out.rad = rad;
    return out;
}


/**
rayhits object?
    get x y z of intersect
    draw normal from centre to point
    draw light ray to light from point
    angle between normal & light ray = brightness (smaller = brighter)

return nothing
 */
int get_brightness(int x, int y, int z, sphere centre) {
    int nx = centre.x - x;
    int ny = centre.y - y;
    int nz = centre.z - z;

    int lx = x - LIGHT_X;
    int ly = y - LIGHT_Y;
    int lz = z - LIGHT_Z;

    int ndotl = nx*lx + ny*ly + nz*lz;

    float theta = acosf(ndotl * Q_rsqrt(nx*nx + ny*ny + nz*nz) * Q_rsqrt(lx*lx + ly*ly + lz*lz));

    return theta * 15 * M_1_PI;
}


int get_distance_sphere(float x, float y, float z, sphere target) {
    int
    a = target.x,
    b = target.y,
    c = target.z,
    rad = target.rad;
   
    // B^2 -4AC
    // A = 1 because we scaled
    int B = -2 * (a*x + b*y + c*z);
    int C = a*a + b*b + c*c - rad*rad;

    int b4ac = B*B - 4 * C;

    if (b4ac < 0) {
        return INF_DIST;
    }

    int result = (-B + sqrt((double)b4ac)) / 2;
    return result;
}


void print_pixel(sphere * sphere_list, int x, int y) {
    int out = INF_DIST;
    int result;

    x = HALF_WIDTH - x;
    y = HALF_HEIGHT - y;
    int z = CAM_DIST;

    // Scale the vectors to unit vectors
    float scale = Q_rsqrt(x*x + y*y + z*z);
    float
    vec_x = x*scale,    
    vec_y = y*scale,    
    vec_z = z*scale;

    sphere closest;

    for (int i = 0; i < num_spheres; i++) {
        result = get_distance_sphere(vec_x, vec_y, vec_z, sphere_list[i]);
        if (result < out) {
            out = result;
            closest = sphere_list[i];
        }
    }

    if (out == INF_DIST || out < 0) {
        printf(" ");
    } else {
        // printf("1");
        out = (9 * get_brightness(out*vec_x, out*vec_y, out*vec_z, closest) / 15);
        printf("%d", out);
    }
}


void display(sphere* sphere_list) {
    for (int y = 0; y < SCREENHEIGHT; y++) {
        for (int x = 0; x < SCREENWIDTH; x++) {
            print_pixel(sphere_list, x, y);
        }
        printf("\n");
    }
    printf("\n");
}


int main() {

    // No time for mallocs, just use the number twice
    sphere sphere_list[num_spheres];
    sphere_list[0] = create_sphere(-20, -20, 30, 15);
    sphere_list[1] = create_sphere(-20, 20, 20, 15);
    sphere_list[2] = create_sphere(20, 20, 20, 15);

    char direction;
    display(sphere_list);

    while (1==1) {
    direction = (char)getchar();

    switch (direction) {
        case UP:
            for (int i = 0; i < num_spheres; i++) {
                sphere_list[i].y -= MOVE_DIST;
            }
            break;

        case LEFT:
            for (int i = 0; i < num_spheres; i++) {
                sphere_list[i].x -= MOVE_DIST;
            }
            break;

        case RIGHT:
            for (int i = 0; i < num_spheres; i++) {
                sphere_list[i].x += MOVE_DIST;
            }
            break;

        case DOWN:
            for (int i = 0; i < num_spheres; i++) {
                sphere_list[i].y += MOVE_DIST;
            }
            break;

        case FORWARD:
            for (int i = 0; i < num_spheres; i++) {
                sphere_list[i].z -= MOVE_DIST;
            }
            break;

        case BACK:
            for (int i = 0; i < num_spheres; i++) {
                sphere_list[i].z += MOVE_DIST;
            }
            break;

        default:
            // printf("I HAVE FAILED YOU MASTER");
            display(sphere_list);
            break;
    }
    }

    return 0;
}
