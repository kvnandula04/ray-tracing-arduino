#include <math.h>
#include <VGAX.h>

VGAX vga;

#define INF_DIST 1000
#define CAM_DIST 50 // This means we are looking backwards on x - bottom right is (-ve x -ve y)
#define MOVE_DIST 2

#define SCREENWIDTH 120
#define SCREENHEIGHT 60

#define HALF_WIDTH (SCREENWIDTH / 2)
#define HALF_HEIGHT (SCREENHEIGHT / 2)

#define num_spheres 1

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

char light_phase = 2;

char direction;

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
//  y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

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



void print_pixel(sphere * sphere_list, int x, int y) {
    int out = INF_DIST;
    int result;

    int temp_x = HALF_WIDTH - x;
    int temp_y = HALF_HEIGHT - y;
    int z = CAM_DIST;

//    Scale the vectors to unit vectors
    float scale = Q_rsqrt(temp_x*temp_x + temp_y*temp_y + z*z);
    float
    vec_x = temp_x*scale,    
    vec_y = temp_y*scale,    
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
//        Serial.print(" ");
        vga.putpixel(x, y, 3);
    } else {
//        printf("1");
        int br = 15 * get_brightness(out*vec_x, out*vec_y, out*vec_z, closest) / 15;
//        Serial.print(br);
        vga.putpixel(x, y, br);
//        printf("%d", out);
    }
}

static inline void display(sphere* sphere_list) {
    for (int y = 0; y < SCREENHEIGHT; y++) {
        for (int x = 0; x < SCREENWIDTH; x++) {
            print_pixel(sphere_list, x, y);
        }
//        Serial.print("\n");
    }
//    Serial.print("\n");
}



sphere sphere_list[num_spheres];

void setup() {
  // put your setup code here, to run once: 
  Serial.begin(4800);
  vga.begin();
  vga.clear(0);
  pinMode(11, OUTPUT);
  pinMode(12, OUTPUT);

  // No time for mallocs, just use the number twice
  sphere_list[0] = create_sphere(20, -11, 60, 15);
//  sphere_list[1] = create_sphere(30, -5, 70, 10);
//  sphere_list[2] = create_sphere(15, -4, 90, 15);
//  sphere_list[3] = create_sphere(20, -10, 0, 30);
//  sphere_list[4] = create_sphere(0, 10, -10, 15);
//  sphere_list[5] = create_sphere(-30, 30, -60, 30);
//  sphere_list[6] = create_sphere(30, 30, -120, 80);

  display(sphere_list);
}

void loop() {
  // put your main code here, to run repeatedly:
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