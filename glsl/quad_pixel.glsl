#version 330

//#define FLAG_SHADOWS
//#define FLAG_SOFT_SHADOWS
#define FLAG_AMBIENTOCCLUSION

in mat4 viewMatrix;
out vec4 outColor;

uniform vec2 iResolution; 
uniform float fieldOfView;
uniform float Time; 

const int MAX_MARCHING_STEPS = 255;
const float MIN_DIST = 0.0;
const float MAX_DIST = 50.0;
const float EPSILON = 0.005;
 
//Signed distance function for a sphere centered at the origin with radius 1.0;
float sphereSDF(vec3 point, float radius) {
    return length(point) - radius;
}

float planeSDF(vec3 point, vec4 normal) {
    normal = normalize(normal);
    return dot(point, normal.xyz) + normal.w;
}

float intersectSDF(float distA, float distB) {
    return max(distA, distB);
}

float unionSDF(float distA, float distB) {
    return min(distA, distB);
}

float differenceSDF(float distA, float distB) {
    return max(distA, -distB);
}

float sierpinskiTriangle(vec3 pos) {
    float t = Time;
/*
    const int FRACTAL_ITERATIONS = 16;
    float scale = 1.8+cos(Time)*0.18;
	float offset = 1.5;
	float x1,y1,z1;
	for(int n=0; n< FRACTAL_ITERATIONS; n++)
	{
        pos.xy = (pos.x+pos.y < 0.0) ? -pos.yx : pos.xy;
		pos.xz = (pos.x+pos.z < 0.0) ? -pos.zx : pos.xz;
		pos.zy = (pos.z+pos.y < 0.0) ? -pos.yz : pos.zy;
       
		pos = scale*pos-offset*(scale-1.0);
	}
 
	return length(pos) * pow(scale, -float(FRACTAL_ITERATIONS));
*/

    const vec3 v1 = vec3(-1.0f, -1.0f, -1.0f);
    const vec3 v2 = vec3(1.0f, 1.0f, -1.0f);
    const vec3 v3 = vec3(1.0f, -1.0f, 1.0f);
    const vec3 v4 = vec3(-1.0f, 1.0f, 1.0f);
    const int maxit = 15;
    const float scale = 2.0;

    for (int i = 0; i < maxit; ++i) {
        float d = distance(pos, v1);
        vec3 c = v1;
        
        float t = distance(pos, v2);
        if (t < d) { c = v2; d = t; }
        
        t = distance(pos, v3);
        if (t < d) { c = v3; d = t; }
        
        t = distance(pos, v4);
        if (t < d) { c = v4; d = t; }
        
        pos = (pos - c)*scale;
    }
    return length(pos) * pow(scale, float(-maxit)); 
    // return length(p) * pow(scale, float(-maxit)) - pixsize; // let the leaves be one pixel in size

/*
    int Iterations = 30;
    float Scale = 3.0f;
    float t = Time;

	vec3 a1 = vec3(1.0f, 1.0f, 1.0f);
	vec3 a2 = vec3(-1.0f, -1.0f, 1.0f);
	vec3 a3 = vec3(1.0f, -1.0f, -1.0f);
	vec3 a4 = vec3(-1.0f, 1.0f, -1.0f);
	vec3 c;
	int n = 0;
	float dist, d;
	while (n < Iterations) {
		c = a1; dist = length(z - a1);
	    d = length(z - a2); if (d < dist) { c = a2; dist = d; }
		d = length(z - a3); if (d < dist) { c = a3; dist = d; }
		d = length(z - a4); if (d < dist) { c = a4; dist = d; }
		z = Scale*z - c*(Scale - 1.0);
		n++;
	}
	return length(z)*pow(Scale, -n);
*/
}


float mandelbulbFractal(vec3 pos) {
    int Iterations = 8;
    float Bailout = 10.0f;
    float Power = 9.0*sin(Time / 50.0f);

	vec3 z = pos;
	float dr = 1.0;
	float r = 0.0;
	for (int i = 0; i < Iterations ; i++) {
		r = length(z);
		if (r > Bailout) break;
		
		// convert to polar coordinates
		float theta = acos(z.z/r);
		float phi = atan(z.y,z.x);
		dr =  pow(r, Power - 1.0)*Power*dr + 1.0;
		
		// scale and rotate the point
		float zr = pow(r, Power);
		theta = theta*Power;
		phi = phi*Power;
		
		// convert back to cartesian coordinates
		z = zr*vec3(sin(theta)*cos(phi), sin(phi)*sin(theta), cos(theta));
		z += pos;
	}
	return 0.5*log(r)*r/dr;
}


