#version 330

#define FLAG_SHADOWS
//#define FLAG_SOFT_SHADOWS

in mat4 viewMatrix;
out vec4 outColor;

uniform vec2 iResolution; 

const int MAX_MARCHING_STEPS = 255;
const float MIN_DIST = 0.0;
const float MAX_DIST = 100.0;
const float EPSILON = 0.0001;
 
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

// Absolute value of the return value indicates the distance to the surface. 
// Sign indicates whether the point is inside or outside the surface, negative indicating inside.
float sceneSDF(vec3 point) {
    //float sphereDist = sphereSDF(point / 1.2) * 1.2;
    //float cubeDist = cubeSDF(point);
    //return intersectSDF(cubeDist, sphereDist);
    //return sphereSDF(point, 1.0);

    float t = sphereSDF(point-vec3(3,-2.5,10), 2.5);
    t = unionSDF(t, sphereSDF(point-vec3(-3, -2.5, 10), 2.5));
    t = unionSDF(t, sphereSDF(point-vec3(0, 2.5, 10), 2.5));
    t = unionSDF(t, planeSDF(point, vec4(0, 1, 0, 5.5)));
    return t;
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

//Compute the normal on the surface at point p, using the gradient of the SDF, 
vec3 computeNormal(vec3 p) {
    return normalize(vec3(
        sceneSDF(vec3(p.x + EPSILON, p.y, p.z)) - sceneSDF(vec3(p.x - EPSILON, p.y, p.z)),
        sceneSDF(vec3(p.x, p.y + EPSILON, p.z)) - sceneSDF(vec3(p.x, p.y - EPSILON, p.z)),
        sceneSDF(vec3(p.x, p.y, p.z + EPSILON)) - sceneSDF(vec3(p.x, p.y, p.z - EPSILON))
    ));
}

// https://www.iquilezles.org/www/articles/rmshadows/rmshadows.htm
// w is the size of the light source, and controls how hard/soft the shadows are
float softshadow(vec3 shadowRayOrigin, vec3 shadowRayDir, float start, float end, float w )
{ 
    float res = 1.0;
    float ph = 1e20;
    for( float t=start; t<end; )
    {
        float h = sceneSDF(shadowRayOrigin + shadowRayDir*t);
        if( h<0.001 )
            return 0.0;
        float y = h*h/(2.0*ph);
        float d = sqrt(h*h-y*y);
        res = min( res, w*d/max(0.0,t-y) );
        ph = h;
        t += h;
    }
    return res;
}

// Lighting contribution of a direction light source via Phong illumination.
vec4 PhongDirectionLight(vec3 ambientColor, vec3 diffuseColor, vec3 specularColor, float shininess, vec3 point, vec3 eye)
{
    // направленный источник света
    const vec3 lightDirection = vec3(0.0f, -1.0f, 0.0f);
    const vec3 ambientLightColor = vec3(0.5, 0.5, 0.5); // интенсивность фонового света
    const vec3 diffuseLightColor = vec3(0.5, 0.5, 0.5); // интенсивность рассеянного света
    const vec3 specularLightColor = vec3(0.5, 0.5, 0.5); // интенсивность зеркального света

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
        shadow = softshadow(shadowRayOrigin, shadowRayDir, MIN_DIST, MAX_DIST, 3.0);
    #endif

    vec3 light_direction = normalize(vec3(-lightDirection)); // L для направленного
    vec3 inEye = normalize(eye - point); // V
    vec3 outNormal = computeNormal(point); // N
    vec3 reflected_light = reflect(-light_direction, outNormal); //R

    vec3 ambient = ambientLightColor*ambientColor;
    vec3 diffuse = diffuseLightColor*diffuseColor*max(dot(light_direction, outNormal), 0.0f)*shadow;
    vec3 specular = specularLightColor*specularColor*pow(max(dot(inEye, reflected_light), 0.0), shininess);
    return clamp(vec4(ambient + diffuse + specular, 1.0), 0.0f, 1.0f);
    //return vec4(ambient + diffuse + specular, 1.0);  
}

vec4 Lambert(vec3 color, vec3 dir_light, vec3 point) {
    vec3 directional_light = normalize(dir_light);
    vec3 outNormal = computeNormal(point); 
    float kd = clamp(dot(directional_light, outNormal), 0.0f, 1.0f);
    return kd*vec4(color, 1.0);
}

void main() {
    vec2 pixelCoord = vec2(gl_FragCoord.x, gl_FragCoord.y);
    vec3 dir = rayDirection(45.0, iResolution, pixelCoord);
    vec3 eye = viewMatrix[3].xyz;
    float dist = shortestDistanceToSurface(eye, dir, MIN_DIST, MAX_DIST);
    
    // Didn't hit anything
    if (dist > MAX_DIST - EPSILON) {
        outColor = vec4(vec3(0.30, 0.36, 0.60) - (dir.y * 0.7), 1.0); // Skybox color
		return;
    }

    vec3 point = eye + dist * dir; // The closest point on the surface to the eyepoint along the view ray

    const vec3 ambientColor = vec3(0.19225, 0.19225, 0.19225); // отражение фонового света материалом
    const vec3 diffuseColor = vec3(0.50754, 0.50754, 0.50754); // отражение рассеянного света материалом
    const vec3 specularColor = vec3(0.50827, 0.50827, 0.50827); // отражение зеркального света материалом
    const float shininess = 2.0; // показатель степени зеркального отражения

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