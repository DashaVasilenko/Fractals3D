//#version 330

//#define FLAG_SOFT_SHADOWS

in mat4 viewMatrix;
out vec4 outColor;

uniform vec2 iResolution; 
uniform float fieldOfView;

#if defined SKYBOX_BACKGROUND || defined SKYBOX_BACKGROUND_HDR
    uniform float backgroundBrightness;
    uniform samplerCube skyBox; 
#endif

#if defined SKYBOX_BACKGROUND_HDR && defined IRRADIANCE_CUBEMAP
    uniform samplerCube irradianceMap; 
#endif

#if defined SOLID_BACKGROUND || defined SOLID_BACKGROUND_WITH_SUN
    uniform vec3 reflectedColor;
#endif

#ifdef SOLID_BACKGROUND_WITH_SUN
    uniform vec3 sunColor;
#endif

uniform float Time;
uniform int antiAliasing;

uniform vec3 lightDirection1;
uniform vec3 lightColor1;
uniform float lightIntensity1;

uniform vec3 lightDirection2;
uniform vec3 lightColor2;
uniform float lightIntensity2;

uniform vec3 ambientLightColor3;
uniform float ambientLightIntensity3;

#if defined COLORING_TYPE_1 || defined COLORING_TYPE_3 || defined COLORING_TYPE_4 || defined COLORING_TYPE_6
uniform vec3 color;
#endif

#ifdef COLORING_TYPE_3
uniform float coef;
#endif

#if defined COLORING_TYPE_2 || defined COLORING_TYPE_5 || defined COLORING_TYPE_7
uniform vec3 color1;
uniform vec3 color2;
uniform vec3 color3;
#endif

uniform float shininess; // показатель степени зеркального отражения
uniform float reflection; // сила отражения
uniform float shadowStrength;
uniform vec3 exposure;

const int MAX_MARCHING_STEPS = 255;
const float MIN_DIST = 0.0;
const float MAX_DIST = 50.0;
const float EPSILON = 0.005;

vec2 hash2( float n ){
    return fract(sin(vec2(n, n+1.0))*vec2(13.5453123, 31.1459123));
}

vec3 hash3( float n ) {
    return fract(sin(vec3(n, n+1.0, n+2.0))*vec3(43758.5453123, 22578.1459123, 19642.3490423));
}

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

//-------------------------------------------------------------------------------------------------------
// Absolute value of the return value indicates the distance to the surface. 
// Sign indicates whether the point is inside or outside the surface, negative indicating inside.
float sceneSDF(vec3 point, out vec4 resColor) {
    float time = Time;
    float m = dot(point, point);

#if defined COLORING_TYPE_1 || defined COLORING_TYPE_2 || defined COLORING_TYPE_4 || defined COLORING_TYPE_5 || defined COLORING_TYPE_7
    vec4 trap = vec4(abs(point), m);
#endif

#if defined COLORING_TYPE_3 || defined COLORING_TYPE_6
    vec2  trap = vec2(1e10);
#endif

    float t = sphereSDF(point - vec3(-3, 0, 0), 2.5);
    t = unionSDF(t, sphereSDF(point - vec3(3, 0, 0), 2.5));
    t = unionSDF(t, sphereSDF(point - vec3(0, 0, 10), 2.5));
    t = unionSDF(t, planeSDF(point, vec4(0, 1, 0, 5.5)));

#if defined COLORING_TYPE_1 || defined COLORING_TYPE_2 || defined COLORING_TYPE_4 || defined COLORING_TYPE_5 || defined COLORING_TYPE_7
    resColor = vec4(m, trap.yzw); // trapping Oxz, Oyz, Oxy, (0,0,0)
#endif

#if defined COLORING_TYPE_3 || defined COLORING_TYPE_6
    trap = min(trap, vec2(m, abs(point.x))); // orbit trapping ( |z|² and z_x  )
    resColor = vec4(trap, 1.0, 1.0);
#endif

    return t;
}

//-------------------------------------------------------------------------------------------------------
// Return the shortest distance from the eyepoint to the scene surface along the marching direction. 
// If no part of the surface is found between start and end, return end.
// 
// eye: the eye point, acting as the origin of the ray
// direction: the normalized direction to march in
// start: the starting distance away from the eye
// end: the max distance away from the eye to march before giving up
float shortestDistanceToSurface(vec3 eye, vec3 direction, float start, float end, out vec4 trapColor) {
    vec4 trap;
    float depth = start;
    float res = end;

    for (int i = 0; i < MAX_MARCHING_STEPS; i++) {
        float h = sceneSDF(eye + depth*direction, trap);
        if (h < EPSILON) break; // We're inside the scene surface!
        if (depth >= end) break; // Gone too far; give up
        depth += h; // Move along the view ray
    }
    
    if (depth < end) { 
        trapColor = trap;
        res = depth;
    }
    return res;
}

