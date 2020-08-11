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

#if defined COLORING_TYPE_1 || defined COLORING_TYPE_3
uniform vec3 color;
#endif

#ifdef COLORING_TYPE_2
uniform vec3 color1;
uniform vec3 color2;
uniform vec3 color3;
#endif

uniform float shininess; // показатель степени зеркального отражения
uniform float reflection; // сила отражения

uniform vec4 offset;
uniform float smoothness;

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

vec3 hash3( float n )
{
    return fract(sin(vec3(n,n+1.0,n+2.0))*vec3(43758.5453123,22578.1459123,19642.3490423));
}

//-------------------------------------------------------------------------------------------------------
// square a quaterion
vec4 qSquare(vec4 a)  {
    return vec4( a.x*a.x - a.y*a.y - a.z*a.z - a.w*a.w,
                 2.0*a.x*a.y,
                 2.0*a.x*a.z,
                 2.0*a.x*a.w );
}
/*
vec4 qSquare( vec4 a )
{
    return vec4( a.x*a.x - dot(a.yzw,a.yzw), 2.0*a.x*(a.yzw) );
}
*/

vec4 qCube(vec4 a) {
	return a*(4.0*a.x*a.x - dot(a,a)*vec4(3.0,1.0,1.0,1.0));
}


const int numIterations = 11;

