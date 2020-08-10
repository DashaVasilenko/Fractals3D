// Related shaders
//
// Julia - Quaternion 1 : https://www.shadertoy.com/view/MsfGRr
// Julia - Quaternion 2 : https://www.shadertoy.com/view/lsl3W2
// Julia - Quaternion 3 : https://www.shadertoy.com/view/3tsyzl

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

uniform float shadowStrength;

uniform vec3 lightDirection1;
uniform vec3 lightColor1;
uniform float lightIntensity1;

uniform vec3 lightDirection2;
uniform vec3 lightColor2;
uniform float lightIntensity2;

uniform vec3 ambientLightColor3;
uniform float ambientLightIntensity3;

uniform float shininess; // показатель степени зеркального отражения
uniform float reflection; // сила отражения

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

//-------------------------------------------------------------------------------------------------------
// square a quaterion
vec4 qsqr(vec4 a)  {
    return vec4( a.x*a.x - a.y*a.y - a.z*a.z - a.w*a.w,
                 2.0*a.x*a.y,
                 2.0*a.x*a.z,
                 2.0*a.x*a.w );
}

const int numIterations = 11;

//-------------------------------------------------------------------------------------------------------
// Compute Julia set
// http://iquilezles.org/www/articles/juliasets3d/juliasets3d.htm
// https://iquilezles.org/www/articles/distancefractals/distancefractals.htm
// https://www.shadertoy.com/view/MsfGRr
float julia(vec3 pos, vec4 c, out vec4 trapColor) {
    vec4 z = vec4(pos, 0.0);
    float md2 = 1.0;
    float mz2 = dot(z, z);
    vec4 trap = vec4(abs(z.xyz), dot(z, z));

    float n = 1.0;
    for(int i = 0; i < numIterations; i++ ) {
        // dz -> 2·z·dz, meaning |dz| -> 2·|z|·|dz|
        // Now we take the 2.0 out of the loop and do it at the end with an exp2
        md2 *= 4.0*mz2;
        // z  -> z^2 + c
        z = qsqr(z) + c;  

        trap = min(trap, vec4(abs(z.xyz), dot(z, z)));

        mz2 = dot(z, z);
        if (mz2 > 4.0) break;
        n += 1.0;
    }
    trapColor = trap;
    return 0.25*sqrt(mz2/md2)*log(mz2);  // d = 0.5·|z|·log|z|/|z'|
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
vec3 computeNormal(vec3 p, vec4 c) {
    vec4 trap;
    const float h = 0.0001; // replace by an appropriate value
    const vec2 k = vec2(1,-1)*h;
    return normalize( k.xyy*julia( p + k.xyy, c, trap) + 
                      k.yyx*julia( p + k.yyx, c, trap) + 
                      k.yxy*julia( p + k.yxy, c, trap) + 
                      k.xxx*julia( p + k.xxx, c, trap) );
}

//-------------------------------------------------------------------------------------------------------
// w is the size of the light source, and controls how hard/soft the shadows are
// https://www.iquilezles.org/www/articles/rmshadows/rmshadows.htm
float softShadow(vec3 shadowRayOrigin, vec3 shadowRayDir, float start, float end, float w, vec4 c ) { 
    float res = 1.0;
    vec4 trap;
    float iterations = 64;
    for(float t=start; t<end; iterations--) {
        //float h = mandelbulb(shadowRayOrigin + shadowRayDir*t, trap);
        float h = julia(shadowRayOrigin + shadowRayDir*t, c, trap);
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
float shortestDistanceToSurface(vec3 eye, vec3 direction, float start, float end, vec4 c, out vec4 trapColor) {
    vec4 trap;
    float depth = start;
    float res = end;

    // bounding sphere
    // !!!!!!!!!!!!!!!!!! проверить, будет ли все влезать!!!!!!!!!!
    // возможно, надо будет удалить сферу
    float dist = isphere(vec4(0.0, 0.0, 0.0, 1.25), eye, direction);
    if(dist < 0.0) return end;
    dist = min(dist, end);

    for (int i = 0; i < MAX_MARCHING_STEPS; i++) {
        float h = julia(eye + depth*direction, c, trap);
        if (h < EPSILON) break; // We're inside the scene surface!
        if (depth >= dist) break; // Gone too far; give up
        //if (depth >= end) break; // Gone too far; give up
        depth += h; // Move along the view ray
    }
    
    if (depth < dist) { 
    //if (depth < end) { 
        trapColor = trap;
        res = depth;
    }
    return res;
}

//-------------------------------------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

//-------------------------------------------------------------------------------------------------------
vec4 render(vec3 eye, vec3 dir, vec4 c, vec2 sp ) {
	vec4 trap;  
    float dist = shortestDistanceToSurface(eye, dir, MIN_DIST, MAX_DIST, c, trap); // intersect fractal

    vec3 point = eye + dist*dir; // The closest point on the surface to the eyepoint along the view ray
    vec3 outNormal = computeNormal(point, c); // N
    
    // Didn't hit anything. sky color
    if (dist >= MAX_DIST) {
#if defined SKYBOX_BACKGROUND || defined SKYBOX_BACKGROUND_HDR
        return texture(skyBox, dir);
#endif

#ifdef SOLID_BACKGROUND
        return vec4(reflectedColor - (dir.y * 0.7), 1.0); // Skybox color
        //vec3 col = vec3(0.7,0.9,1.0)*(0.7+0.3*dir.y);
		//col += vec3(0.8,0.7,0.5)*pow( clamp(dot(dir,sun),0.0,1.0), 48.0 );
        //return vec4(col, 1.0);
#endif	
	}
    // color fractal
	else {
        vec3 albedo = vec3(1.0,0.8,0.7)*0.3;
		//albedo.x = 1.0-10.0*trap.x;
		float occlusion = clamp(2.5*trap.w - 0.15, 0.0, 1.0);
        vec3 col = vec3(0.0);
        vec3 hal = normalize(lightDirection1 - dir);
        float shadow = 1.0;

    #ifdef FLAG_SOFT_SHADOWS
        vec3 shadowRayOrigin = point + 0.001*outNormal;
        vec3 shadowRayDir = normalize(lightDirection1); // луч, направленный на источник света
        // предпоследний параметр это сила размытости мягких теней
        shadow = softShadow(shadowRayOrigin, shadowRayDir, MIN_DIST, MAX_DIST, shadowStrength, c);
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
        	 ////lin +=  1.5*vec3(0.10,0.20,0.30)*dif3;
             lin +=  ambientLightIntensity3*ambientLightColor3*(0.05+0.95*occlusion); // ambient light
		//vec3 col = albedo*lin;
        col = albedo*lin;
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
        //color = sqrt(color); // gamma
        //color = vec4(pow(color.xyz, vec3(1.0/2.2)), 1.0); // gamma
        //color *= 1.0 - 0.05*length(sp); // vignette
        //return color;
	    return vec4(pow(color.xyz, vec3(0.4545)), 1.0);
    }
}

void main() {
    float s = shadowStrength;
    float f = fieldOfView; // !!!!!!!!!!!!!!!!!!!!! удалить эту строку потом !!!!!!!!!!!!!!!!!!!

    vec2 fragCoord = vec2(gl_FragCoord.x, gl_FragCoord.y);
    vec2  sp = (2.0*fragCoord-iResolution.xy) / iResolution.y;


    // anim
    float time = Time*.15;
    vec4 c = 0.45*cos( vec4(0.5,3.9,1.4,1.1) + time*vec4(1.2,1.7,1.3,2.5) ) - vec4(0.3,0.0,0.0,0.0);

    // camera
	float r = 1.5+0.15*cos(0.0+0.29*time);
	vec3 ro = vec3(           r*cos(0.3+0.37*time), 
					0.3 + 0.8*r*cos(1.0+0.33*time), 
					          r*cos(2.2+0.31*time) );
	vec3 ta = vec3(0.0,0.0,0.0);
    float cr = 0.1*cos(0.1*time);
    
    
    // render
    vec4 col = vec4(0.0);
    for( int j=0; j<AA; j++ )
    for( int i=0; i<AA; i++ )
    {
        vec2 p = (-iResolution.xy + 2.0*(fragCoord + vec2(float(i),float(j))/float(AA))) / iResolution.y;

        vec3 cw = normalize(ta-ro);
        vec3 cp = vec3(sin(cr), cos(cr),0.0);
        vec3 cu = normalize(cross(cw,cp));
        vec3 cv = normalize(cross(cu,cw));
        vec3 rd = normalize( p.x*cu + p.y*cv + 2.0*cw );

        col += render( ro, rd, c, sp );
    }
    col /= float(AA*AA);
    
    vec2 uv = fragCoord.xy / iResolution.xy;
	col *= 0.7 + 0.3*pow(16.0*uv.x*uv.y*(1.0-uv.x)*(1.0-uv.y),0.25);
    
	//outColor = vec4( col, 1.0 );
    outColor = col;
}