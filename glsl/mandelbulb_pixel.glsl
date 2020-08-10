//#version 330

//#define FLAG_HARD_SHADOWS
//#define FLAG_SOFT_SHADOWS
//#define FLAG_AMBIENTOCCLUSION

in mat4 viewMatrix;
out vec4 outColor;

uniform vec2 iResolution; 
uniform float fieldOfView;

#if defined SKYBOX_BACKGROUND || defined SKYBOX_BACKGROUND_HDR
    uniform samplerCube skyBox; // сэмплер для кубической карты
#endif

#if defined SKYBOX_BACKGROUND_HDR && defined IRRADIANCE_CUBEMAP
    uniform samplerCube irradianceMap; // освещенность из кубмапы
#endif

#ifdef SOLID_BACKGROUND
    uniform vec3 reflectedColor;
#endif

uniform float Time;

uniform float shininess; // показатель степени зеркального отражения
uniform float reflection; // сила отражения

uniform vec3 lightDirection1;
uniform vec3 lightColor1;
uniform float lightIntensity1;

uniform vec3 lightDirection2;
uniform vec3 lightColor2;
uniform float lightIntensity2;

uniform vec3 ambientLightColor3;
uniform float ambientLightIntensity3;

uniform vec3 color1;
uniform vec3 color2;
uniform vec3 color3;

uniform int Iterations = 8;
uniform float Bailout = 10.0f;
uniform float Power = 8.0;

//const int MAX_MARCHING_STEPS = 255;
const int MAX_MARCHING_STEPS = 128;
const float MIN_DIST = 0.0;
const float MAX_DIST = 10.0; //50
const float EPSILON = 0.0005;
 
//-------------------------------------------------------------------------------------------------------
// The basic formulation of the Mandelbulb is derived from extracting the polar coordinates of a 3D point 
// and doubling its angles and squaring its length. The idea of duplicating can be generalized to triplicating,
// or more popularly multiplying by eight. 
// https://iquilezles.org/www/articles/mandelbulb/mandelbulb.htm
// https://www.shadertoy.com/view/ltfSWn
//
// colord using orbit traps method
// https://iquilezles.org/www/articles/orbittraps3d/orbittraps3d.htm
// https://iquilezles.org/www/articles/ftrapsgeometric/ftrapsgeometric.htm
// https://en.wikipedia.org/wiki/Orbit_trap
float mandelbulb(vec3 pos, out vec4 resColor) {
    //int Iterations = 8;
    //float Bailout = 10.0f;
    //float Power = 8.0;
    //float Power = 8.0*sin(Time / 50.0f);
    float t = Time;
    
    vec3 point = pos;
	float dr = 1.0;
	float r = 0.0;
    float m = dot(point, point);
    vec4 trap = vec4(abs(point), m);

	for (int i = 0; i < Iterations ; i++) {
        r = length(point);
		if (r > Bailout) break;
 
		// convert to polar coordinates
		float theta = acos(point.z/r);
		float phi = atan(point.y,point.x);
		dr =  pow(r, Power - 1.0)*Power*dr + 1.0;
		
		// scale and rotate the point
		float zr = pow(r, Power);
		theta = theta*Power;
		phi = phi*Power;
		
		// convert back to cartesian coordinates
		point = pos + zr*vec3(sin(theta)*cos(phi), sin(phi)*sin(theta), cos(theta));

        m = dot(point, point);
        trap = min(trap, vec4(abs(point),m));   
    }
    resColor = vec4(m,trap.yzw);
    //resColor = trap;
    return 0.5*log(r)*r/dr;
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
    return normalize( k.xyy*mandelbulb( p + k.xyy, trap) + 
                      k.yyx*mandelbulb( p + k.yyx, trap) + 
                      k.yxy*mandelbulb( p + k.yxy, trap) + 
                      k.xxx*mandelbulb( p + k.xxx, trap) );
}

