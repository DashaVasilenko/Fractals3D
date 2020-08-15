in mat4 viewMatrix;
out vec4 outColor;

uniform vec2 iResolution; 
uniform float fieldOfView;

uniform float Time;

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

//uniform float shininess; // показатель степени зеркального отражения
//uniform float reflection; // сила отражения

uniform float offset1;
uniform float offset2;
uniform int iterations;

//const int MAX_MARCHING_STEPS = 255;
const int MAX_MARCHING_STEPS = 256;
const float MIN_DIST = 0.01;
const float MAX_DIST = 20.0; //50
const float EPSILON = 0.001; //0.0005;

// antialias level (1, 2, 3...)
#define AA 1
//#else
//#define AA 2  // Set AA to 1 if your machine is too slow
//#endif
             
//-------------------------------------------------------------------------------------------------------
// Compute Apollonian/kleinian fractal
// https://www.shadertoy.com/view/llKXzh
float apollonian(vec3 pos, float s, out vec4 trapColor) {
    /*
    float scale = 1.0;
	float add = sin(iTime)*.2+.1;

	for( int i=0; i < 9;i++ )
	{
		p = 2.0*clamp(p, -CSize, CSize) - p;
		float r2 = dot(p,p);
		float k = max((1.15)/r2, 1.);
		p     *= k;
		scale *= k;
	}
	float l = length(p.xy);
	float rxy = l - 4.0;
	float n = l * p.z;
	rxy = max(rxy, -(n) / (length(p))-.07+sin(iTime*2.0+p.x+p.y+23.5*p.z)*.02);
    float x = (1.+sin(iTime*2.));x =x*x*x*x*.5;
    float h = dot(sin(p*.013),(cos(p.zxy*.191)))*x;
	return ((rxy+h) / abs(scale));
    */

        float scale = 1.0;
        float add = sin(Time)*0.2 + 0.1;
        vec3 CSize = vec3(.808, .8, 1.137);


#if defined COLORING_TYPE_1 || defined COLORING_TYPE_2 || defined COLORING_TYPE_4 || defined COLORING_TYPE_5 || defined COLORING_TYPE_7
    //vec4 trap = vec4(abs(pos), dot(pos, pos));
    vec4 trap = vec4(1000.0);
#endif

#if defined COLORING_TYPE_3 || defined COLORING_TYPE_6
    vec2  trap = vec2(1e10);
#endif
	
    //for (int i = 0; i < 6; i++ ) {
    for (int i = 0; i < iterations; i++) {
        //pos = -1.0 + 2.0*fract(0.5*pos + 0.5);
        //pos -= sign(pos)*0.04; // trick
        pos = 2.0*clamp(pos, -CSize, CSize) - pos;
        float r2 = dot(pos, pos);

        //trap = min(trap, r2); trap - float
/*
        p = 2.0*clamp(p, -CSize, CSize) - p;
		float r2 = dot(p,p);
		float k = max((1.15)/r2, 1.);
		p     *= k;
		scale *= k;
*/
    #if defined COLORING_TYPE_1 || defined COLORING_TYPE_2 || defined COLORING_TYPE_4 || defined COLORING_TYPE_5 || defined COLORING_TYPE_7
        trap = min(trap, vec4(abs(pos), r2));  // trapping Oxz, Oyz, Oxy, (0,0,0)
    #endif
 
    #if defined COLORING_TYPE_3 || defined COLORING_TYPE_6
        trap = min(trap, vec2(r2, abs(pos.x))); // orbit trapping ( |z|² and z_x  )
    #endif
        
        //float k = 0.95/r2;
        //float k = s/r2;
        float k = max(s/r2, 1.0);
		pos *= k;
		scale *= k;
	}

    float l = length(pos.xy);
	float rxy = l - 4.0;
	float n = l * pos.z;
	rxy = max(rxy, -(n) / (length(pos))-.07+sin(Time*2.0+pos.x+pos.y+23.5*pos.z)*.02);
    float x = (1.+sin(Time*2.)); x =x*x*x*x*.5;
    float h = dot(sin(pos*.013),(cos(pos.zxy*.191)))*x;

    //float d1 = sqrt( min( min( dot(pos.xy, pos.xy), dot(pos.yz, pos.yz) ), dot(pos.zx, pos.zx) ) ) - 0.02;
    //float d2 = abs(pos.y);
    //float dmi = d2;
    //float adr = 0.7*floor((0.5*pos.y + 0.5)*8.0);
    //if (d1 < d2) {
    //    dmi = d1;
    //    adr = 0.0;
    //}

 #if defined COLORING_TYPE_1 || defined COLORING_TYPE_2 || defined COLORING_TYPE_4 || defined COLORING_TYPE_5 || defined COLORING_TYPE_7
    trapColor = trap;
#endif

#if defined COLORING_TYPE_3 || defined COLORING_TYPE_6
    trapColor = vec4(trap, adr, 1.0);
#endif

    return ((rxy+h) / abs(scale));
    //return 0.5*dmi/scale;
    //return vec3( 0.5*dmi/scale, adr, orb );



/*
    float scale = 1.0;

#if defined COLORING_TYPE_1 || defined COLORING_TYPE_2 || defined COLORING_TYPE_4 || defined COLORING_TYPE_5 || defined COLORING_TYPE_7
    //vec4 trap = vec4(abs(pos), dot(pos, pos));
    vec4 trap = vec4(1000.0);
#endif

#if defined COLORING_TYPE_3 || defined COLORING_TYPE_6
    vec2  trap = vec2(1e10);
#endif
	
    //for (int i = 0; i < 6; i++ ) {
    for (int i = 0; i < iterations; i++) {
        pos = -1.0 + 2.0*fract(0.5*pos + 0.5);
        pos -= sign(pos)*0.04; // trick
        float r2 = dot(pos, pos);

        //trap = min(trap, r2); trap - float

    #if defined COLORING_TYPE_1 || defined COLORING_TYPE_2 || defined COLORING_TYPE_4 || defined COLORING_TYPE_5 || defined COLORING_TYPE_7
        trap = min(trap, vec4(abs(pos), r2));  // trapping Oxz, Oyz, Oxy, (0,0,0)
    #endif
 
    #if defined COLORING_TYPE_3 || defined COLORING_TYPE_6
        trap = min(trap, vec2(r2, abs(pos.x))); // orbit trapping ( |z|² and z_x  )
    #endif
        
        //float k = 0.95/r2;
        float k = s/r2;
		pos *= k;
		scale *= k;
	}

    float d1 = sqrt( min( min( dot(pos.xy, pos.xy), dot(pos.yz, pos.yz) ), dot(pos.zx, pos.zx) ) ) - 0.02;
    float d2 = abs(pos.y);
    float dmi = d2;
    float adr = 0.7*floor((0.5*pos.y + 0.5)*8.0);
    if (d1 < d2) {
        dmi = d1;
        adr = 0.0;
    }

 #if defined COLORING_TYPE_1 || defined COLORING_TYPE_2 || defined COLORING_TYPE_4 || defined COLORING_TYPE_5 || defined COLORING_TYPE_7
    trapColor = trap;
#endif

#if defined COLORING_TYPE_3 || defined COLORING_TYPE_6
    trapColor = vec4(trap, adr, 1.0);
#endif

    return 0.5*dmi/scale;
    //return vec3( 0.5*dmi/scale, adr, orb );
*/
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
vec3 computeNormal(vec3 p, float s) {
    vec4 trap;
    const float h = 0.0001; // replace by an appropriate value
    const vec2 k = vec2(1,-1)*h;
    return normalize( k.xyy*apollonian( p + k.xyy, s, trap) + 
                      k.yyx*apollonian( p + k.yyx, s, trap) + 
                      k.yxy*apollonian( p + k.yxy, s, trap) + 
                      k.xxx*apollonian( p + k.xxx, s, trap) );
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
        //if (depth >= dist) break; // Gone too far; give up
        if (depth >= end) break; // Gone too far; give up
        depth += h; // Move along the view ray
    }
    
    if (depth > end) return -1.0;
    if (depth < end) { 
        trapColor = trap;
        res = depth;
    }
    return res;
}

