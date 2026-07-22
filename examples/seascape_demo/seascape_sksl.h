const char* SEASCAPE_SKSL = R"(
uniform float u_time;
uniform float2 u_resolution;
/*
 * "Seascape" by Alexander Alekseev aka TDM - 2014
 * License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
 * Contact: tdmaav@gmail.com
 */

const int NUM_STEPS = 32;
const float PI	 	= 3.141592;
const float EPSILON	= 1e-3;

// sea
const int ITER_GEOMETRY = 3;
const int ITER_FRAGMENT = 5;
const float SEA_HEIGHT = 0.6;
const float SEA_CHOPPY = 4.0;
const float SEA_SPEED = 0.8;
const float SEA_FREQ = 0.16;





// math
float3x3 fromEuler(float3 ang) {
	float2 a1 = float2(sin(ang.x),cos(ang.x));
    float2 a2 = float2(sin(ang.y),cos(ang.y));
    float2 a3 = float2(sin(ang.z),cos(ang.z));
    float3x3 m;
    m[0] = float3(a1.y*a3.y+a1.x*a2.x*a3.x,a1.y*a2.x*a3.x+a3.y*a1.x,-a2.y*a3.x);
	m[1] = float3(-a2.y*a1.x,a1.y*a2.y,a2.x);
	m[2] = float3(a3.y*a1.x*a2.x+a1.y*a3.x,a1.x*a3.x-a1.y*a3.y*a2.x,a2.y*a3.y);
	return m;
}
float hash( float2 p ) {
	float h = dot(p,float2(127.1,311.7));	
    return fract(sin(h)*43758.5453123);
}
float noise( in float2 p ) {
    float2 i = floor( p );
    float2 f = fract( p );	
	float2 u = f*f*(3.0-2.0*f);
    return -1.0+2.0*mix( mix( hash( i + float2(0.0,0.0) ), 
                     hash( i + float2(1.0,0.0) ), u.x),
                mix( hash( i + float2(0.0,1.0) ), 
                     hash( i + float2(1.0,1.0) ), u.x), u.y);
}

// lighting
float diffuse(float3 n,float3 l,float p) {
    return pow(dot(n,l) * 0.4 + 0.6,p);
}
float specular(float3 n,float3 l,float3 e,float s) {    
    float nrm = (s + 8.0) / (PI * 8.0);
    return pow(max(dot(reflect(e,n),l),0.0),s) * nrm;
}

// sky
float3 getSkyColor(float3 e) {
    e.y = (max(e.y,0.0)*0.8+0.2)*0.8;
    return float3(pow(1.0-e.y,2.0), 1.0-e.y, 0.6+(1.0-e.y)*0.4) * 1.1;
}

// sea
float sea_octave(float2 uv, float choppy) {
    uv += noise(uv);        
    float2 wv = 1.0-abs(sin(uv));
    float2 swv = abs(cos(uv));    
    wv = mix(wv,swv,wv);
    return pow(1.0-pow(wv.x * wv.y,0.65),choppy);
}

float map(float3 p) {
    float freq = SEA_FREQ;
    float amp = SEA_HEIGHT;
    float choppy = SEA_CHOPPY;
    float2 uv = p.xz; uv.x *= 0.75;
    
    float d, h = 0.0;    
    for(int i = 0; i < ITER_GEOMETRY; i++) {        
    	d = sea_octave((uv+(1.0 + u_time * SEA_SPEED))*freq,choppy);
    	d += sea_octave((uv-(1.0 + u_time * SEA_SPEED))*freq,choppy);
        h += d * amp;        
    	uv *= float2x2(1.6,1.2,-1.2,1.6); freq *= 1.9; amp *= 0.22;
        choppy = mix(choppy,1.0,0.2);
    }
    return p.y - h;
}