//-------------------------------------------------------------------------------------------------------
// Return the normalized direction to march in from the eye point for a single pixel.
// 
// fieldOfView: vertical field of view in degrees
// size: resolution of the output image
// fragCoord: the x,y coordinate of the pixel in the output image
vec3 rayDirection(float fieldOfView, vec2 size, vec2 fragCoord) {
    vec2 xy = fragCoord - size / 2.0;
    float z = size.y/tan(radians(fieldOfView)/ 2.0);
    vec3 dir = xy.x*viewMatrix[0].xyz + xy.y*viewMatrix[1].xyz + z*viewMatrix[2].xyz;
    return normalize(dir);
}

//-------------------------------------------------------------------------------------------------------
//Compute the normal on the surface at point p, using the gradient of the SDF
vec3 computeNormal(vec3 p) {
    vec4 t;
    return normalize(vec3(
        sceneSDF(vec3(p.x + EPSILON, p.y, p.z), t) - sceneSDF(vec3(p.x - EPSILON, p.y, p.z), t),
        sceneSDF(vec3(p.x, p.y + EPSILON, p.z), t) - sceneSDF(vec3(p.x, p.y - EPSILON, p.z), t),
        sceneSDF(vec3(p.x, p.y, p.z + EPSILON), t) - sceneSDF(vec3(p.x, p.y, p.z - EPSILON), t)
    ));
}

