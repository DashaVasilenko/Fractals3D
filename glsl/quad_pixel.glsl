#version 330

in mat4 viewMatrix;
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

// Absolute value of the return value indicates the distance to the surface. 
// Sign indicates whether the point is inside or outside the surface, negative indicating inside.
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
    float z = size.y / tan(radians(fieldOfView) / 2.0);
    return normalize(vec3(vec4(normalize(vec3(xy, -z)), 0.0)*viewMatrix));
}

//Compute the normal on the surface at point p, using the gradient of the SDF, 
vec3 computeNormal(vec3 p) {
    return normalize(vec3(
        sceneSDF(vec3(p.x + EPSILON, p.y, p.z)) - sceneSDF(vec3(p.x - EPSILON, p.y, p.z)),
        sceneSDF(vec3(p.x, p.y + EPSILON, p.z)) - sceneSDF(vec3(p.x, p.y - EPSILON, p.z)),
        sceneSDF(vec3(p.x, p.y, p.z + EPSILON)) - sceneSDF(vec3(p.x, p.y, p.z - EPSILON))
    ));
}

// Lighting contribution of a single point light source via Phong illumination.
vec4 PhongPointLight(vec3 ambientColor, vec3 diffuseColor, vec3 specularColor, float shininess, vec3 point, vec3 eye)
{
    //const vec3 lightPosition = vec3(0.0f, 1.0f, 0.0f);
    const vec3 lightPosition = vec3(4.0f, 2.0f, 4.0f);
    const vec3 ambientLightColor = vec3(0.5, 0.5, 0.5); // интенсивность фонового света
    const vec3 diffuseLightColor = vec3(0.5, 0.5, 0.5); // интенсивность рассеянного света
    const vec3 specularLightColor = vec3(0.5, 0.5, 0.5); // интенсивность зеркального света

    vec3 light_direction = normalize(vec3(lightPosition-point)); // L направление на источник света
    vec3 inEye = normalize(eye - point); // V
    vec3 outNormal = computeNormal(point); // N
    vec3 reflected_light = reflect(-light_direction, outNormal); //R

    vec3 ambient = ambientLightColor*ambientColor;
    vec3 diffuse = diffuseLightColor*diffuseColor*max(dot(light_direction, outNormal), 0.0f);
    vec3 specular = specularLightColor*specularColor*pow(max(dot(inEye, reflected_light), 0.0), shininess);
    return clamp(vec4(ambient + diffuse + specular, 1.0), 0.0f, 1.0f);
}

void main() {
    vec2 pixelCoord = vec2(gl_FragCoord.x, gl_FragCoord.y);
    vec3 dir = rayDirection(45.0, iResolution, pixelCoord);
    vec3 eye = -viewMatrix[3].xyz;
    float dist = shortestDistanceToSurface(eye, dir, MIN_DIST, MAX_DIST);
    
    if (dist > MAX_DIST - EPSILON) {
        // Didn't hit anything
        outColor = vec4(0.0, 0.0, 0.0, 0.0);
		return;
    }

    // The closest point on the surface to the eyepoint along the view ray
    vec3 point = eye + dist * dir;

    const vec3 ambientColor = vec3(0.19225, 0.19225, 0.19225); // отражение фонового света материалом
    const vec3 diffuseColor = vec3(0.50754, 0.50754, 0.50754); // отражение рассеянного света материалом
    const vec3 specularColor = vec3(0.50827, 0.50827, 0.50827); // отражение зеркального света материалом
    const float shininess = 0.40; // показатель степени зеркального отражения

    /*
    const vec3 ambientColor = vec3(0.2, 0.2, 0.2); // отражение фонового света материалом
    const vec3 diffuseColor = vec3(0.7, 0.2, 0.2); // отражение рассеянного света материалом
    const vec3 specularColor = vec3(1.0, 1.0, 1.0); // отражение зеркального света материалом
    const float shininess = 0.40; // показатель степени зеркального отражения
    */

    outColor = PhongPointLight(ambientColor, diffuseColor, specularColor, shininess, point, eye);
} 