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

uniform vec3 lightDirection;
uniform vec3 ambientLightColor; // интенсивность фонового света
uniform vec3 diffuseLightColor; // интенсивность рассеянного света
uniform vec3 specularLightColor; // интенсивность зеркального света

uniform vec3 ambientColor; // отражение фонового света материалом
uniform vec3 diffuseColor; // отражение рассеянного света материалом
uniform vec3 specularColor; // отражение зеркального света материалом
uniform float shininess; // показатель степени зеркального отражения
uniform float reflection;

uniform int Iterations = 8;
uniform float Bailout = 10.0f;
uniform float Power = 8.0;

const int MAX_MARCHING_STEPS = 255;
const float MIN_DIST = 0.0;
const float MAX_DIST = 50.0;
const float EPSILON = 0.005;
 
//-------------------------------------------------------------------------------------------------------
// The basic formulation of the Mandelbulb is derived from extracting the polar coordinates of a 3D point 
// and doubling its angles and squaring its length. The idea of duplicating can be generalized to triplicating,
// or more popularly multiplying by eight. We will in fact choose the arbitrary value of eight, because for higher 
// powers the asymptotic behaviour of the formulas tends to produce more symmetric shapes. So, let's call w to our 3D point,
// then choose eight as our polynomial degree for our Mandelbrot, and so multiply the polar angles of our 3D point by eight 
// and expand it's modulo by a power of eight:
// https://iquilezles.org/www/articles/mandelbulb/mandelbulb.htm

//-------------------------------------------------------------------------------------------------------
// In my experiments I opted for using the old orbit traps method applied in 3D but using the results to assign a color. 
// What I did is to compute four orbit traps. One of the traps was the origin, and the other three traps were the x=0, y=0 and z=0 planes.
// The last three traps were used to mix the basic surface color with three other colors. The point trap in the origin was used as 
// a multiplicative factor to the color, which simulated some ambient occlusion.
//
// The improvement from the other rudimentary coloring methods is that because the behaviour of the orbit traps follows 
// the fractal structure of the set, so do the colors, and therefore we get meaningful color patterns emerging in the 
// surface as opposed to generic perlin noise based coloring.
// https://iquilezles.org/www/articles/orbittraps3d/orbittraps3d.htm
// https://iquilezles.org/www/articles/ftrapsgeometric/ftrapsgeometric.htm
// In mathematics, an orbit trap is a method of colouring fractal images based upon how close an iterative function,
// used to create the fractal, approaches a geometric shape, called a "trap". Typical traps are points, lines, circles,
// flower shapes and even raster images.
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
        trap = min( trap, vec4(abs(point),m) );
        //m = dot(point, point);

/*
        r = length(point);
		if (r > Bailout) break;
 
        dr = 8.0*pow(sqrt(m),Power - 1.0)*dr + 1.0;
        
        float b = 8.0*acos( point.y/r);
        float a = 8.0*atan( point.x, point.z );
        point = pos + pow(r, Power) * vec3( sin(b)*sin(a), cos(b), sin(b)*cos(a) );

        m = dot(point, point);
        trap = min( trap, vec4(abs(point),m) );
*/
    }
    resColor = vec4(m,trap.yzw);
    //resColor = vec4(m,trap.xyw);

    return 0.5*log(r)*r/dr;
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
    for (int i = 0; i < MAX_MARCHING_STEPS; i++) {
        float dist = mandelbulb(eye + depth*direction, trap);
        if (dist < EPSILON) {
			return depth; // We're inside the scene surface!
        }
        depth += dist; // Move along the view ray
        if (depth >= end) {
            return depth; // Gone too far; give up
        }
    }
    trapColor = trap;
    return depth;
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
// Compute the normal on the surface at point p, using the gradient of the SDF
vec3 computeNormal(vec3 p) {
    vec4 trap;
    return normalize(vec3(
        mandelbulb(vec3(p.x + EPSILON, p.y, p.z), trap) - mandelbulb(vec3(p.x - EPSILON, p.y, p.z), trap),
        mandelbulb(vec3(p.x, p.y + EPSILON, p.z), trap) - mandelbulb(vec3(p.x, p.y - EPSILON, p.z), trap),
        mandelbulb(vec3(p.x, p.y, p.z + EPSILON), trap) - mandelbulb(vec3(p.x, p.y, p.z - EPSILON), trap)
    ));
}