vec3 forwardSF(float i, float n) {
    const float PI  = 3.141592653589793238;
    const float PHI = 1.618033988749894848;
    float phi = 2.0*PI*fract(i/PHI);
    float zi = 1.0 - (2.0*i + 1.0)/n;
    float sinTheta = sqrt( 1.0 - zi*zi);
    return vec3(cos(phi)*sinTheta, sin(phi)*sinTheta, zi);
}

float calcAO(vec3 pos, vec3 n, float s) {
	float ao = 0.0;
    vec4 t;
    for (int i = 0; i < 16; i++ ) {
        vec3 w = forwardSF(float(i), 16.0);
		w *= sign( dot(w, n) );
        float h = float(i)/15.0;
        ao += clamp(apollonian(pos + n*0.01 + w*h*0.15, s, t)*2.0, 0.0, 1.0);
    }
	ao /= 16.0;
	
    return clamp(ao*16.0, 0.0, 1.0);
}

//-------------------------------------------------------------------------------------------------------
vec4 render(vec3 eye, vec3 dir, vec2 sp, float s ) {
    //float r = reflection;
	vec4 trap;  
    float dist = shortestDistanceToSurface(eye, dir, MIN_DIST, MAX_DIST, s, trap); // intersect fractal

    vec3 point = eye + dist*dir; // The closest point on the surface to the eyepoint along the view ray
    vec3 outNormal = computeNormal(point, s); // N
    vec3 col = vec3(0.0);
        
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
        vec3 albedo = 0.5 + 0.5*cos(6.2831*trap.x + color);
    #endif
    #ifdef COLORING_TYPE_7
        vec3 albedo = color1;
        albedo = mix(albedo, color2, clamp(6.0*trap.y, 0.0, 1.0));
        albedo = mix(albedo, color3, pow(clamp(1.0 - 2.0*trap.z, 0.0, 1.0), 8.0));

        //vec3 albedo = vec3(1.0);
        //albedo = mix(albedo, vec3(1.0, 0.80, 0.2), clamp(6.0*trap.y, 0.0, 1.0));
        //albedo = mix(albedo, vec3(1.0, 0.55, 0.0), pow(clamp(1.0 - 2.0*trap.z, 0.0, 1.0), 8.0));
    #endif
        		
        float occlusion = pow(clamp(trap.x*2.0, 0.0, 1.0), 1.2);
        occlusion = 1.5*(0.1 + 0.9*occlusion)*calcAO(point, outNormal, s);
        //float occlusion = pow(clamp(trap.w*2.0, 0.0, 1.0), 1.2);
        //vec3 hal = normalize(lightDirection1 - dir);

        float dif1 = clamp(dot(lightDirection1, outNormal), 0.0, 1.0 )*occlusion; // light1
        //float s = shininess;
        //float spe1 = pow(clamp(dot(outNormal, hal), 0.0, 1.0), shininess)*dif1*(0.04 + 0.96*pow(clamp(dot(dir, lightDirection1), 0.0, 1.0), 5.0));
        float dif2 = clamp(0.2 + 0.8*dot(lightDirection2, outNormal), 0.0, 1.0)*occlusion; // bounce
        
        vec3 lin = vec3(0.0); 
		     lin += lightIntensity1*lightColor1*dif1; // light1
		     lin += lightIntensity2*lightColor2*dif2; //light2
             lin +=  ambientLightIntensity3*ambientLightColor3*(0.7 + 0.3*outNormal.y)*occlusion; // ambient light
        col = albedo*lin*exp(-0.3*dist);    
        //col *= exp(-0.3*dist);
        //col = albedo*lin*exp(-0.2*dist);
		//col = pow(col, vec3(0.7, 0.9, 1.0));
        //col += spe1*15.0;
        //col += spe1*lightIntensity1;  

        // linear tone mapping
        //float exposure = 1.0;
	    //col = clamp(exposure*col, 0.0, 1.0);
	    //col = pow(col, vec3(1.0 / 2.2));

        // luma based Reinhard tone mapping
	    float luma = dot(col, vec3(0.2126, 0.7152, 0.0722));
	    float toneMappedLuma = luma / (1.0 + luma);
	    col *= toneMappedLuma / luma;
	    col = pow(col, vec3(1.0 / 2.2)); // gamma

        // RomBinDaHouse tone mapping
        //col = exp(-1.0/(2.72*col + 0.15));
	    //col = pow(col, vec3(1.0/2.2)); // gamma

    }
    return vec4(col, 1.0);
    //return vec4(sqrt(col), 1.0);
}