// Absolute value of the return value indicates the distance to the surface. 
// Sign indicates whether the point is inside or outside the surface, negative indicating inside.
float sceneSDF(vec3 point) {
    float time = Time;

    float t = sphereSDF(point - vec3(-3, 0, 0), 2.5);
    t = unionSDF(t, sphereSDF(point-vec3(3, 0, 0), 2.5));
    t = unionSDF(t, sphereSDF(point-vec3(0, 0, 10), 2.5));
    t = unionSDF(t, planeSDF(point, vec4(0, 1, 0, 5.5)));
    return t;
/*
    float t = sphereSDF(point-vec3(3,-2.5,10), 2.5);
    t = unionSDF(t, sphereSDF(point-vec3(-3, -2.5, 10), 2.5));
    t = unionSDF(t, sphereSDF(point-vec3(0, 2.0, 10), 2.5));
    t = unionSDF(t, planeSDF(point, vec4(0, 1, 0, 5.5)));
    return t;
*/

    //point.x = mod(point.x, 2.0f) - 1.0f;
    //point.z = mod(point.z, 2.0f) - 1.0f;
    //return mandelbulbFractal(point);

    //return sierpinskiTriangle(point);
}

/*
    Return the shortest distance from the eyepoint to the scene surface along
    the marching direction. If no part of the surface is found between start and end,
    return end.
  
    eye: the eye point, acting as the origin of the ray
    direction: the normalized direction to march in
    start: the starting distance away from the eye
    end: the max distance away from the eye to march before giving up
*/
float shortestDistanceToSurface(vec3 eye, vec3 direction, float start, float end) {
    float depth = start;
    for (int i = 0; i < MAX_MARCHING_STEPS; i++) {
        float dist = sceneSDF(eye + depth*direction);
        if (dist < EPSILON) {
			return depth; // We're inside the scene surface!
        }
        depth += dist; // Move along the view ray
        if (depth >= end) {
            return depth; // Gone too far; give up
        }
    }
    return depth;
}

/*
    Return the normalized direction to march in from the eye point for a single pixel.
 
    fieldOfView: vertical field of view in degrees
    size: resolution of the output image
    fragCoord: the x,y coordinate of the pixel in the output image
*/
vec3 rayDirection(float fieldOfView, vec2 size, vec2 fragCoord) {
    vec2 xy = fragCoord - size / 2.0;
    float z = size.y / tan(radians(fieldOfView) / 2.0);
    vec3 dir = xy.x*viewMatrix[0].xyz + xy.y*viewMatrix[1].xyz + z*viewMatrix[2].xyz;
    return normalize(dir);
}

//Compute the normal on the surface at point p, using the gradient of the SDF
vec3 computeNormal(vec3 p) {
    return normalize(vec3(
        sceneSDF(vec3(p.x + EPSILON, p.y, p.z)) - sceneSDF(vec3(p.x - EPSILON, p.y, p.z)),
        sceneSDF(vec3(p.x, p.y + EPSILON, p.z)) - sceneSDF(vec3(p.x, p.y - EPSILON, p.z)),
        sceneSDF(vec3(p.x, p.y, p.z + EPSILON)) - sceneSDF(vec3(p.x, p.y, p.z - EPSILON))
    ));
}

// https://www.iquilezles.org/www/articles/rmshadows/rmshadows.htm
// w is the size of the light source, and controls how hard/soft the shadows are
float softShadow(vec3 shadowRayOrigin, vec3 shadowRayDir, float start, float end, float w )
{ 
    float res = 1.0;
    float ph = 1e20;
    for(float t=start; t<end; ) {
        float h = sceneSDF(shadowRayOrigin + shadowRayDir*t);
        if( h<0.001 )
            return 0.0;
        float y = h*h/(2.0*ph);
        float d = sqrt(h*h - y*y);
        res = min(res, w*d/max(0.0, t - y) );
        ph = h;
        t += h;
    }
    return res;
}

// s is a scale variable determining the darkening effects of the occlusion
// http://www2.compute.dtu.dk/pubdb/views/edoc_download.php/6392/pdf/imm6392.pdf
float ambientOcclusion(vec3 point, vec3 normal, float step, float samples, float s) {
    float ao = 0.0f;
    float dist;
    for (float i = 1.0f; i <= samples; i += 1.0f) {
        dist = step*i;  
        ao = max( (dist - (sceneSDF(point + normal*dist))) / pow(2.0, i), ao); 
        //ao += max( (dist - (sceneSDF(point + normal*dist))) / pow(2.0, i), 0.0); 
        //ao += (dist - (sceneSDF(point + normal*dist))) / (dist*dist); 
    }
    return 1.0f - ao*s;
    //return clamp(1.0f - ao*s, 0.0, 1.0);
}

