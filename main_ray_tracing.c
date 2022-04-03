#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>

#define INF_DIST 1000
#define CAM_DIST 40 // This means we are looking backwards on x - bottom right is (-ve x -ve y)
#define MOVE_DIST 2

#define SCREENWIDTH 60
#define SCREENHEIGHT 60

#define HALF_WIDTH (SCREENWIDTH / 2)
#define HALF_HEIGHT (SCREENHEIGHT / 2)

#define num_spheres 3

#define LIGHT_STEP 50

#define LIGHT_ZERO_X 100
#define LIGHT_ZERO_Y 100
#define LIGHT_ZERO_Z 50

#define LIGHT_ONE_X 0
#define LIGHT_ONE_Y 50
#define LIGHT_ONE_Z 100

#define LIGHT_TWO_X -100
#define LIGHT_TWO_Y -100
#define LIGHT_TWO_Z 50

#define LIGHT_THREE_X 0
#define LIGHT_THREE_Y -50
#define LIGHT_THREE_Z 0

char light_x = LIGHT_ZERO_X;
char light_y = LIGHT_ZERO_Y;
char light_z = LIGHT_ZERO_Z;

char light_phase = 0;

// //  switch between 4 predet. light sources
// #define l0 'u'
// #define l1 'i'
// #define l2 'o'
// #define l3 'p'

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
    char rad;
} sphere;


// THE ONE AND ONLY
static inline float Q_rsqrt( float number )
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


static inline sphere create_sphere(int x, int y, int z, char rad) {
    sphere out;
    out.x = x;
    out.y = y;
    out.z = z;
    out.rad = rad;
    return out;
}


/*
rayhits object?
    get x y z of intersect
    draw normal from centre to point
    draw light ray to light from point
    angle between normal & light ray = brightness (smaller = brighter)

return nothing
 */
static inline int get_brightness(int x, int y, int z, sphere centre) {
    int nx = centre.x - x;
    int ny = centre.y - y;
    int nz = centre.z - z;

    int lx = x - light_x;
    int ly = y - light_y;
    int lz = z - light_z;

    int ndotl = nx*lx + ny*ly + nz*lz;

    float theta = acosf(ndotl * Q_rsqrt(nx*nx + ny*ny + nz*nz) * Q_rsqrt(lx*lx + ly*ly + lz*lz));

    return theta * 15 * M_1_PI;
}


static inline int get_distance_sphere(float x, float y, float z, sphere target) {
    int
    a = target.x,
    b = target.y,
    c = target.z;
    char rad = target.rad;
   
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


char ascii_map(int in)
{
    if (in == 0){
        return ' ';
    }
    if (in == 1){
        return ':';
    }
    if (in == 2){
        return '-';
    }
    if (in == 3){
        return '*';
    }
    if (in == 4){
        return '+';
    }
    if (in == 5){
        return '=';
    }
    if (in == 6){
        return '%';
    }
    if (in == 7){
        return '@';
    }
    return '#';
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
        printf("%c", ascii_map(out));
    }
}


static inline void display(sphere* sphere_list) {
    for (int y = 0; y < SCREENHEIGHT; y++) {
        for (int x = 0; x < SCREENWIDTH; x++) {
            print_pixel(sphere_list, x, y);
        }
        printf("\n");
    }
    printf("\n");
}

#define theta 0.05
#define ROLL 'j'
#define PITCH 'k'
#define YAW 'l'