void main() {
    float t = Time;
    
    vec2 pixelCoord = vec2(gl_FragCoord.x, gl_FragCoord.y);
    vec3 dir = rayDirection(fieldOfView, iResolution, pixelCoord);
    vec3 eye = viewMatrix[3].xyz;
    vec2  sp = (2.0*pixelCoord-iResolution.xy) / iResolution.y;

    //float time = Time*0.25;
    //float c = 1.1 + 0.5*smoothstep( -0.3, 0.3, cos(0.1*time) );
    float c = offset2 + 0.5*smoothstep( -0.3, 0.3, cos(0.1*offset1) );
    
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

        col += render(eye, dir, sp, c);
    }
    col /= float(AA*AA);

    /*
    // это АА для этого фрактала
    float time = iTime*0.15 + 0.005*iMouse.x;
    
    vec3 tot = vec3(0.0);
    #if AA>1
    for( int jj=0; jj<AA; jj++ )
    for( int ii=0; ii<AA; ii++ )
    #else
    int ii = 0, jj = 0;
    #endif
    {
        vec2 q = fragCoord+vec2(float(ii),float(jj))/float(AA);

        // camera
        vec3 ro = vec3( 2.8*cos(0.1+.33*time), 0.5 + 0.20*cos(0.37*time), 2.8*cos(0.5+0.35*time) );
        vec3 ta = vec3( 1.9*cos(1.2+.41*time), 0.5 + 0.10*cos(0.27*time), 1.9*cos(2.0+0.38*time) );
        float roll = 0.2*cos(0.1*time);
        vec3 cw = normalize(ta-ro);
        vec3 cp = vec3(sin(roll), cos(roll),0.0);
        vec3 cu = normalize(cross(cw,cp));
        vec3 cv = normalize(cross(cu,cw));

        #if 1
        vec2 p = (2.0*q-iResolution.xy)/iResolution.y;
        vec3 rd = normalize( p.x*cu + p.y*cv + 2.0*cw );
        #else
        vec2 p = q/iResolution.xy;
        vec2 an = 3.1415926535898 * (p*vec2(2.0, 1.0) - vec2(0.0,0.5));
        vec3 rd = vec3(cos(an.y) * sin(an.x), sin(an.y), cos(an.y) * cos(an.x));
		#endif

        tot += render( ro, rd );
    }
    
    tot = tot/float(AA*AA);
    
	fragColor = vec4( tot, 1.0 );	
    */
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