// Lighting contribution of a direction light source via Phong illumination.
vec4 PhongDirectionLight(vec3 ambientColor, vec3 diffuseColor, vec3 specularColor, float shininess, vec3 point, vec3 eye)
{
    const vec3 lightDirection = vec3(0.0f, -1.0f, 0.0f);
    /*
    const vec3 ambientLightColor = vec3(0.5, 0.5, 0.5); // интенсивность фонового света
    const vec3 diffuseLightColor = vec3(0.5, 0.5, 0.5); // интенсивность рассеянного света
    const vec3 specularLightColor = vec3(0.5, 0.5, 0.5); // интенсивность зеркального света
    */
    const vec3 ambientLightColor = vec3(1.0, 1.0, 1.0); // интенсивность фонового света
    const vec3 diffuseLightColor = vec3(1.0, 1.0, 1.0); // интенсивность рассеянного света
    const vec3 specularLightColor = vec3(1.0, 1.0, 1.0); // интенсивность зеркального света
    
    float shadow = 1.0;
     
    #ifdef FLAG_SHADOWS
        vec3 shadowRayOrigin = point + computeNormal(point)*0.01;
        vec3 shadowRayDir = normalize(vec3(-lightDirection)); 
        float dist = shortestDistanceToSurface(shadowRayOrigin, shadowRayDir, MIN_DIST, MAX_DIST);
        if (dist < MAX_DIST)
            return vec4(0.0, 0.0, 0.0, 1.0);
    #endif

    #ifdef FLAG_SOFT_SHADOWS
        vec3 shadowRayOrigin = point + computeNormal(point)*0.01;
        vec3 shadowRayDir = normalize(vec3(-lightDirection));
        shadow = softShadow(shadowRayOrigin, shadowRayDir, MIN_DIST, MAX_DIST, 3.0);
    #endif

    vec3 light_direction = normalize(vec3(-lightDirection)); // L для направленного
    vec3 inEye = normalize(eye - point); // V
    vec3 outNormal = computeNormal(point); // N
    vec3 reflected_light = reflect(-light_direction, outNormal); //R

    vec3 ambient = ambientLightColor*ambientColor;
    vec3 diffuse = diffuseLightColor*diffuseColor*max(dot(light_direction, outNormal), 0.0f)*shadow;
    vec3 specular = specularLightColor*specularColor*pow(max(dot(inEye, reflected_light), 0.0), shininess);
    vec3 color = ambient + diffuse + specular;

    #ifdef FLAG_AMBIENTOCCLUSION
        float ao = ambientOcclusion(point, outNormal, 2.5, 3.0, 0.5);
	    color *= ao;
    #endif

    return clamp(vec4(color, 1.0), 0.0f, 1.0f);;  
}

vec4 Lambert(vec3 color, vec3 dir_light, vec3 point) {
    vec3 directional_light = normalize(dir_light);
    vec3 outNormal = computeNormal(point); 
    float kd = clamp(dot(directional_light, outNormal), 0.0f, 1.0f);
    return kd*vec4(color, 1.0);
}

void main() {
    vec2 pixelCoord = vec2(gl_FragCoord.x, gl_FragCoord.y);
    vec3 dir = rayDirection(fieldOfView, iResolution, pixelCoord);
    vec3 eye = viewMatrix[3].xyz;
    float dist = shortestDistanceToSurface(eye, dir, MIN_DIST, MAX_DIST);
    
    // Didn't hit anything
    if (dist > MAX_DIST - EPSILON) {
        outColor = vec4(vec3(0.30, 0.36, 0.60) - (dir.y * 0.7), 1.0); // Skybox color
		return;
    }

    vec3 point = eye + dist*dir; // The closest point on the surface to the eyepoint along the view ray

    const vec3 ambientColor = vec3(0.19225, 0.19225, 0.19225); // отражение фонового света материалом
    const vec3 diffuseColor = vec3(0.50754, 0.50754, 0.50754); // отражение рассеянного света материалом
    const vec3 specularColor = vec3(0.50827, 0.50827, 0.50827); // отражение зеркального света материалом
    const float shininess = 2.0; // показатель степени зеркального отражения

    /*
    const vec3 ambientColor = vec3(1.0, 1.0, 1.0); // отражение фонового света материалом
    const vec3 diffuseColor = vec3(1.0, 1.0, 1.0); // отражение рассеянного света материалом
    const vec3 specularColor = vec3(1.0, 1.0, 1.0); // отражение зеркального света материалом
    const float shininess = 20.0; // показатель степени зеркального отражения
    */

    /*
    const vec3 ambientColor = vec3(0.2, 0.2, 0.2); // отражение фонового света материалом
    const vec3 diffuseColor = vec3(0.7, 0.2, 0.2); // отражение рассеянного света материалом
    const vec3 specularColor = vec3(1.0, 1.0, 1.0); // отражение зеркального света материалом
    const float shininess = 0.40; // показатель степени зеркального отражения
    */

    //outColor = Lambert(vec3(0.0, 1.0 , 0.0), vec3(0.0f, 1.0f, 1.0f), point);
    outColor = PhongDirectionLight(ambientColor, diffuseColor, specularColor, shininess, point, eye);
    //outColor = vec4(pow(outColor.xyz, vec3(1.0/2.2)), 1.0); // Gamma correction
} 