in mat4 viewMatrix;
out vec4 outColor;

uniform vec2 iResolution; 
uniform float fieldOfView;

#if defined SKYBOX_BACKGROUND || defined SKYBOX_BACKGROUND_HDR
    uniform float backgroundBrightness;
    uniform samplerCube skyBox; // сэмплер для кубической карты
#endif

#if defined SKYBOX_BACKGROUND_HDR && defined IRRADIANCE_CUBEMAP
    uniform samplerCube irradianceMap; // освещенность из кубмапы
#endif

#if defined SOLID_BACKGROUND || defined SOLID_BACKGROUND_WITH_SUN
    uniform vec3 reflectedColor;
#endif

#ifdef SOLID_BACKGROUND_WITH_SUN
    uniform vec3 sunColor;
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

#if defined COLORING_TYPE_1 || defined COLORING_TYPE_3 || defined COLORING_TYPE_4 || defined COLORING_TYPE_6
uniform vec3 color;
#endif

#ifdef COLORING_TYPE_3
uniform float coef;
#endif

#if defined COLORING_TYPE_2 || defined COLORING_TYPE_5
uniform vec3 color1;
uniform vec3 color2;
uniform vec3 color3;
#endif

uniform float shininess; // показатель степени зеркального отражения
uniform float reflection; // сила отражения

uniform float offset1;
uniform float offset2;
uniform int iterations;

//const int MAX_MARCHING_STEPS = 255;
const int MAX_MARCHING_STEPS = 128;
const float MIN_DIST = 0.0;
const float MAX_DIST = 10.0; //50
const float EPSILON = 0.0005;

// antialias level (1, 2, 3...)
#define AA 1
//#else
//#define AA 2  // Set AA to 1 if your machine is too slow
//#endif

float cylinderUnion(vec3 p){
    float xy = dot(p.xy, p.xy);
    float xz = dot(p.xz, p.xz);
    float yz = dot(p.yz, p.yz);
    return sqrt(min(xy, min(xz, yz))) - 1.0;
}

float cylinderIntersection(vec3 p){
    float xy = dot(p.xy, p.xy);
    float xz = dot(p.xz, p.xz);
    float yz = dot(p.yz, p.yz);
    return sqrt(max(xy, max(xz, yz))) - 1.0;
}
             
//-------------------------------------------------------------------------------------------------------
// Compute Menger sponge
// https://www.iquilezles.org/www/articles/menger/menger.htm
float mengerSponge(vec3 pos, out vec4 trapColor) {
    float d = cylinderIntersection(pos);
    float s = 1.0;
    vec4 res = vec4(d, 1.0, 0.0, 0.0);

    float ani = offset2;
	float off = offset1;

    //float ani = smoothstep(-0.2, 0.2, -cos(0.5*Time));
	//float off = 1.5*sin(0.01*Time);

#if defined COLORING_TYPE_1 || defined COLORING_TYPE_2 || defined COLORING_TYPE_4 || defined COLORING_TYPE_5
    vec4 trap = vec4(abs(pos), dot(pos, pos));
#endif

#if defined COLORING_TYPE_3 || defined COLORING_TYPE_6
    vec2  trap = vec2(1e10);
#endif

    for (int i = 0; i < iterations; i++) {
        pos = mix(pos, pos+off, ani);
        vec3 a = mod(pos*s, 2.0) - 1.0;
    	s *= 3.0;
        vec3 r = abs(1.0 - 3.0*abs(a));
    	float c = cylinderUnion(r)/s;

    #if defined COLORING_TYPE_1 || defined COLORING_TYPE_2 || defined COLORING_TYPE_4 || defined COLORING_TYPE_5
        trap = min(trap, vec4(abs(pos), dot(pos, pos)));  // trapping Oxz, Oyz, Oxy, (0,0,0)
    #endif
 
    #if defined COLORING_TYPE_3 || defined COLORING_TYPE_6
        trap = min(trap, vec2(dot(pos, pos), abs(pos.x))); // orbit trapping ( |z|² and z_x  )
    #endif
        
        if (c > d) {
          d = c;
          float da = max(r.x, r.y);
          float db = max(r.y, r.z);
          float dc = max(r.z, r.x);
          res = vec4(d, min(res.y, 0.2*da*db*dc), (1.0+float(i))/4.0, 0.0 );
        }	
    }
#if defined COLORING_TYPE_1 || defined COLORING_TYPE_2 || defined COLORING_TYPE_4 || defined COLORING_TYPE_5
    trapColor = trap;
#endif

#if defined COLORING_TYPE_3 || defined COLORING_TYPE_6
    trapColor = vec4(trap, res.y, res.z);
#endif

    return d;
}