//-------------------------------------------------------------------------------------------------------
// w is the size of the light source, and controls how hard/soft the shadows are
// https://www.iquilezles.org/www/articles/rmshadows/rmshadows.htm
float softShadow(vec3 shadowRayOrigin, vec3 shadowRayDir, float start, float end, float w ) { 
    float res = 1.0;
    vec4 trap;
    float iterations = 64;
    for(float t=start; t<end; iterations--) {
        float h = mandelbulb(shadowRayOrigin + shadowRayDir*t, trap);
        //if (h < 0.0001 ) return 0.0;
        res = min( res, w*h/t );
        if (res < 0.001 || iterations <= 0) break;
        //if (iterations <= 0) break;
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
    float dist = isphere(vec4(0.0, 0.0, 0.0, 1.25), eye, direction);
    if(dist < 0.0) return end;
    dist = min(dist, end);

    for (int i = 0; i < MAX_MARCHING_STEPS; i++) {
        float h = mandelbulb(eye + depth*direction, trap);
        if (h < EPSILON) break; // We're inside the scene surface!
        if (depth >= dist) break; // Gone too far; give up
        depth += h; // Move along the view ray
    }
    
    if (depth < dist) { 
        trapColor = trap;
        res = depth;
    }
    return res;
}

//-------------------------------------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec4 Render(vec3 eye, vec3 dir, vec2 sp) {
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
        vec3 albedo = vec3(0.001); // чем больше значение, тем более засвеченный фрактал
        albedo = mix(albedo, color1, clamp(trap.y, 0.0, 1.0));
	 	albedo = mix(albedo, color2, clamp(trap.z*trap.z, 0.0, 1.0));
        albedo = mix(albedo, color3, clamp(pow(trap.w, 6.0), 0.0, 1.0));
        albedo *= 0.5;
        
    #ifdef FLAG_SOFT_SHADOWS
        vec3 shadowRayOrigin = point + 0.001*outNormal;
        vec3 shadowRayDir = normalize(lightDirection1); // луч, направленный на источник света
        // последний параметр это сила размытости мягких теней
        shadow = softShadow(shadowRayOrigin, shadowRayDir, MIN_DIST, MAX_DIST, 32.0);
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
    float time = Time*.1;
    vec2 pixelCoord = vec2(gl_FragCoord.x, gl_FragCoord.y);
    float f = fieldOfView;

    // camera
	float di = 1.4+0.1*cos(.29*time);
	vec3  ro = di * vec3( cos(.33*time), 0.8*sin(.37*time), sin(.31*time) );
	vec3  ta = vec3(0.0,0.1,0.0);
	float cr = 0.5*cos(0.1*time);

    // camera matrix
	vec3 cp = vec3(sin(cr), cos(cr),0.0);
    vec3 cw = normalize(ta-ro);
	vec3 cu = normalize(cross(cw,cp));
	vec3 cv =          (cross(cu,cw));
    mat4 cam = mat4( cu, ro.x, cv, ro.y, cw, ro.z, 0.0, 0.0, 0.0, 1.0 );

    const float fle = 1.5; 	// ray setup
    vec2  sp = (2.0*pixelCoord-iResolution.xy) / iResolution.y;
    float px = 2.0/(iResolution.y*fle);

    vec3  eye = vec3( cam[0].w, cam[1].w, cam[2].w );
	vec3  dir = normalize( (cam*vec4(sp,fle,0.0)).xyz );

/*
    vec2 pixelCoord = vec2(gl_FragCoord.x, gl_FragCoord.y);
    vec3 dir = rayDirection(fieldOfView, iResolution, pixelCoord);
    vec3 eye = viewMatrix[3].xyz;
    vec2  sp = (2.0*pixelCoord-iResolution.xy) / iResolution.y;
*/
    outColor = Render(eye, dir, sp);

/*
// !!!!!!!!!! добавить антиалиасинг при рендеринге картинки конечной !!!!!!!!!
    // render
    //#if AA<2
    //cam = transpose(viewMatrix);
    vec3 col = render(pixelCoord, cam);
    //#else
    //vec3 col = vec3(0.0);
    //for( int j=ZERO; j<AA; j++ )
    //for( int i=ZERO; i<AA; i++ ) {
	//    col += render( fragCoord + (vec2(i,j)/float(AA)), cam );
    //}
	//col /= float(AA*AA);
    //#endif
*/
} 