//-------------------------------------------------------------------------------------------------------
// w is the size of the light source, and controls how hard/soft the shadows are
// https://www.iquilezles.org/www/articles/rmshadows/rmshadows.htm
float softShadow(vec3 shadowRayOrigin, vec3 shadowRayDir, float start, float end, float w ) { 
    float res = 1.0;
    vec4 trap;
    float iterations = 64;
    for (float t = start; t < end; iterations--) {
        float h = sceneSDF(shadowRayOrigin + shadowRayDir*t, trap);
        //if (h < 0.0001 ) return 0.0;
        res = min(res, w*h/t);
        if (res < 0.001 || iterations <= 0) break;
        //if (iterations <= 0) break;
        t += h;
    }
    return clamp(res, 0.0, 1.0);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec4 Render(vec3 eye, vec3 dir, vec2 sp) {
    float s = shadowStrength;
    vec4 trap;  
    float dist = shortestDistanceToSurface(eye, dir, MIN_DIST, MAX_DIST, trap); // intersect fractal

    vec3 point = eye + dist*dir; // The closest point on the surface to the eyepoint along the view ray
    vec3 outNormal = computeNormal(point); // N

    // Didn't hit anything. sky color
    if (dist > MAX_DIST - EPSILON) {
#if defined SKYBOX_BACKGROUND || defined SKYBOX_BACKGROUND_HDR
        return texture(skyBox, dir)*backgroundBrightness;
#endif

#ifdef SOLID_BACKGROUND
        return vec4(reflectedColor - (dir.y*0.7), 1.0); // Skybox color
#endif

#ifdef SOLID_BACKGROUND_WITH_SUN
        vec3 col  = reflectedColor*(0.6 + 0.4*dir.y); 
        col += lightIntensity1*sunColor*pow(clamp(dot(dir, lightDirection1),0.0,1.0), 32.0); 
        return vec4(col, 1.0);
#endif
    }
    // color fractal
	else {
        // lighting terms
        vec3 hal = normalize(lightDirection1 - dir);
        float occlusion = clamp(0.05*log(trap.x), 0.0, 1.0);
        float shadow = 1.0;
        
        // main color
    #ifdef COLORING_TYPE_1
        vec3 albedo = color;
        albedo *= 0.1;
    #endif
    #ifdef COLORING_TYPE_2
        vec3 albedo = vec3(0.001); // чем больше значение, тем более засвеченный фрактал
        albedo = mix(albedo, color1, clamp(trap.y, 0.0, 1.0));
	    albedo = mix(albedo, color2, clamp(trap.z*trap.z, 0.0, 1.0));
        albedo = mix(albedo, color3, clamp(pow(trap.w, 6.0), 0.0, 1.0));
        albedo *= 0.5;
    #endif
    #ifdef COLORING_TYPE_3
        vec3 albedo = color + color*sin(trap.y*coef + coef + color + outNormal*0.2).xzy;
    #endif
    #ifdef COLORING_TYPE_4
        vec3 albedo = color;
        albedo *= 0.1;
        albedo.x = 1.0 - 10.0*trap.x; 
    #endif
    #ifdef COLORING_TYPE_5
        vec3 albedo = vec3(0.0);
        albedo = mix(albedo, color1, sqrt(trap.x));
		albedo = mix(albedo, color2, sqrt(trap.y));
		albedo = mix(albedo, color3, trap.z);
        albedo *= 0.09;
    #endif   
    #ifdef COLORING_TYPE_6
        vec3 albedo = color + 0.5*cos(6.2831*trap.x + color);
    #endif
    #ifdef COLORING_TYPE_7
        vec3 albedo = color1;
        albedo = mix(albedo, color2, clamp(6.0*trap.y, 0.0, 1.0));
        albedo = mix(albedo, color3, pow(clamp(1.0 - 2.0*trap.z, 0.0, 1.0), 8.0));
    #endif
		
    #ifdef FLAG_SOFT_SHADOWS
        vec3 shadowRayOrigin = point + 0.001*outNormal;
        vec3 shadowRayDir = normalize(lightDirection1); // луч, направленный на источник света
        shadow = softShadow(shadowRayOrigin, shadowRayDir, MIN_DIST, MAX_DIST, shadowStrength);
    #endif

        float dif1 = clamp(dot(lightDirection1, outNormal), 0.0, 1.0 )*shadow; // sun
        float spe1 = pow(clamp(dot(outNormal, hal), 0.0, 1.0), shininess)*dif1*(0.04 + 0.96*pow(clamp(dot(dir, lightDirection1), 0.0, 1.0), 5.0));
        float dif2 = clamp(0.5 + 0.5*dot(lightDirection2, outNormal), 0.0, 1.0)*occlusion; // bounce
        
		vec3 lin = vec3(0.0); 
		     lin += lightIntensity1*lightColor1*dif1; // sun
		     lin += lightIntensity2*lightColor2*dif2; //light1
             lin +=  ambientLightIntensity3*ambientLightColor3*(0.05+0.95*occlusion); // ambient light
		vec3 col = albedo*lin;
		col = pow(col, vec3(0.7, 0.9, 1.0));
        col += spe1*lightIntensity1;
        col = exp(-1.0/(2.72*col + 0.15)); // tone mapping

    #ifdef TONE_MAPPING
        // luma based Reinhard tone mapping
	    float luma = dot(col, exposure);
	    float toneMappedLuma = luma/(1.0 + luma);
	    col *= toneMappedLuma/luma;
    #endif

        // sky
        vec4 color; 
        float intensity = (lightIntensity1 + lightIntensity2 + ambientLightIntensity3)*0.1;
    #if defined SKYBOX_BACKGROUND || defined SKYBOX_BACKGROUND_HDR
        vec3 reflected_dir = reflect(dir, outNormal); //R
        vec4 reflected_color = texture(skyBox, reflected_dir);
        color = vec4(col, 1.0)*(1.0 - reflection) + reflected_color*reflection;
    #endif
    #if defined SOLID_BACKGROUND || defined SOLID_BACKGROUND_WITH_SUN
        color = vec4(col, 1.0)*(1.0 - reflection) + vec4(reflectedColor, 1.0)*reflection;
    #endif
        color *= intensity;

    #if defined SKYBOX_BACKGROUND_HDR && defined IRRADIANCE_CUBEMAP
        vec3 inEye = normalize(eye - point); // V
        // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
        // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
        vec3 F0 = vec3(0.04); 
        // ambient lighting (we now use IBL as the ambient term)
        vec3 kS = fresnelSchlick(max(dot(outNormal, inEye), 0.0), F0);
        vec3 kD = 1.0 - kS;
        vec3 irradiance = texture(irradianceMap, outNormal).rgb;
        vec3 diffuseIBL = irradiance*albedo;
        vec3 ambientIBL = (kD*diffuseIBL)*occlusion;
        color.xyz += ambientIBL; 
    #endif

        color = sqrt(color); // gamma
        color *= 1.0 - 0.05*length(sp); // vignette
        return color;
    }
}

void main() {
    vec2 pixelCoord = vec2(gl_FragCoord.x, gl_FragCoord.y);
    //vec3 dir = rayDirection(fieldOfView, iResolution, pixelCoord);
    vec3 eye = viewMatrix[3].xyz;
    vec2  sp = (2.0*pixelCoord-iResolution.xy) / iResolution.y;

    vec4 col = vec4(0.0);
    for( int i = 0; i < antiAliasing; i++ ) {
        for( int j = 0; j < antiAliasing; j++ ) {
            vec2 pixel = pixelCoord + (vec2(i,j)/float(antiAliasing));
            vec3 dir = rayDirection(fieldOfView, iResolution, pixel);
	        col += Render(eye, dir, sp);
        }
    }
	col /= float(antiAliasing*antiAliasing);
    outColor = col;
}