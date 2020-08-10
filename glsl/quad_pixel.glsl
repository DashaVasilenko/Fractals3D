//#version 330

//#define FLAG_SOFT_SHADOWS

in mat4 viewMatrix;
out vec4 outColor;

uniform vec2 iResolution; 
uniform float fieldOfView;

#if defined SKYBOX_BACKGROUND || defined SKYBOX_BACKGROUND_HDR
    uniform samplerCube skyBox; // сэмплер для кубической карты
#endif

#if defined SKYBOX_BACKGROUND_HDR && defined IRRADIANCE_CUBEMAP
//#ifdef IRRADIANCE_CUBEMAP
    uniform samplerCube irradianceMap; // освещенность из кубмапы
#endif

#ifdef SOLID_BACKGROUND
    uniform vec3 reflectedColor;
#endif

uniform float Time;

uniform float shadowStrength;

uniform vec3 lightDirection1;
uniform vec3 lightColor1;
uniform float lightIntensity1;

uniform vec3 lightDirection2;
uniform vec3 lightColor2;
uniform float lightIntensity2;

uniform vec3 ambientLightColor3;
uniform float ambientLightIntensity3;

uniform vec3 color;
uniform float shininess; // показатель степени зеркального отражения
uniform float reflection; // сила отражения

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

//-------------------------------------------------------------------------------------------------------
// Absolute value of the return value indicates the distance to the surface. 
// Sign indicates whether the point is inside or outside the surface, negative indicating inside.
float sceneSDF(vec3 point, out vec4 resColor) {
    float time = Time;
    float m = dot(point, point);
    vec4 trap = vec4(abs(point), m);

#ifdef TEST
    float t = sphereSDF(point - vec3(-3, 0, 0), 2.5);
    t = unionSDF(t, sphereSDF(point-vec3(3, 0, 0), 2.5));
    t = unionSDF(t, sphereSDF(point-vec3(0, 0, 10), 2.5));
    t = unionSDF(t, planeSDF(point, vec4(0, 1, 0, 5.5)));
    resColor = vec4(m,trap.yzw);
    return t;
#endif

    return 0;
    //return sierpinskiTriangle(point);
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
    float z = size.y / tan(radians(fieldOfView) / 2.0);
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
    for(float t=start; t<end; iterations--) {
        float h = sceneSDF(shadowRayOrigin + shadowRayDir*t, trap);
        //if (h < 0.0001 ) return 0.0;
        res = min( res, w*h/t );
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
        return texture(skyBox, dir);
#endif

#ifdef SOLID_BACKGROUND
        return vec4(reflectedColor - (dir.y * 0.7), 1.0); // Skybox color
        //vec3 col  = vec3(0.8,0.9,1.1)*(0.6+0.4*dir.y);
		//col += 5.0*vec3(0.8,0.7,0.5)*pow( clamp(dot(dir,light1),0.0,1.0), 32.0 );
        //return vec4(col, 1.0);
#endif
    }
    // color fractal
	else {
        // lighting terms
        vec3 hal = normalize(lightDirection1 - dir);
        float occlusion = clamp(0.05*log(trap.x), 0.0, 1.0);
        float shadow = 1.0;
        
        // main color
        vec3 albedo = color;
        //vec3 albedo = vec3(0.001); // чем больше значение, тем более засвеченный фрактал
        //albedo = mix(albedo, color1, clamp(trap.y, 0.0, 1.0));
	 	//albedo = mix(albedo, color2, clamp(trap.z*trap.z, 0.0, 1.0));
        //albedo = mix(albedo, color3, clamp(pow(trap.w, 6.0), 0.0, 1.0));
        //albedo *= 0.5;
        
    #ifdef FLAG_SOFT_SHADOWS
        vec3 shadowRayOrigin = point + 0.001*outNormal;
        vec3 shadowRayDir = normalize(lightDirection1); // луч, направленный на источник света
        shadow = softShadow(shadowRayOrigin, shadowRayDir, MIN_DIST, MAX_DIST, shadowStrength);
    #endif

        // sun
        float dif1 = clamp(dot(lightDirection1, outNormal), 0.0, 1.0 )*shadow;
        float spe1 = pow(clamp(dot(outNormal, hal), 0.0, 1.0), shininess)*dif1*(0.04 + 0.96*pow(clamp(dot(dir, lightDirection1), 0.0, 1.0), 5.0));
        // bounce
        float dif2 = clamp(0.5 + 0.5*dot(lightDirection2, outNormal), 0.0, 1.0)*occlusion;
        // sky
        //float dif3 = (0.7+0.3*outNormal.y)*(0.2+0.8*occlusion);
        
		vec3 lin = vec3(0.0); 
		     lin += lightIntensity1*lightColor1*dif1; // sun
		     lin += lightIntensity2*lightColor2*dif2; //light1
        	 //lin +=  1.5*vec3(0.10,0.20,0.30)*dif3;
             lin +=  ambientLightIntensity3*ambientLightColor3*(0.05+0.95*occlusion); // ambient light
		vec3 col = albedo*lin;
		col = pow(col, vec3(0.7, 0.9, 1.0));
        col += spe1*15.0;

    #if defined SKYBOX_BACKGROUND_HDR && defined IRRADIANCE_CUBEMAP
        vec3 inEye = normalize(eye - point); // V
        // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
        // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
        vec3 F0 = vec3(0.04); 
        //F0 = mix(F0, albedo, metallic);
        // ambient lighting (we now use IBL as the ambient term)
        vec3 kS = fresnelSchlick(max(dot(outNormal, inEye), 0.0), F0);
        vec3 kD = 1.0 - kS;
        //kD *= 1.0 - metallic;	  
        vec3 irradiance = texture(irradianceMap, outNormal).rgb;
        //vec3 diffuse      = irradiance * albedo;
        vec3 diffuseIBL      = irradiance * albedo;
        //vec3 ambient = (kD * diffuse) * ao;
        vec3 ambientIBL = (kD * diffuseIBL) * occlusion;

        col += ambientIBL; 
    #endif

        vec4 color;
        // sky
    #if defined SKYBOX_BACKGROUND || defined SKYBOX_BACKGROUND_HDR
        vec3 reflected_dir = reflect(dir, outNormal); //R
        vec4 reflected_color = texture(skyBox, reflected_dir);
        color = vec4(col, 1.0)*(1.0 - reflection) + reflected_color*reflection;
    #endif

    #ifdef SOLID_BACKGROUND
        color = vec4(col, 1.0)*(1.0 - reflection) + vec4(reflectedColor, 1.0)*reflection;
    #endif

        //color = clamp(color, 0.0, 1.0);
        color = sqrt(color); // gamma
        //color = vec4(pow(color.xyz, vec3(1.0/2.2)), 1.0); // gamma
        color *= 1.0 - 0.05*length(sp); // vignette
        return color;
    }
}

void main() {
    vec2 pixelCoord = vec2(gl_FragCoord.x, gl_FragCoord.y);
    vec3 dir = rayDirection(fieldOfView, iResolution, pixelCoord);
    vec3 eye = viewMatrix[3].xyz;
    vec2  sp = (2.0*pixelCoord-iResolution.xy) / iResolution.y;

    outColor = Render(eye, dir, sp);
}