//-------------------------------------------------------------------------------------------------------
// Return the normalized direction to march in from the eye point for a single pixel.
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
// Compute the normal on the surface at point p, using the gradient of the SDF  
// Compute the normal on the surface at point p, using the Tetrahedron technique
// https://iquilezles.org/www/articles/normalsSDF/normalsSDF.htm
vec3 computeNormal(vec3 p) {
    vec4 trap;
    const float h = 0.0001; // replace by an appropriate value
    const vec2 k = vec2(1,-1)*h;
    return normalize( k.xyy*mengerSponge( p + k.xyy, trap) + 
                      k.yyx*mengerSponge( p + k.yyx, trap) + 
                      k.yxy*mengerSponge( p + k.yxy, trap) + 
                      k.xxx*mengerSponge( p + k.xxx, trap) );
}

//-------------------------------------------------------------------------------------------------------
// w is the size of the light source, and controls how hard/soft the shadows are
// https://www.iquilezles.org/www/articles/rmshadows/rmshadows.htm
float softShadow(vec3 shadowRayOrigin, vec3 shadowRayDir, float start, float end, float w) { 
    float res = 1.0;
    vec4 trap;
    float iterations = 64;
    for(float t=start; t<end; iterations--) {
        //float h = mandelbulb(shadowRayOrigin + shadowRayDir*t, trap);
        float h = mengerSponge(shadowRayOrigin + shadowRayDir*t, trap);
        res = min( res, w*h/t );
        if (res < 0.001 || iterations <= 0) break;
        t += h;
    }
    return clamp(res, 0.0, 1.0);
}

//-------------------------------------------------------------------------------------------------------
// Basic Bounding Volumes
// boundingSphere.xyz - centrum, boundingSphere..w - radius
// https://iquilezles.org/www/articles/sdfbounding/sdfbounding.htm
float isphere(vec4 boundingSphere, vec3 point, vec3 direction) {
    vec3 dist = point - boundingSphere.xyz;
	float b = dot(dist,direction);
	float c = dot(dist,dist) - boundingSphere.w*boundingSphere.w;
    float h = b*b - c;
    
    if (h < 0.0) return -1.0;
    return -b + sqrt(h);
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

    // bounding sphere
    // !!!!!!!!!!!!!!!!!! проверить, будет ли все влезать!!!!!!!!!!
    // возможно, надо будет удалить сферу
    //float dist = isphere(vec4(0.0, 0.0, 0.0, 1.75), eye, direction);
    //if(dist < 0.0) return end;
    //dist = min(dist, end);

    for (int i = 0; i < MAX_MARCHING_STEPS; i++) {
        float h = mengerSponge(eye + depth*direction, trap);
        if (h < EPSILON) break; // We're inside the scene surface!
        //if (depth >= dist) break; // Gone too far; give up
        if (depth >= end) break; // Gone too far; give up
        depth += h; // Move along the view ray
    }
    
    //if (depth < dist) { 
    if (depth < end) { 
        trapColor = trap;
        res = depth;
    }
    return res;
}

//-------------------------------------------------------------------------------------------------------
float occlusion(vec3 pos, vec3 normal) {
	float ao = 0.0;
    float sca = 1.0;
    vec4 t;
    for (int i = 0; i < 8; i++) {
        float h = 0.001 + 0.5*pow(i/7.0, 1.5);
        float d = mengerSponge(pos + h*normal, t);
        ao += -(d - h)*sca;
        sca *= 0.95;
    }
    return clamp(1.0 - 0.8*ao, 0.0, 1.0);
}

//-------------------------------------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