//-------------------------------------------------------------------------------------------------------
// Compute Julia set
// http://iquilezles.org/www/articles/juliasets3d/juliasets3d.htm
// https://iquilezles.org/www/articles/distancefractals/distancefractals.htm
// https://www.shadertoy.com/view/lsl3W2
float julia( vec3 pos, vec4 c, out vec4 trapColor) {
    vec4 z = vec4(pos, 0.2);
	float m2 = 0.0;
	float dz2 = 1.0;

#if defined COLORING_TYPE_1 || defined COLORING_TYPE_2
    vec4 trap = vec4(abs(z.xyz), dot(z, z));
#endif

#ifdef COLORING_TYPE_3
    vec2  trap = vec2(1e10);
#endif


	for( int i = 0; i < numIterations; i++ ) {
        vec4 q = qSquare(z);
        // dz2 *= 9.0*dot(q, q); // |dz|² = |3z²|²	
		dz2 *= smoothness*dot(q, q); // |dz|² = |3z²|²		
		z = qCube(z) + c; // z = z^3 + c
		
        // stop under divergence		
        m2 = dot(z, z);		
        if (m2 > 10000.0) break;	
        //if (m2 > 10.0) break;	

    #if defined COLORING_TYPE_1 || defined COLORING_TYPE_2
        trap = min(trap, vec4(abs(z.xyz), dot(z, z)));  // trapping Oxz, Oyz, Oxy, (0,0,0)
    #endif

    #ifdef COLORING_TYPE_3
        trap = min(trap, vec2(m2, abs(z.x))); // orbit trapping ( |z|² and z_x  )
    #endif			 

	}

#if defined COLORING_TYPE_1 || defined COLORING_TYPE_2
    trapColor = trap;
#endif

#ifdef COLORING_TYPE_3
    trapColor = vec4(trap, 1.0, 1.0);
#endif

	return 0.25*log(m2)*sqrt(m2/dz2); // distance estimator: d(z) = 0.5·log|z|·|z|/|dz| 
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

/*
vec3 raycast( in vec3 ro, in vec3 rd, in vec4 c ) {
	float maxd = 8.0;
	float precis = 0.002;
    float h = 1.0;
    float t = 0.0;
	float d = 0.0;
    float m = 1.0;
    for( int i=0; i<150; i++ ) {
        if( h<precis||t>maxd ) break;
        t += h;
	    vec3 res = map( ro+rd*t, c );
        h = res.x;
		d = res.y;
		m = res.z;
    }

    if( t>maxd ) m=-1.0;
    return vec3( t, d, m );
}
*/

/*
vec3 calcPixel(vec2 pi, float time ) {
    vec4 c = vec4(-0.1,0.6,0.9,-0.3) + 0.1*sin( vec4(3.0,0.0,1.0,2.0) + 0.5*vec4(1.0,1.3,1.7,2.1)*iTime);

	vec2 q = pi / iResolution.xy;
    vec2 p = -1.0 + 2.0*q;
    p.x *= iResolution.x/iResolution.y;
    vec2 m = vec2(0.5);
	//if(iMouse.z > 0.0) m = iMouse.xy/iResolution.xy;

    // camera
	float an = -2.4 + 0.2*time - 6.2*m.x;
    vec3 ro = 4.0*vec3(sin(an),0.25,cos(an));
    vec3 ta = vec3( 0.0, 0.08, 0.0 );
    vec3 ww = normalize( ta - ro );
    vec3 uu = normalize( cross(ww,vec3(0.0,1.0,0.0) ) );
    vec3 vv = normalize( cross(uu,ww));
    vec3 rd = normalize( p.x*uu + p.y*vv + 4.1*ww );

	// raymarch
    vec3 tmat = raycast(ro,rd,c);
	
	// shade
    vec3 col = vec3(0.0);
    if( tmat.z>-0.5 )
    {
        // geometry
        vec3 pos = ro + tmat.x*rd;
        vec3 nor = calcNormal(pos, 0.001,c);
        vec3 sor = calcNormal(pos, 0.01,c);

        // material		
		vec3 mate = 0.5 + 0.5*sin( tmat.z*4.0 + 4.0 + vec3(3.0,1.5,2.0)  + nor*0.2 ).xzy;
		
        // lighting		
		float occ = clamp( tmat.y*0.5 + 0.5*(tmat.y*tmat.y), 0.0, 1.0 ) * (1.0 + 0.1*nor.y);
		
        // diffuse		
		col = vec3(0.0);
		for( int i=0; i<32; i++ )
		{
			vec3 rr = normalize(-1.0 + 2.0*hash3(float(i)*123.5463));
			rr = normalize( nor + 8.0*rr );
			rr = rr * sign(dot(nor,rr));							  
            col += pow( texture( iChannel0, rr ).xyz, vec3(2.2) ) * dot(rr,nor);
		}
        col = 5.0 * occ * (col/32.0);

        // rim		
		col *= 1.0 + 1.0*pow(clamp(1.0+dot(rd,sor),0.0,1.0),1.0)*vec3(1.0);

        // specular		 
		float fre = pow( clamp(1.0+dot(rd,sor),0.0,1.0), 5.0 );
		vec3  ref = reflect( rd, nor );
		col *= 1.0 - 0.5*fre; 
		col += 1.5 * (0.5 + 0.5*fre) * pow( texture( iChannel0, ref ).xyz, vec3(2.0) ) * occ;

        col *= mate;
    }
	else {
        // background		
		col = pow( texture( iChannel0, rd ).xyz, vec3(2.2) );
	}

	col = pow( clamp( col, 0.0, 1.0 ), vec3(0.45) ); // gamma
    return col;
}
*/

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
        float intensity = (lightIntensity1 + lightIntensity2 + ambientLightIntensity3)*0.1;
#if defined SKYBOX_BACKGROUND_HDR && defined IRRADIANCE_CUBEMAP
    intensity += 0.5;
#endif

#if defined SKYBOX_BACKGROUND || defined SKYBOX_BACKGROUND_HDR
        return texture(skyBox, dir)*intensity;
#endif

#ifdef SOLID_BACKGROUND
        return vec4(reflectedColor - (dir.y * 0.7), 1.0)*intensity; // Skybox color
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
        vec3 albedo = 0.5 + 0.5*sin(trap.y*4.0 + 4.0 + color + outNormal*0.2).xzy;
    #endif
        // material		
		//albedo = 0.5 + 0.5*sin(trap.y*4.0 + 4.0 + vec3(3.0,1.5,2.0)  + outNormal*0.2).xzy;
        		
        //vec3 albedo = color*0.3;
		//albedo.x = 1.0-10.0*trap.x; // !!!!!!!!!!!!!!!! добавить еще эту окраску !!!!!!!!!!!!!!!!!!!!!!!

		//float occlusion = clamp(2.5*trap.w - 0.15, 0.0, 1.0);
        float occlusion = clamp(trap.x*0.5 + 0.5*(trap.x*trap.x), 0.0, 1.0) * (1.0 + 0.1*outNormal.y);
        vec3 hal = normalize(lightDirection1 - dir);
        float shadow = 1.0;

    #ifdef FLAG_SOFT_SHADOWS
        vec3 shadowRayOrigin = point + 0.001*outNormal;
        vec3 shadowRayDir = normalize(lightDirection1); // луч, направленный на источник света
        shadow = softShadow(shadowRayOrigin, shadowRayDir, MIN_DIST, MAX_DIST, shadowStrength, c);
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

        col += render(eye, dir, offset, sp );
    }
    col /= float(AA*AA);
    
    
    outColor = col;
}