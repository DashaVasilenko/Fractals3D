in mat4 viewMatrix;
out vec4 outColor;

uniform vec2 iResolution; 
uniform float fieldOfView;

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

uniform vec3 backgroundColor;
uniform float offset1;
uniform float offset2;
uniform float offset3;
uniform vec3 csize;
uniform int iterations;
uniform vec3 exposure;

const int MAX_MARCHING_STEPS = 256;
const float MIN_DIST = 0.01;
const float MAX_DIST = 20.0; 
const float EPSILON = 0.001; 
             
//-------------------------------------------------------------------------------------------------------
// Compute Apollonian/kleinian fractal
// https://www.shadertoy.com/view/ldjGDw
float apollonian(vec3 pos, float s, out vec4 trapColor) {
    float scale = 1.0;
    float add = sin(offset3)*0.2 + 0.1;

#if defined COLORING_TYPE_1 || defined COLORING_TYPE_2 || defined COLORING_TYPE_4 || defined COLORING_TYPE_5 || defined COLORING_TYPE_7
    vec4 trap = vec4(1000.0);
#endif

#if defined COLORING_TYPE_3 || defined COLORING_TYPE_6
    vec2  trap = vec2(1e10);
#endif
	
    for (int i = 0; i < iterations; i++) {
        pos = 2.0*clamp(pos, -csize, csize) - pos;
        float r2 = dot(pos, pos);

    #if defined COLORING_TYPE_1 || defined COLORING_TYPE_2 || defined COLORING_TYPE_4 || defined COLORING_TYPE_5 || defined COLORING_TYPE_7
        trap = min(trap, vec4(abs(pos), r2));  // trapping Oxz, Oyz, Oxy, (0,0,0)
    #endif
 
    #if defined COLORING_TYPE_3 || defined COLORING_TYPE_6
        trap = min(trap, vec2(r2, abs(pos.x))); // orbit trapping ( |z|² and z_x  )
    #endif
        
        float k = max(s/r2, 1.0);
		pos *= k;
		scale *= k;
	}

    float l = length(pos.xy);
	float rxy = l - 4.0;
	float n = l*pos.z;
    rxy = max(rxy, -n/(length(pos)) - 0.07 + sin(offset3*2.0 + pos.x + pos.y + 23.5*pos.z)*0.02);
    float x = (1.0 + sin(offset3*2.0));
    x =x*x*x*x*0.5;
    float h = dot(sin(pos*0.013), (cos(pos.zxy*0.191)))*x;

 #if defined COLORING_TYPE_1 || defined COLORING_TYPE_2 || defined COLORING_TYPE_4 || defined COLORING_TYPE_5 || defined COLORING_TYPE_7
    trapColor = trap;
#endif

#if defined COLORING_TYPE_3 || defined COLORING_TYPE_6
    trapColor = vec4(trap, 1.0, 1.0);
#endif

    return ((rxy + h)/abs(scale));
}

//-------------------------------------------------------------------------------------------------------
// Return the normalized direction to march in from the eye point for a single pixel.
// fieldOfView: vertical field of view in degrees
// size: resolution of the output image
// fragCoord: the x,y coordinate of the pixel in the output image
vec3 rayDirection(float fieldOfView, vec2 size, vec2 fragCoord) {
    vec2 xy = fragCoord - size/2.0;
    float z = size.y / tan(radians(fieldOfView)/2.0);
    vec3 dir = xy.x*viewMatrix[0].xyz + xy.y*viewMatrix[1].xyz + z*viewMatrix[2].xyz;
    return normalize(dir);
}

//-------------------------------------------------------------------------------------------------------
// Compute the normal on the surface at point p, using the gradient of the SDF  
// Compute the normal on the surface at point p, using the Tetrahedron technique
// https://iquilezles.org/www/articles/normalsSDF/normalsSDF.htm
vec3 computeNormal(vec3 p, float s) {
    vec4 trap;
    const float h = 0.0001; // replace by an appropriate value
    const vec2 k = vec2(1, -1)*h;
    return normalize( k.xyy*apollonian(p + k.xyy, s, trap) + 
                      k.yyx*apollonian(p + k.yyx, s, trap) + 
                      k.yxy*apollonian(p + k.yxy, s, trap) + 
                      k.xxx*apollonian(p + k.xxx, s, trap) );
}

//-------------------------------------------------------------------------------------------------------
// Return the shortest distance from the eyepoint to the scene surface along the marching direction. 
// If no part of the surface is found between start and end, return end.
// 
// eye: the eye point, acting as the origin of the ray
// direction: the normalized direction to march in
// start: the starting distance away from the eye
// end: the max distance away from the eye to march before giving up
float shortestDistanceToSurface(vec3 eye, vec3 direction, float start, float end, float s, out vec4 trapColor) {
    vec4 trap;
    float res = -1.0;
    float depth = start;

    for (int i = 0; i < MAX_MARCHING_STEPS; i++) {
        float precis = EPSILON*depth;
        float h = apollonian(eye + depth*direction, s, trap);
        if (h < precis) break; // We're inside the scene surface!
        if (depth >= end) break; // Gone too far; give up
        depth += h; // Move along the view ray
    }
    
    if (depth > end) return -1.0;
    trapColor = trap;
    res = depth;
    return res;
}