//-------------------------------------------------------------------------------------------------------
// https://www.iquilezles.org/www/articles/rmshadows/rmshadows.htm
// w is the size of the light source, and controls how hard/soft the shadows are
float softShadow(vec3 shadowRayOrigin, vec3 shadowRayDir, float start, float end, float w ) { 
    float res = 1.0;
    float ph = 1e20;
    vec4 trap;
    for(float t=start; t<end; ) {
        float h = mandelbulb(shadowRayOrigin + shadowRayDir*t, trap);
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

//-------------------------------------------------------------------------------------------------------
// s is a scale variable determining the darkening effects of the occlusion
// http://www2.compute.dtu.dk/pubdb/views/edoc_download.php/6392/pdf/imm6392.pdf
float ambientOcclusion(vec3 point, vec3 normal, float step, float samples, float s) {
    float ao = 0.0f;
    float dist;
    vec4 trap;
    for (float i = 1.0f; i <= samples; i += 1.0f) {
        dist = step*i;  
        ao = max( (dist - (mandelbulb(point + normal*dist, trap))) / pow(2.0, i), ao); 
        //ao += max( (dist - (mandelbulb(point + normal*dist))) / pow(2.0, i), 0.0); 
        //ao += (dist - (mandelbulb(point + normal*dist))) / (dist*dist); 
    }
    return 1.0f - ao*s;
    //return clamp(1.0f - ao*s, 0.0, 1.0);
}

//-------------------------------------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

//-------------------------------------------------------------------------------------------------------
// Lighting contribution of a direction light source via Phong illumination.
vec4 PhongDirectionLight(vec3 ambientColor, vec3 diffuseColor, vec3 specularColor, float shininess, vec3 point, vec3 eye, vec4 trapColor) {
    float shadow = 1.0;
     
#ifdef FLAG_HARD_SHADOWS
    vec3 shadowRayOrigin = point + computeNormal(point)*0.01;
    vec3 shadowRayDir = normalize(vec3(-lightDirection)); 
    float dist = shortestDistanceToSurface(shadowRayOrigin, shadowRayDir, MIN_DIST, MAX_DIST);
    if (dist < MAX_DIST)
        return vec4(0.0, 0.0, 0.0, 1.0);
#endif

#ifdef FLAG_SOFT_SHADOWS
    vec3 shadowRayOrigin = point + computeNormal(point)*0.01;
    vec3 shadowRayDir = normalize(vec3(-lightDirection));
    // последний параметр это сила размытости мягких теней
    shadow = softShadow(shadowRayOrigin, shadowRayDir, MIN_DIST, MAX_DIST, 6.0);
#endif

    vec3 light_direction = normalize(vec3(-lightDirection)); // L для направленного
    vec3 inEye = normalize(eye - point); // V
    vec3 outNormal = computeNormal(point); // N
    vec3 reflected_light = reflect(-light_direction, outNormal); //R

    vec3 ambient = ambientLightColor*ambientColor;
    vec3 diffuse = diffuseLightColor*diffuseColor*max(dot(light_direction, outNormal), 0.0f)*shadow;
    vec3 specular = specularLightColor*specularColor*pow(max(dot(inEye, reflected_light), 0.0), shininess);
    vec3 color = ambient + diffuse + specular;

    //#ifdef MANDELBULB
    ambient = vec3(0.01);
	ambient = mix( ambient, vec3(0.10,0.20,0.30), clamp(trapColor.y,0.0,1.0) );
	ambient = mix( ambient, vec3(0.02,0.10,0.30), clamp(trapColor.z*trapColor.z,0.0,1.0) );
    ambient = mix( ambient, vec3(0.30,0.10,0.02), clamp(pow(trapColor.w,6.0),0.0,1.0) );
    ambient *= 0.5;
    color = ambient;
    //#endif


#if defined SKYBOX_BACKGROUND_HDR && defined IRRADIANCE_CUBEMAP
//#ifdef IRRADIANCE_CUBEMAP
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
    vec3 diffuseIBL      = irradiance * diffuseColor;
    //vec3 ambient = (kD * diffuse) * ao;
    vec3 ambientIBL = (kD * diffuseIBL) * ambientColor;

    color += ambientIBL; 
#endif

#ifdef FLAG_AMBIENTOCCLUSION
    float ao = ambientOcclusion(point, outNormal, 2.5, 3.0, 0.5);
    color *= ao;
#endif

    return clamp(vec4(color, 1.0), 0.0f, 1.0f);
}

//-------------------------------------------------------------------------------------------------------
vec4 Lambert(vec3 color, vec3 dir_light, vec3 point) {
    vec3 directional_light = normalize(dir_light);
    vec3 outNormal = computeNormal(point); 
    float kd = clamp(dot(directional_light, outNormal), 0.0f, 1.0f);
    return kd*vec4(color, 1.0);
}

/*
#ifdef MANDELBULB

vec3 mb(vec3 p) {
	p.xyz = p.xzy;
	vec3 z = p;
	vec3 dz=vec3(0.0);
	float power = 8.0;
	float r, theta, phi;
	float dr = 1.0;
	
	float t0 = 1.0;
	for(int i = 0; i < 7; ++i) {
		r = length(z);
		if(r > 2.0) continue;
		theta = atan(z.y / z.x);
        #ifdef phase_shift_on
		phi = asin(z.z / r) + iTime*0.1;
        #else
        phi = asin(z.z / r);
        #endif
		
		dr = pow(r, power - 1.0) * dr * power + 1.0;
	
		r = pow(r, power);
		theta = theta * power;
		phi = phi * power;
		
		z = r * vec3(cos(theta)*cos(phi), sin(theta)*cos(phi), sin(phi)) + p;
		
		t0 = min(t0, r);
	}
	return vec3(0.5 * log(r) * r / dr, t0, 0.0);
}

vec3 intersect( in vec3 ro, in vec3 rd )
{
    float t = 1.0;
    float res_t = 0.0;
    float res_d = 1000.0;
    vec3 c, res_c;
    float max_error = 1000.0;
	float d = 1.0;
    float pd = 100.0;
    float os = 0.0;
    float step = 0.0;
    float error = 1000.0;
    
    for( int i=0; i<48; i++ )
    {   
            c = mb(ro + rd*t);
            d = c.x;

            if(d > os)
            {
                os = 0.4 * d*d/pd;
                step = d + os;
                pd = d;
            }
            else
            {
                step =-os; os = 0.0; pd = 100.0; d = 1.0;
            }

            error = d / t;

            if(error < max_error) 
            {
                max_error = error;
                res_t = t;
                res_c = c;
            }
        
            t += step;
    }
	if( t>20.0 ) res_t=-1.0;
    return vec3(res_t, res_c.y, res_c.z);
}

vec3 nor( in vec3 pos )
{
    vec3 eps = vec3(0.001,0.0,0.0);
	return normalize( vec3(
           mb(pos+eps.xyy).x - mb(pos-eps.xyy).x,
           mb(pos+eps.yxy).x - mb(pos-eps.yxy).x,
           mb(pos+eps.yyx).x - mb(pos-eps.yyx).x ) );
}

 float softshadow(vec3 ro, vec3 rd, float k ){ 
     float akuma=1.0,h=0.0; 
	 float t = 0.01;
     for(int i=0; i < 50; ++i){ 
         h=mb(ro+rd*t).x; 
         if(h<0.001)return 0.02; 
         akuma=min(akuma, k*h/t); 
 		 t+=clamp(h,0.01,2.0); 
     } 
     return akuma; 
 } 
#endif
*/

void main() {
    vec2 pixelCoord = vec2(gl_FragCoord.x, gl_FragCoord.y);
    vec3 dir = rayDirection(fieldOfView, iResolution, pixelCoord);
    vec3 eye = viewMatrix[3].xyz;
    vec4 trapColor;
    float dist = shortestDistanceToSurface(eye, dir, MIN_DIST, MAX_DIST, trapColor);
    
    // Didn't hit anything
    if (dist > MAX_DIST - EPSILON) {
#if defined SKYBOX_BACKGROUND || defined SKYBOX_BACKGROUND_HDR
        outColor = texture(skyBox, dir);
#endif

#ifdef SOLID_BACKGROUND
        //outColor = vec4(vec3(0.30, 0.36, 0.60) - (dir.y * 0.7), 1.0); // Skybox color
        outColor = vec4(reflectedColor - (dir.y * 0.7), 1.0); // Skybox color
#endif
		return;
    }

    vec3 point = eye + dist*dir; // The closest point on the surface to the eyepoint along the view ray
    //outColor = Lambert(vec3(0.0, 1.0 , 0.0), vec3(0.0f, 1.0f, 1.0f), point);
    outColor = PhongDirectionLight(ambientColor, diffuseColor, specularColor, shininess, point, eye, trapColor);

    vec3 outNormal = computeNormal(point); // N
    vec3 reflected_dir = reflect(dir, outNormal); //R

//#ifdef MANDELBULB 
    const float fle = 1.5;
    //vec2  sp = (2.0*p-iResolution.xy) / iResolution.y;
    //float px = 2.0/(iResolution.y*fle);
    //vec3  ro = vec3( cam[0].w, cam[1].w, cam[2].w );
	vec3  rd = normalize(dir);
    vec3 light1 = vec3(  0.577, 0.577, -0.577 );
    vec3 light2 = vec3( -0.707, 0.000,  0.707 );


    vec3 col = vec3(0.01);
	col = mix( col, vec3(0.10,0.20,0.30), clamp(trapColor.y,0.0,1.0) );
	col = mix( col, vec3(0.02,0.10,0.30), clamp(trapColor.z*trapColor.z,0.0,1.0) );
    col = mix( col, vec3(0.30,0.10,0.02), clamp(pow(trapColor.w,6.0),0.0,1.0) );
    col *= 0.5;
	//col = vec3(0.1);
        
    // lighting terms
    //vec3 pos = ro + t*rd;
    //vec3 nor = calcNormal( point, t, px );
    vec3 nor = outNormal;
    vec3 hal = normalize( light1-rd);
    //vec3 ref = reflect( rd, nor );
    float occ = clamp(0.05*log(trapColor.x),0.0,1.0);
    float fac = clamp(1.0+dot(rd,nor),0.0,1.0);

    // sun
    //float sha1 = softshadow( pos+0.001*nor, light1, 32.0 );
    //vec3 shadowRayOrigin = point + computeNormal(point)*0.01;
    //vec3 shadowRayDir = normalize(vec3(-lightDirection));
    float sha1 = softShadow(point+0.001*nor, light1, MIN_DIST, MAX_DIST, 6.0);
    float dif1 = clamp( dot( light1, nor ), 0.0, 1.0 )*sha1;
    float spe1 = pow( clamp(dot(nor,hal),0.0,1.0), 32.0 )*dif1*(0.04+0.96*pow(clamp(1.0-dot(hal,light1),0.0,1.0),5.0));
    // bounce
    float dif2 = clamp( 0.5 + 0.5*dot( light2, nor ), 0.0, 1.0 )*occ;
    // sky
    float dif3 = (0.7+0.3*nor.y)*(0.2+0.8*occ);
        
	vec3 lin = vec3(0.0); 
	     lin += 7.0*vec3(1.50,1.10,0.70)*dif1;
	     lin += 4.0*vec3(0.25,0.20,0.15)*dif2;
    	 lin += 1.5*vec3(0.10,0.20,0.30)*dif3;
         lin += 2.5*vec3(0.35,0.30,0.25)*(0.05+0.95*occ); // ambient
    	 lin += 4.0*fac*occ;                          // fake SSS
	col *= lin;
	col = pow( col, vec3(0.7,0.9,1.0) );                  // fake SSS
    col += spe1*15.0;
	//col = sqrt( vec4(col, 0.0f) ); // gamma
    //p = ro + res.x * rd;   
//#endif

#if defined SKYBOX_BACKGROUND || defined SKYBOX_BACKGROUND_HDR
    vec4 reflected_color = texture(skyBox, reflected_dir);
    outColor = outColor*(1.0 - reflection) + reflected_color*reflection;
#endif

#ifdef SOLID_BACKGROUND
    outColor = outColor*(1.0 - reflection) + vec4(reflectedColor, 1.0)*reflection;
#endif
    //outColor = vec4(pow(outColor.xyz, vec3(1.0/2.2)), 1.0); // Gamma correction

//#ifdef MANDELBULB   
    outColor = sqrt( vec4(col, 0.0f) ); // gamma
    //outColor = color; // gamma
//#endif

/*
#ifdef MANDELBULB
    vec2 q=gl_FragCoord.xy/iResolution.xy; 
 	vec2 uv = -1.0 + 2.0*q; 
 	uv.x*=iResolution.x/iResolution.y; 
     
    float pixel_size = 1.0/(iResolution.x * 3.0);

 	vec3 ta=vec3(0.0,0.0,0.0); 
    //vec3 eye = viewMatrix[3].xyz;

 	vec3 cf = normalize(ta-eye); 
    vec3 cs = normalize(cross(cf,vec3(0.0,1.0,0.0))); 
    vec3 cu = normalize(cross(cs,cf)); 
 	vec3 dirM = normalize(uv.x*cs + uv.y*cu + 3.0*cf);  // transform from view to world

    vec3 sundir = normalize(vec3(0.1, 0.8, 0.6)); 
    vec3 sun = vec3(1.64, 1.27, 0.99); 
    vec3 skycolor = vec3(0.6, 1.5, 1.0); 

	vec3 bg = exp(uv.y-2.0)*vec3(0.4, 1.6, 1.0);

    float halo=clamp(dot(normalize(vec3(-eye.x, -eye.y, -eye.z)), dirM), 0.0, 1.0);
    vec3 col; 
    //vec3 col=bg+vec3(1.0,0.8,0.4)*pow(halo,17.0); 

    float t=0.0;
    vec3 p=eye; 
	 
	vec3 res = intersect(eye, dirM);
	if(res.x > 0.0){
		p = eye + res.x * dirM;
        vec3 n=nor(p); 
        float shadow = softshadow(p, sundir, 10.0 );

        float dif = max(0.0, dot(n, sundir)); 
        float sky = 0.6 + 0.4 * max(0.0, dot(n, vec3(0.0, 1.0, 0.0))); 
 		float bac = max(0.3 + 0.7 * dot(vec3(-sundir.x, -1.0, -sundir.z), n), 0.0); 
        float spe = max(0.0, pow(clamp(dot(sundir, reflect(dirM, n)), 0.0, 1.0), 10.0)); 

        vec3 lin = 4.5 * sun * dif * shadow; 
        lin += 0.8 * bac * sun; 
        lin += 0.6 * sky * skycolor*shadow; 
        lin += 3.0 * spe * shadow; 

		res.y = pow(clamp(res.y, 0.0, 1.0), 0.55);
		vec3 tc0 = 0.5 + 0.5 * sin(3.0 + 4.2 * res.y + vec3(0.0, 0.5, 1.0));
        col = lin *vec3(0.9, 0.8, 0.6) *  0.2 * tc0;
 		col=mix(col,bg, 1.0-exp(-0.001*res.x*res.x)); 
    } 

    // post
    col=pow(clamp(col,0.0,1.0),vec3(0.45)); 
    col=col*0.6+0.4*col*col*(3.0-2.0*col);  // contrast
    col=mix(col, vec3(dot(col, vec3(0.33))), -0.5);  // satuation
    col*=0.5+0.5*pow(16.0*q.x*q.y*(1.0-q.x)*(1.0-q.y),0.7);  // vigneting
 	outColor = vec4(col.xyz, smoothstep(0.55, .76, 1.-res.x/5.));
#endif
*/

    // gamma
	// col = sqrt( col );
    // vignette
    // vec2 sp = (2.0*pixelCoord-iResolution.xy) / iResolution.y
    // col *= 1.0 - 0.05*length(sp);
} 