//-------------------------------------------------------------------------------------------------------
vec4 render(vec3 eye, vec3 dir, vec2 sp ) {
	vec4 trap;  
    float dist = shortestDistanceToSurface(eye, dir, MIN_DIST, MAX_DIST, trap); // intersect fractal

    vec3 point = eye + dist*dir; // The closest point on the surface to the eyepoint along the view ray
    vec3 outNormal = computeNormal(point); // N
    
    // Didn't hit anything. sky color
    if (dist >= MAX_DIST) {
#if defined SKYBOX_BACKGROUND || defined SKYBOX_BACKGROUND_HDR
        return texture(skyBox, dir)*backgroundBrightness;
#endif

#ifdef SOLID_BACKGROUND
        return vec4(reflectedColor - (dir.y * 0.7), 1.0); // Skybox color
#endif

#ifdef SOLID_BACKGROUND_WITH_SUN
        vec3 col  = reflectedColor*(0.6+0.4*dir.y); 
        col += lightIntensity1*sunColor*pow( clamp(dot(dir, lightDirection1),0.0,1.0), 32.0); 
        return vec4(col, 1.0);
#endif
	}
    // color fractal
	else {

        // main color
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
        //vec3 albedo = 0.5 + 0.5*sin(trap.y*4.0 + 4.0 + color + outNormal*0.2).xzy;
        //vec3 albedo = 0.5 + 0.5*cos(6.2831*trap.z + color);
        vec3 albedo = vec3(color.x+color.x*cos(color.x+coef*trap.w),
                           color.y+color.y*cos(color.y+coef*trap.w),
                           color.z+color.z*cos(color.z+coef*trap.w) );
    #endif
    #ifdef COLORING_TYPE_4
        vec3 albedo = color;
        albedo *= 0.1;
        albedo.x = 1.0-10.0*trap.x; 
    #endif
    #ifdef COLORING_TYPE_5
        vec3 albedo = vec3(0.0);
        albedo = mix(albedo, color1, sqrt(trap.x) );
		albedo = mix(albedo, color2, sqrt(trap.y) );
		albedo = mix(albedo, color3, trap.z );
        //albedo *= 0.4;
    #endif 
    #ifdef COLORING_TYPE_6
        vec3 albedo = 0.5 + 0.5*cos(6.2831*trap.z + color);
    #endif
        		
        //float occlusion = occlusion(point, outNormal);
        float occlusion = trap.z;
        vec3 hal = normalize(lightDirection1 - dir);
        float shadow = 1.0;

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
        //col += spe1*15.0;
        col += spe1*lightIntensity1;
        
        // sky
        vec4 color; 
        float intensity = (lightIntensity1 + lightIntensity2 + ambientLightIntensity3)*0.05;
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

        //col += ambientIBL; 
        color.xyz += ambientIBL; 
    #endif
/*
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

    #if defined SOLID_BACKGROUND || defined SOLID_BACKGROUND_WITH_SUN
        color = vec4(col, 1.0)*(1.0 - reflection) + vec4(reflectedColor, 1.0)*reflection;
    #endif
*/
        //color = clamp(color, 0.0, 1.0);
        //color = sqrt(color); // gamma
        //color = vec4(pow(color.xyz, vec3(1.0/2.2)), 1.0); // gamma
        //color *= 1.0 - 0.05*length(sp); // vignette
        //return color;
        //return vec4(pow(clamp(color.xyz, 0.0, 1.0), vec3(0.4545)), 1.0); // gamma
	    return vec4(pow(color.xyz, vec3(0.4545)), 1.0);
    }
}

void main() {
    float s = shadowStrength;
    float t = Time;
    
    vec2 pixelCoord = vec2(gl_FragCoord.x, gl_FragCoord.y);
    vec3 dir = rayDirection(fieldOfView, iResolution, pixelCoord);
    vec3 eye = viewMatrix[3].xyz;
    vec2  sp = (2.0*pixelCoord-iResolution.xy) / iResolution.y;
    vec4 offset; // фиктивный параметр из старого фрактала
    //vec4 c = vec4(-0.1,0.6,0.9,-0.3) + 0.1*sin( vec4(3.0,0.0,1.0,2.0) + 0.5*vec4(1.0,1.3,1.7,2.1)*offset);

    //vec4 col = vec4(calcPixel(pixelCoord, Time), 1.0);

    
    // render
    vec4 col = vec4(0.0);
    for( int j=0; j<AA; j++ )
    for( int i=0; i<AA; i++ )
    {
        //vec2 p = (-iResolution.xy + 2.0*(fragCoord + vec2(float(i),float(j))/float(AA))) / iResolution.y;
        //vec3 cw = normalize(ta-ro);
        //vec3 cp = vec3(sin(cr), cos(cr),0.0);
        //vec3 cu = normalize(cross(cw,cp));
        //vec3 cv = normalize(cross(cu,cw));
        //vec3 rd = normalize( p.x*cu + p.y*cv + 2.0*cw );
        //col += render(ro, rd, c, sp );

        col += render(eye, dir, sp );
    }
    col /= float(AA*AA);
/*
    // Menger sponge
    // camera
    vec3 ro = 1.1*vec3(2.5*sin(0.25*iTime),1.0+1.0*cos(iTime*.13),2.5*cos(0.25*iTime));

#if AA>1
    #define ZERO (min(iFrame,0))
    vec3 col = vec3(0.0);
    for( int m=ZERO; m<AA; m++ )
    for( int n=ZERO; n<AA; n++ )
    {
        // pixel coordinates
        vec2 o = vec2(float(m),float(n)) / float(AA) - 0.5;
        vec2 p = (2.0*(fragCoord+o)-iResolution.xy)/iResolution.y;

        vec3 ww = normalize(vec3(0.0) - ro);
        vec3 uu = normalize(cross( vec3(0.0,1.0,0.0), ww ));
        vec3 vv = normalize(cross(ww,uu));
        vec3 rd = normalize( p.x*uu + p.y*vv + 2.5*ww );

        col += render( ro, rd );
    }
    col /= float(AA*AA);
#else   
    vec2 p = (2.0*fragCoord-iResolution.xy)/iResolution.y;
    vec3 ww = normalize(vec3(0.0) - ro);
    vec3 uu = normalize(cross( vec3(0.0,1.0,0.0), ww ));
    vec3 vv = normalize(cross(ww,uu));
    vec3 rd = normalize( p.x*uu + p.y*vv + 2.5*ww );
    vec3 col = render( ro, rd );
#endif        
    
    fragColor = vec4(col,1.0);
*/   
    
    outColor = col;
}