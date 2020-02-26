#version 330

out vec4 outColor;

uniform vec2 iResolution; 

const int MAX_MARCHING_STEPS = 255;
const float MIN_DIST = 0.0;
const float MAX_DIST = 100.0;
const float EPSILON = 0.0001;
 
//Signed distance function for a sphere centered at the origin with radius 1.0;
float sphereSDF(vec3 point) {
    return length(point) - 1.0;
}

/*
    Signed distance function describing the scene. 
    Absolute value of the return value indicates the distance to the surface.
    Sign indicates whether the point is inside or outside the surface, negative indicating inside.
 */
float sceneSDF(vec3 point) {
    //float sphereDist = sphereSDF(point / 1.2) * 1.2;
    //float cubeDist = cubeSDF(point);
    //return intersectSDF(cubeDist, sphereDist);
    return sphereSDF(point);
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
        float dist = sceneSDF(eye + depth * direction);
        if (dist < EPSILON) {
			return depth; // We're inside the scene surface!
        }
        depth += dist; // Move along the view ray
        if (depth >= end) {
            return end; // Gone too far; give up
        }
    }
    return end;
}

/*
    Return the normalized direction to march in from the eye point for a single pixel.
 
    fieldOfView: vertical field of view in degrees
    size: resolution of the output image
    fragCoord: the x,y coordinate of the pixel in the output image
 */
vec3 rayDirection(float fieldOfView, vec2 size, vec2 fragCoord) {
    vec2 xy = fragCoord - size / 2.0;
    //vec2 xy = 2.0 * (fragCoord/iResolution.xy - 0.5);
    float z = size.y / tan(radians(fieldOfView) / 2.0);
    return normalize(vec3(xy, -z));
}

//Compute the normal on the surface at point p, using the gradient of the SDF, 
vec3 computeNormal(vec3 p) {
    return normalize(vec3(
        sceneSDF(vec3(p.x + EPSILON, p.y, p.z)) - sceneSDF(vec3(p.x - EPSILON, p.y, p.z)),
        sceneSDF(vec3(p.x, p.y + EPSILON, p.z)) - sceneSDF(vec3(p.x, p.y - EPSILON, p.z)),
        sceneSDF(vec3(p.x, p.y, p.z + EPSILON)) - sceneSDF(vec3(p.x, p.y, p.z - EPSILON))
    ));
}

/*
    Lighting contribution of a single point light source via Phong illumination.

    The vec3 returned is the RGB color of the light's contribution.

    k_a: Ambient color
    k_d: Diffuse color
    k_s: Specular color
    alpha: Shininess coefficient
    p: position of point being lit
    eye: the position of the camera
    lightPos: the position of the light
    lightIntensity: color/intensity of the light
 */
vec3 phongContribForLight(vec3 k_a, vec3 k_d, vec3 k_s, float shininess, vec3 p, vec3 eye)
{
    const vec3 ambientLight = 0.5 * vec3(1.0, 1.0, 1.0);
    vec3 color = ambientLight * k_a;

    vec3 lightPos = vec3(4.0, 2.0, 4.0);
    vec3 lightIntensity = vec3(0.4, 0.4, 0.4);

    vec3 N = computeNormal(p);
    vec3 L = normalize(lightPos - p);
    vec3 V = normalize(eye - p);
    vec3 R = normalize(reflect(-L, N));
    
    float dotLN = dot(L, N);
    float dotRV = dot(R, V);
    
    if (dotLN < 0.0) {
        // Light not visible from this point on the surface
        return vec3(0.0, 0.0, 0.0);
    } 
    
    if (dotRV < 0.0) {
        // Light reflection in opposite direction as viewer, apply only diffuse
        // component
        return lightIntensity * (k_d * dotLN);
    }
    return color + lightIntensity * (k_d * dotLN + k_s * pow(dotRV, shininess));
}

void main() {

    vec2 pixelCoord = vec2(gl_FragCoord.x, gl_FragCoord.y);

    vec3 dir = rayDirection(45.0, iResolution, pixelCoord);
    vec3 eye = vec3(0.0, 0.0, 5.0);
    float dist = shortestDistanceToSurface(eye, dir, MIN_DIST, MAX_DIST);
    
    if (dist > MAX_DIST - EPSILON) {
        // Didn't hit anything
        outColor = vec4(0.0, 0.0, 0.0, 0.0);
		return;
    }

    // The closest point on the surface to the eyepoint along the view ray
    vec3 p = eye + dist * dir;

    vec3 K_a = vec3(0.2, 0.2, 0.2);
    vec3 K_d = vec3(0.7, 0.2, 0.2);
    vec3 K_s = vec3(1.0, 1.0, 1.0);
    float shininess = 10.0;

    vec3 color = phongContribForLight(K_a, K_d, K_s, shininess, p, eye);
    
    outColor = vec4(color, 1.0);
} 