float map_detailed(float3 p) {
    float freq = SEA_FREQ;
    float amp = SEA_HEIGHT;
    float choppy = SEA_CHOPPY;
    float2 uv = p.xz; uv.x *= 0.75;
    
    float d, h = 0.0;    
    for(int i = 0; i < ITER_FRAGMENT; i++) {        
    	d = sea_octave((uv+(1.0 + u_time * SEA_SPEED))*freq,choppy);
    	d += sea_octave((uv-(1.0 + u_time * SEA_SPEED))*freq,choppy);
        h += d * amp;        
    	uv *= float2x2(1.6,1.2,-1.2,1.6); freq *= 1.9; amp *= 0.22;
        choppy = mix(choppy,1.0,0.2);
    }
    return p.y - h;
}

float3 getSeaColor(float3 p, float3 n, float3 l, float3 eye, float3 dist) {  
    float fresnel = clamp(1.0 - dot(n, -eye), 0.0, 1.0);
    fresnel = min(fresnel * fresnel * fresnel, 0.5);
    
    float3 reflected = getSkyColor(reflect(eye, n));    
    float3 refracted = float3(0.0,0.09,0.18) + diffuse(n, l, 80.0) * (float3(0.8,0.9,0.6)*0.6) * 0.12; 
    
    float3 color = mix(refracted, reflected, fresnel);
    
    float atten = max(1.0 - dot(dist, dist) * 0.001, 0.0);
    color += (float3(0.8,0.9,0.6)*0.6) * (p.y - SEA_HEIGHT) * 0.18 * atten;
    
    color += specular(n, l, eye, 600.0 * inversesqrt(dot(dist,dist)));
    
    return color;
}

// tracing
float3 getNormal(float3 p, float eps) {
    float3 n;
    n.y = map_detailed(p);    
    n.x = map_detailed(float3(p.x+eps,p.y,p.z)) - n.y;
    n.z = map_detailed(float3(p.x,p.y,p.z+eps)) - n.y;
    n.y = eps;
    return normalize(n);
}

float heightMapTracing(float3 ori, float3 dir, out float3 p) {  
    float tm = 0.0;
    float tx = 1000.0;    
    float hx = map(ori + dir * tx);
    if(hx > 0.0) {
        p = ori + dir * tx;
        return tx;   
    }
    float hm = map(ori);    
    for(int i = 0; i < NUM_STEPS; i++) {
        float tmid = mix(tm, tx, hm / (hm - hx));
        p = ori + dir * tmid;
        float hmid = map(p);        
        if(hmid < 0.0) {
            tx = tmid;
            hx = hmid;
        } else {
            tm = tmid;
            hm = hmid;
        }        
        if(abs(hmid) < EPSILON) break;
    }
    return mix(tm, tx, hm / (hm - hx));
}

float3 getPixel(in float2 coord, float time) {    
    float2 uv = coord / u_resolution.xy;
    uv = uv * 2.0 - 1.0;
    uv.x *= u_resolution.x / u_resolution.y;    
        
    // ray
    float3 ang = float3(sin(time*3.0)*0.1,sin(time)*0.2+0.3,time);    
    float3 ori = float3(0.0,3.5,time*5.0);
    float3 dir = normalize(float3(uv.xy,-2.0)); dir.z += length(uv) * 0.14;
    dir = normalize(dir) * fromEuler(ang);
    
    // tracing
    float3 p;
    heightMapTracing(ori,dir,p);
    float3 dist = p - ori;
    float3 n = getNormal(p, dot(dist,dist) * (0.1 / u_resolution.x));
    float3 light = normalize(float3(0.0,1.0,0.8)); 
             
    // color
    return mix(
        getSkyColor(dir),
        getSeaColor(p,n,light,dir,dist),
    	pow(smoothstep(0.0,-0.02,dir.y),0.2));
}

// main
half4 main(float2 fragCoord) {
    fragCoord.y = u_resolution.y - fragCoord.y;
    float time = u_time * 0.3 + 0.0*0.01;
	
float3 color = getPixel(fragCoord, time);
    
    // post
	return half4(pow(color,float3(0.65)), 1.0);
}
)";