int main() {

    // No time for mallocs, just use the number twice
    sphere sphere_list[num_spheres];
    sphere_list[0] = create_sphere(-10, -20, 50, 15);
    sphere_list[1] = create_sphere(-30, 20, 40, 15);
    sphere_list[2] = create_sphere(20, 0, 30, 15);

    char direction;
    display(sphere_list);

            // case l0:
            //     light_x = LIGHT_ZERO_X;
            //     light_y = LIGHT_ZERO_Y;
            //     light_z = LIGHT_ZERO_Z;
            //     break;
            // case l1:
            //     light_x = LIGHT_ONE_X;
            //     light_y = LIGHT_ONE_Y;
            //     light_z = LIGHT_ONE_Z;
            //     break;
            // case l2:
            //     light_x = LIGHT_TWO_X;
            //     light_y = LIGHT_TWO_Y;
            //     light_z = LIGHT_TWO_Z;
            //     break;
            // case l3:
            //     light_x = LIGHT_THREE_X;
            //     light_y = LIGHT_THREE_Y;
            //     light_z = LIGHT_THREE_Z;
            //     break;

    while (1==1) {
        direction = (char)getchar();

        switch (direction) {

            case ROLL:
                for (int i=0; i < num_spheres; i++)
                {
                    sphere_list[i].z -= CAM_DIST;
                    sphere_list[i].y = cos(theta)*sphere_list[i].y - sin(theta)*sphere_list[i].z;
                    sphere_list[i].z = sin(theta)*sphere_list[i].y + cos(theta)*sphere_list[i].z;
                    sphere_list[i].z += CAM_DIST;
                }
                break;
            case PITCH:
                for (int i=0; i < 4; i++)
                {
                    sphere_list[i].z -= CAM_DIST;
                    sphere_list[i].x = cos(theta)*sphere_list[i].x + sin(theta)*sphere_list[i].z;
                    sphere_list[i].z = - sin(theta)*sphere_list[i].x + cos(theta)*sphere_list[i].z;
                    sphere_list[i].z += CAM_DIST;
                }
                break;
            case YAW:
                for (int i=0; i < 4; i++)
                {
                    sphere_list[i].z -= CAM_DIST;
                    sphere_list[i].x = cos(theta)*sphere_list[i].x - sin(theta)*sphere_list[i].y;
                    sphere_list[i].y = sin(theta)*sphere_list[i].x + cos(theta)*sphere_list[i].y;
                    sphere_list[i].z += CAM_DIST;
                }
                break;

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

        switch (light_phase) {
            case 0:
                light_x += (LIGHT_ONE_X - LIGHT_ZERO_X) / LIGHT_STEP;
                light_y += (LIGHT_ONE_Y - LIGHT_ZERO_Y) / LIGHT_STEP;
                light_z += (LIGHT_ONE_Y - LIGHT_ZERO_Y) / LIGHT_STEP;

                if (light_x == LIGHT_ONE_X) {
                    light_phase = 1;
                }
                break;

            case 1:
                light_x += (LIGHT_TWO_X - LIGHT_ONE_X) / LIGHT_STEP;
                light_y += (LIGHT_TWO_Y - LIGHT_ONE_Y) / LIGHT_STEP;
                light_z += (LIGHT_TWO_Y - LIGHT_ONE_Y) / LIGHT_STEP;

                if (light_x == LIGHT_TWO_X) {
                    light_phase = 2;
                }
                break;

            case 2:
                light_x += (LIGHT_THREE_X - LIGHT_TWO_X) / LIGHT_STEP;
                light_y += (LIGHT_THREE_Y - LIGHT_TWO_Y) / LIGHT_STEP;
                light_z += (LIGHT_THREE_Y - LIGHT_TWO_Y) / LIGHT_STEP;

                if (light_x == LIGHT_THREE_X) {
                    light_phase = 3;
                }
                break;

            case 3:
                light_x += (LIGHT_ZERO_X - LIGHT_THREE_X) / LIGHT_STEP;
                light_y += (LIGHT_ZERO_Y - LIGHT_THREE_Y) / LIGHT_STEP;
                light_z += (LIGHT_ZERO_Y - LIGHT_THREE_Y) / LIGHT_STEP;

                if (light_x == LIGHT_ZERO_X) {
                    light_phase = 0;
                }
                break;

        }

    }

    return 0;
}