float calculateAO(vec3 pos, vec3 normal, float s) {
	float ao = 0.0;
    float sca = 1.0;
    vec4 t;
    for (int i = 0; i < 8; i++) {
        float h = 0.001 + 0.5*pow(i/7.0, 1.5);
        float d = apollonian(pos + h*normal, s, t);
        ao += -(d - h)*sca;
        sca *= 0.95;
    }
    return clamp(1.0 - 0.8*ao, 0.0, 1.0);
}

//-------------------------------------------------------------------------------------------------------
vec4 render(vec3 eye, vec3 dir, vec2 sp, float s ) {
	vec4 trap;  
    float dist = shortestDistanceToSurface(eye, dir, MIN_DIST, MAX_DIST, s, trap); // intersect fractal

    vec3 point = eye + dist*dir; // The closest point on the surface to the eyepoint along the view ray
    vec3 outNormal = computeNormal(point, s); // N
    vec3 col = backgroundColor;
        
    if (dist > 0) {
    #ifdef COLORING_TYPE_1
        vec3 albedo = color*0.3;
    #endif
    #ifdef COLORING_TYPE_2
        vec3 albedo = vec3(0.001); // чем больше значение, тем более засвеченный фрактал
        albedo = mix(albedo, color1, clamp(trap.y, 0.0, 1.0));
	 	albedo = mix(albedo, color2, clamp(trap.z*trap.z, 0.0, 1.0));
        albedo = mix(albedo, color3, clamp(pow(trap.w, 6.0), 0.0, 1.0));
        albedo *= 0.5;
    #endif
    #ifdef COLORING_TYPE_3
        vec3 albedo = vec3(color.x+color.x*cos(color.x+coef*trap.x),
                           color.y+color.y*cos(color.y+coef*trap.x),
                           color.z+color.z*cos(color.z+coef*trap.w) );
    #endif
    #ifdef COLORING_TYPE_4
        vec3 albedo = 0.5 + 0.5*cos(6.2831*trap.z + color); 
    #endif
    #ifdef COLORING_TYPE_5
        vec3 albedo = vec3(0.0);
        albedo = mix(albedo, color1, sqrt(trap.x) );
		albedo = mix(albedo, color2, sqrt(trap.y) );
		albedo = mix(albedo, color3, trap.z );
    #endif 
    #ifdef COLORING_TYPE_6
        vec3 albedo = 0.5 + 0.5*cos(6.2831*trap.x + color);
    #endif
    #ifdef COLORING_TYPE_7
        vec3 albedo = color1;
        albedo = mix(albedo, color2, clamp(6.0*trap.y, 0.0, 1.0));
        albedo = mix(albedo, color3, pow(clamp(1.0 - 2.0*trap.z, 0.0, 1.0), 8.0));
    #endif
        		
        float occlusion = calculateAO(point, outNormal, s);

        float dif1 = clamp(dot(lightDirection1, outNormal), 0.0, 1.0 )*occlusion; // light1
        float dif2 = clamp(0.2 + 0.8*dot(lightDirection2, outNormal), 0.0, 1.0)*occlusion; // bounce
        
        vec3 lin = vec3(0.0); 
		     lin += lightIntensity1*lightColor1*dif1; // light1
		     lin += lightIntensity2*lightColor2*dif2; //light2
             lin +=  ambientLightIntensity3*ambientLightColor3*(0.7 + 0.3*outNormal.y)*occlusion; // ambient light
        col = albedo*lin*exp(-0.3*dist);    

    #ifdef TONE_MAPPING
        // luma based Reinhard tone mapping
	    float luma = dot(col, exposure);
	    float toneMappedLuma = luma/(1.0 + luma);
	    col *= toneMappedLuma/luma;
    #endif    

    }
    return vec4(pow(col, vec3(1.0/2.2)), 1.0); // gamma
}

void main() {
    float t = Time;
    
    vec2 pixelCoord = vec2(gl_FragCoord.x, gl_FragCoord.y);
    //vec3 dir = rayDirection(fieldOfView, iResolution, pixelCoord);
    vec3 eye = viewMatrix[3].xyz;
    vec2  sp = (2.0*pixelCoord-iResolution.xy) / iResolution.y;

    //float time = Time*0.25;
    //float c = 1.1 + 0.5*smoothstep( -0.3, 0.3, cos(0.1*time) );
    float c = offset2 + 0.5*smoothstep( -0.3, 0.3, cos(0.1*offset1) );

    vec4 col = vec4(0.0);
    for( int i = 0; i < antiAliasing; i++ ) {
        for( int j = 0; j < antiAliasing; j++ ) {
            vec2 pixel = pixelCoord + (vec2(i,j)/float(antiAliasing));
            vec3 dir = rayDirection(fieldOfView, iResolution, pixel);
	        col += render(eye, dir, sp, c);
        }
    }
	col /= float(antiAliasing*antiAliasing);
    outColor = col;
}