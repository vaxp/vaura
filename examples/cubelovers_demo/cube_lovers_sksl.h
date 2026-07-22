const char* CUBE_LOVERS_SKSL = R"(
uniform float u_time;
uniform float2 u_resolution;

const float PI = 3.141592654;
const float TAU = 6.283185307;
const float LAYERS = 5.0;
const float TOLERANCE = 0.0001;
const float MAX_RAY_LENGTH = 120.0;
const int MAX_RAY_MARCHES_LO = 30;
const int MAX_RAY_MARCHES_HI = 70;
const float NORM_OFF = 0.005;
float4 roadDim() { return float4(normalize(float3(0.0, 1.0, 0.15)), 20.0); }
float2x2 ROT(float a) { return float2x2(cos(a), sin(a), -sin(a), cos(a)); }
// CC0: More "cubes" for the cube lovers
//  Tinkering around with glow effects and one bounce reflections
//  Produced a few interesting "cubes" that some might enjoy.

// Song : Rush Connection - Culture Shock

// Try different approximations of "cubes" by changing between DF0-DF7

// Some "cubes" can be more or less boxy
//  define or comment out
// #define BOXY
// Some "cubes" have flair variants
//  define or comment out


















// License: WTFPL, author: sam hocevar, found: https://stackoverflow.com/a/17897228/418488
const float4 hsv2rgb_K = float4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);

float3 HSV2RGB(float3 c) {
  float3 p = abs(fract(c.xxx + float3(1.0, 2.0/3.0, 1.0/3.0)) * 6.0 - float3(3.0));
  return c.z * mix(float3(1.0), clamp(abs(fract(c.xxx + float3(1.0, 2.0/3.0, 1.0/3.0)) * 6.0 - float3(3.0)) - float3(1.0), 0.0, 1.0), c.y);
}
float3 glowCol1() { return HSV2RGB(float3(-0.025+0.65, 0.75, 0.2)); }
float3 sunCol1()  { return HSV2RGB(float3(-0.025+0.75, 0.50, 0.5)); }
float3 diffCol()  { return HSV2RGB(float3(-0.025+0.40, 0.75, 0.125)); }
float3 sunDir1()  { return normalize(float3(3.0, 3.0, -7.0)); }

float3 hsv2rgb(float3 c) {
  float3 p = abs(fract(c.xxx + hsv2rgb_K.xyz) * 6.0 - hsv2rgb_K.www);
  return c.z * mix(hsv2rgb_K.xxx, clamp(p - hsv2rgb_K.xxx, 0.0, 1.0), c.y);
}



// License: Unknown, author: Matt Taylor (https://github.com/64), found: https://64.github.io/tonemapping/
float3 aces_approx(float3 v) {
  v = max(v, 0.0);
  v *= 0.6;
  float a = 2.51;
  float b = 0.03;
  float c = 2.43;
  float d = 0.59;
  float e = 0.14;
  return clamp((v*(a*v+b))/(v*(c*v+d)+e), 0.0, 1.0);
}

// License: Unknown, author: Unknown, found: don't remember
float hash(float co) {
  return fract(sin(co*12.9898) * 13758.5453);
}


// License: Unknown, author: Unknown, found: don't remember
float2 hash2(float2 p) {
  p = float2(dot (p, float2 (127.1, 311.7)), dot (p, float2 (269.5, 183.3)));
  return fract(sin(p)*43758.5453123);
}

// License: CC BY-NC-SA 3.0, author: Stephane Cuillerdier - Aiekick/2015 (twitter:@aiekick), found: https://www.shadertoy.com/view/Mt3GW2
float3 blackbody(float Temp) {
  float3 col = float3(255.);
  col.x = 56100000. * pow(Temp,(-3. / 2.)) + 148.;
  col.y = 100.04 * log(Temp) - 623.6;
  if (Temp > 6500.) col.y = 35200000. * pow(Temp,(-3. / 2.)) + 184.;
  col.z = 194.18 * log(Temp) - 1448.6;
  col = clamp(col, 0., 255.)/255.;
  if (Temp < 1000.) col *= Temp/1000.;
  return col*col;
}

// License: Unknown, author: Unknown, found: don't remember
float3 tanh3(float3 x) { float3 x2 = x*x; return clamp(x*(27.0 + x2)/(27.0+9.0*x2), -1.0, 1.0); }
float tanh_approx(float x) {
//  return tanh(x);
  float x2 = x*x;
  return clamp(x*(27.0 + x2)/(27.0+9.0*x2), -1.0, 1.0);
}

// License: MIT, author: Inigo Quilez, found: https://iquilezles.org/articles/smin
float pmin(float a, float b, float k) {
    float h = clamp( 0.5+0.5*(b-a)/k, 0.0, 1.0 );
    return mix( b, a, h ) - k*h*(1.0-h);
}

// License: MIT, author: Inigo Quilez, found: https://iquilezles.org/articles/smin
float3 pmin(float3 a, float3 b, float k) {
    float3 h = clamp( 0.5+0.5*(b-a)/k, 0.0, 1.0 );
    return mix( b, a, h ) - k*h*(1.0-h);
}

float pmax(float a, float b, float k) {
  return -pmin(-a, -b, k);
}

float3 pabs(float3 a, float k) {
  return -pmin(a, -a, k);
}

// License: MIT OR CC-BY-NC-4.0, author: mercury, found: https://mercury.sexy/hg_sdf/
float mod1(inout float p, float size) {
  float halfsize = size*0.5;
  float c = floor((p + halfsize)/size);
  p = mod(p + halfsize, size) - halfsize;
  return c;
}

// License: MIT OR CC-BY-NC-4.0, author: mercury, found: https://mercury.sexy/hg_sdf/
float2 mod2(inout float2 p, float2 size) {
  float2 c = floor((p + size*0.5)/size);
  p = mod(p + size*0.5,size) - size*0.5;
  return c;
}

// License: MIT, author: Inigo Quilez, found: https://iquilezles.org/articles/intersectors
float rayPlane(float3 ro, float3 rd, float4 p) {
  return -(dot(ro,p.xyz)+p.w)/dot(rd,p.xyz);
}

float circle(float2 p, float r) {
  return length(p) - r;
}

// License: MIT, author: Inigo Quilez, found: www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
float torus(float3 p, float2 t) {
  float2 q = float2(length(p.xz)-t.x,p.y);
  return length(q)-t.y;
}

// License: MIT OR CC-BY-NC-4.0, author: mercury, found: https://mercury.sexy/hg_sdf/
float3 mod3(inout float3 p, float3 size) {
  float3 c = floor((p + size*0.5)/size);
  p = mod(p + size*0.5,size) - size*0.5;
  return c;
}

// License: MIT, author: Inigo Quilez, found: www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
float box(float3 p, float3 b) {
  float3 q = abs(p) - b;
  return length(max(q,0.0)) + min(max(q.x,max(q.y,q.z)),0.0);
}

// License: MIT, author: Inigo Quilez, found: www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
float boxf(float3 p, float3 b, float e) {
       p = abs(p  )-b;
  float3 q = abs(p+e)-e;
  return min(min(
      length(max(float3(p.x,q.y,q.z),0.0))+min(max(p.x,max(q.y,q.z)),0.0),
      length(max(float3(q.x,p.y,q.z),0.0))+min(max(q.x,max(p.y,q.z)),0.0)),
      length(max(float3(q.x,q.y,p.z),0.0))+min(max(q.x,max(q.y,p.z)),0.0));
}

// Intentionally bounded, not exact
float bbox(float3 p, float3 b) {
  float3 q = abs(p) - b;
  return (max(q.x,max(q.y,q.z)));
}

float segmentx(float2 p) {
  float d0 = abs(p.y);
  float d1 = length(p);
  return p.x > 0.0 ? d0 : d1;
}

float segmentx(float2 p, float l) {
  float hl = 0.5*l;
  p.x = abs(p.x);
  float d0 = abs(p.y);
  float d1 = length(p-float2(hl, 0.0));
  return p.x > hl ? d1 : d0;
}

float sphere(float3 p, float r) {
  return length(p) - r;
}

float sphere4(float3 p, float r) {
  p *= p;
  return pow(dot(p, p), 0.25) - r;
}

float sphere8(float3 p, float r) {
  p *= p;
  p *= p;
  return pow(dot(p, p), 0.125) - r;
}

float3 toSpherical(float3 p) {
  float r   = length(p);
  float t   = acos(p.z/r);
  float ph  = atan(p.y, p.x);
  return float3(r, t, ph);
}

float sun(float2 p) {
  const float ch = 0.0125;
  float2 sp = p;
  float d0 = circle(sp, 0.5);
  float d = d0;
  return d;
}

float synth(float2 p, float aa, out float h, out float db) { h = 0.0; db = 0.0;
  const float z = 75.0;
  p.y -= -70.0;
  const float st = 0.04;
  p.x = abs(p.x);
  p.x -= 20.0-3.5;
  p.x += st*20.0;
  p /= z;
  float n = mod1(p.x, st);
  float dib = 1E6;
  const int around = 0;
  for (int i = -around; i <=around ;++i) {
    float fft = 0.3 + 0.5 * sin(u_time * 5.0 + (n+float(i))*0.5); 
    fft *= fft;
    if (i == 0) h = fft;
    float dibb = segmentx((p-float2(st*float(i), 0.0)).yx, fft+0.05)-st*0.4;
    dib = min(dib, dibb);
  }
  
  float d = dib;
  db = abs(p.y)*z;
  return smoothstep(aa, -aa, d*z);
}

float3 road(float3 ro, float3 rd, float3 nrd, float glare, out float pt) {
  const float szoom   = 0.5;
  const float bsz     = 25.0;
  const float sm      = 1.0;
  float off = abs(roadDim().w);
  float t = rayPlane(ro, rd, roadDim());
  pt = t;

  float3 p  = ro+rd*t;
  float3 np = ro+nrd*t;

  float2 pp   = p.xz;
  float2 npp  = np.xz;
  float2 opp  = pp;

  float aa  = length(npp-pp)*sqrt(0.5);
  pp.y += -60.0*u_time;

  float3 gcol = float3(0.0);

  float dr = abs(pp.x)-off;
  float2 cp = pp;
  mod1(cp.y, 6.0*off);
  float2 sp = pp;
  sp.x = abs(sp.x);
  mod1(sp.y, off);
  float dcl = segmentx(cp.yx, 1.5*off);
  float dsl = segmentx((sp-float2(0.95*off, 0.0)).yx, off*0.5);

  float2 mp = pp;
  mod2(mp, float2(off*0.5));
    
  float2 dp = abs(mp);
  float d = dp.x;
  d = pmin(d, dp.y, sm);
  d = max(d, -dr);
  d = min(d, dcl); 
  d = min(d, dsl); 
  float2 s2 = sin(u_time+2.0*p.xz/off);
  float m = mix(0.75, 0.9, tanh_approx(s2.x+s2.y));
  m *= m;
  m *= m;
  m *= m;
  float3 hsv = float3(0.4+mix(0.5, 0.0, m), tanh_approx(0.15*mix(30.0, 10.0, m)*d), 1.0);
  float fo = exp(-0.04*max(abs(t)-off*2., 0.0));
  float3 bcol = hsv2rgb(hsv);
  gcol += 2.0*bcol*exp(-0.1*mix(30.0, 10.0, m)*d)*fo;

  float sh;
  float sdb;
  float sd = synth(opp, 4.0*aa, sh, sdb)*smoothstep(aa, -aa, -0.5*dr);
  sh = tanh_approx(sh);
  sdb *= 0.075;
  sdb *= sdb;
  sdb += 0.05;
  float3 scol = sd*(sdb)*pow(tanh3(float3(0.1)+bcol), mix(float3(1.0), float3(1.5, 0.5, 0.5), smoothstep(0.4, 0.5, sh)));
  gcol += scol;


  gcol = t > 0.0 ? gcol : float3(0.0);
  return gcol+scol;
}

float3 stars(float2 sp, float hh) {
  float3 col = float3(0.0);
  
  const float m = LAYERS;
  hh = tanh_approx(20.0*hh);

  for (float i = 0.0; i < m; ++i) {
    float2 pp = sp+0.5*i;
    float s = i/(m-1.0);
    float2 dim  = float2(mix(0.05, 0.003, s)*PI);
    float2 np = mod2(pp, dim);
    float2 h = hash2(np+127.0+i);
    float2 o = -1.0+2.0*h;
    float y = sin(sp.x);
    pp += o*dim*0.5;
    pp.y *= y;
    float l = length(pp);
  
    float h1 = fract(h.x*1667.0);
    float h2 = fract(h.x*1887.0);
    float h3 = fract(h.x*2997.0);

    float3 scol = mix(8.0*h2, 0.25*h2*h2, s)*blackbody(mix(3000.0, 20000.0, h1*h1));

    float3 ccol = col + exp(-(mix(6000.0, 2000.0, hh)/mix(2.0, 0.25, s))*max(l-0.001, 0.0))*scol;
    ccol *= mix(0.125, 1.0, smoothstep(1.0, 0.99, sin(0.33*u_time+TAU*h.y)));
    col = h3 < y ? ccol : col;
  }
  
  return col;
}

float3 meteorite(float2 sp) {
  const float period = 3.0;
  float mtime = mod(u_time, period);
  float ntime = floor(u_time/period);
  float h0 = hash(ntime+123.4);
  float h1 = fract(1667.0*h0);
  float h2 = fract(9967.0*h0);
  float2 mp = sp;
  mp.x += -1.0;
  mp.y += -0.5*h1;
  mp.y += PI*0.5;
  mp *= ROT(PI+mix(-PI/4.0, PI/4.0, h0));
  float m = mtime/period;
  mp.x += mix(-1.0, 2.0, m);
  
  float d0 = length(mp);
  float d1 = segmentx(mp);
  
  float3 col = float3(0.0);
  
  col += 0.5*exp(-4.0*max(d0, 0.0))*exp(-1000.0*max(d1, 0.0));
  col *= 2.0*HSV2RGB(float3(0.8, 0.5, 1.0));
  float fl = smoothstep(-0.5, 0.5, sin(12.0*(6.283185307*u_time)));
  col += mix(1.0, 0.5, fl)*exp(-mix(100.0, 150.0, fl)*max(d0, 0.0));
  
  col = h2 > 0.8 ? col: float3(0.0);
  return col;
}

float3 skyGrid(float2 sp) {
  const float m = 1.0;

  const float2 dim = float2(1.0/12.0*PI);
  float y = sin(sp.x);
  float2 pp = sp;
  float2 np = mod2(pp, dim*float2(1.0/floor(1.0/y), 1.0));

  float3 col = float3(0.0);

  float d = min(abs(pp.x), abs(pp.y*y));
  
  float aa = 2.0/u_resolution.y;
  
  col += 0.25*float3(0.5, 0.5, 1.0)*exp(-2000.0*max(d-0.00025, 0.0));
  
  return col;
}

float3 sunset(float2 sp, float2 nsp) {
  const float szoom   = 0.5;
  float aa = length(nsp-sp)*sqrt(0.5);
  sp -= float2(float2(0.5, -0.5)*PI);
  sp /= szoom;
  sp = sp.yx;
  sp.y += 0.22;
  sp.y = -sp.y;
  float ds = sun(sp)*szoom;
  
  float3 bscol = hsv2rgb(float3(fract(0.7-0.25*(sp.y)), 1.0, 1.0));
  float3 gscol = 0.75*sqrt(bscol)*exp(-50.0*max(ds, 0.0));
  float3 scol = mix(gscol, bscol, smoothstep(aa, -aa, ds));
  return scol;
}

float3 glow(float3 ro, float3 rd, float2 sp, float3 lp) {
  float ld = max(dot(normalize(lp-ro), rd),0.0);
  float y = -0.5+sp.x/PI;
  y = max(abs(y)-0.02, 0.0)+0.1*smoothstep(0.5, PI, abs(sp.y));
  float ci = pow(ld, 10.0)*2.0*exp(-25.0*y);
  float h = 0.65;
  float3 col = hsv2rgb(float3(h, 0.75, 0.35*exp(-15.0*y)))+HSV2RGB(float3(0.8, 0.75, 0.5))*ci;
  return col;
}

float3 neonSky(float3 ro, float3 rd, float3 nrd, out float gl) {
  const float3 lp       = 500.0*float3(0.0, 0.25, -1.0);
  float3 skyCol = HSV2RGB(float3(0.8, 0.75, 0.05));


  float glare = pow(abs(dot(rd, normalize(lp))), 20.0);
  
  float2 sp   = toSpherical(rd.xzy).yz;
  float2 nsp  = toSpherical(nrd.xzy).yz;
  float3 grd  = rd;
  grd.xy = ROT(0.025*u_time) * grd.xy;
  float2 spp = toSpherical(grd).yz;

  float gm = 1.0/abs(rd.y)*mix(0.005, 2.0, glare);
  float3 col = skyCol*gm;
  float ig = 1.0-glare;
  col += glow(ro, rd, sp, lp);
  if (rd.y > 0.0) {
    col += sunset(sp, nsp);
    col += stars(sp, 0.0)*ig;
    col += skyGrid(spp)*ig;
    col += meteorite(sp)*ig;
  }
  gl = glare;
  return col;
}

float3 render0(float3 ro, float3 rd, float3 nrd) {
  float glare;
  float3 col = neonSky(ro, rd, nrd, glare);
  if (rd.y < 0.0) {
    float t;
    col += road(ro, rd, nrd, glare, t);
  }
  return col;
}




float dfeffect(float3 p, out float ogd, float3x3 g_rot) {
  float3 p0 = p;
  float3 p1 = p;
  p1 = g_rot * p1;
  p1 = pabs(p1, 10.0);
  p1 -= 12.0;
  p1 = g_rot * p1;
  float d0 = sphere8(p0, 20.0);
  float d1 = torus(p1, 10.0*float2(1.0, 0.0125));
  float d = d0;
  d = pmax(d, -(d1-2.0), 5.0);
  d = min(d, d1);
  ogd = d1;
  return d;
}
float df(float3 p, inout float g_gd, float3x3 g_rot) {
  float d0 = dot(roadDim().xyz, p)+roadDim().w;
  p.y += -20.0*1.30;
  p.z += 66.0;
  p = g_rot * p;
  float gd1;
  float d1 = dfeffect(p, gd1, g_rot);

  float d = max(d1, -d0);
  float gd = gd1;
  g_gd = min(g_gd, gd);
  
  return d; 
}

float3 normal(float3 pos, inout float g_gd, float3x3 g_rot) {
  float2  eps = float2(NORM_OFF,0.0);
  float3 nor;
  nor.x = df(pos+eps.xyy, g_gd, g_rot) - df(pos-eps.xyy, g_gd, g_rot);
  nor.y = df(pos+eps.yxy, g_gd, g_rot) - df(pos-eps.yxy, g_gd, g_rot);
  nor.z = df(pos+eps.yyx, g_gd, g_rot) - df(pos-eps.yyx, g_gd, g_rot);
  return normalize(nor);
}

float rayMarchLo(float3 ro, float3 rd, float tinit, out int iter, inout float g_gd, float3x3 g_rot) {
  float t = tinit;
  const float tol = TOLERANCE;
  int iter_val = 0;
  for (int i = 0; i < MAX_RAY_MARCHES_LO; ++i) { iter_val = i;
    float d = df(ro + rd*t, g_gd, g_rot);
    if (d < TOLERANCE || t > MAX_RAY_LENGTH) {
      break;
    }
    t += d;
  }
  iter = iter_val;
  return t;
}

float rayMarchHi(float3 ro, float3 rd, float tinit, out int iter, inout float g_gd, float3x3 g_rot) {
  float t = tinit;
  const float tol = TOLERANCE;

  float2 dti = float2(1e10,0.0);

  int iter_val = 0;
  for (int i = 0; i < MAX_RAY_MARCHES_HI; ++i) { iter_val = i;
    float d = df(ro + rd*t, g_gd, g_rot);

    if (d<dti.x) { dti=float2(d,t); }

    if (d < TOLERANCE || t > MAX_RAY_LENGTH) {
      break;
    }
    t += d;
  }

  if(iter_val==MAX_RAY_MARCHES_HI-1) { t=dti.y; };

  iter = iter_val;
  return t;
}

// License: CC0, author: Mårten Rånge, found: https://github.com/mrange/glsl-snippets
float3x3 rotX(float a) {
  float c = cos(a);
  float s = sin(a);
  return float3x3(
    1.0 , 0.0 , 0.0
  , 0.0 , +c  , +s
  , 0.0 , -s  , +c
  );
}

// License: CC0, author: Mårten Rånge, found: https://github.com/mrange/glsl-snippets
float3x3 rotY(float a) {
  float c = cos(a);
  float s = sin(a);
  return float3x3(
    +c  , 0.0 , +s
  , 0.0 , 1.0 , 0.0
  , -s  , 0.0 , +c
  );
}

// License: CC0, author: Mårten Rånge, found: https://github.com/mrange/glsl-snippets
float3x3 rotZ(float a) {
  float c = cos(a);
  float s = sin(a);
  return float3x3(
    +c  , +s  , 0.0
  , -s  , +c  , 0.0
  , 0.0 , 0.0 , 1.0
  );
}

float3 render1(float3 col, float3 m, float3 ro, float3 rd, float3 nrd) {
  float g_gd;
  float3x3 g_rot = float3x3(1.0,0.0,0.0, 0.0,1.0,0.0, 0.0,0.0,1.0);
  float tm = u_time*0.5;
  g_rot = rotX(0.333*tm)*rotZ(0.5*tm)*rotY(0.23*tm);

  int iter;
  g_gd = 1E3;
  float t = rayMarchHi(ro, rd, 0.0, iter, g_gd, g_rot);
  float gd = g_gd;
  float3 ggcol = (glowCol1())*inversesqrt(max(gd, 0.00025));
  if (t < MAX_RAY_LENGTH) {
    float3 p = ro+rd*t;
    float3 n = normal(p, g_gd, g_rot);
    float3 r = reflect(rd, n);
    float3 nr = reflect(nrd, n);
    float fre0 = 1.0+dot(rd, n);
    float fre = fre0;
    fre *= fre;
  
    float ao = 1.0-float(iter)/float(MAX_RAY_MARCHES_HI);
    float fo = mix(0.2, 1.0, ao);
    float3 rf = m*mix(0.33, 1.0, fre)*fo*0.75;

    float3 fre1 = HSV2RGB(float3(0.8, 0.5, 1.0)); 

    g_gd = 1E3;
    int riter;
    float rt = rayMarchLo(p, r, 1.0, riter, g_gd, g_rot);
    float rgd = g_gd;
    float3 rggcol = (glowCol1())*inversesqrt(max(rgd, 0.00025));

    float3 rcol = clamp(rggcol, 0.0, 4.0);
    if (rt < MAX_RAY_LENGTH) {
      rcol += diffCol()*0.2;
    } else {
      rcol += render0(p, r, nr);
    }

    float dif = dot(sunDir1(), n); 
    col *= (1.0-m);
    col += m*sunCol1()*dif*dif*diffCol()*fo;
    col += rf*rcol*fre1;
  }
  
  col += clamp(m*ggcol, 0.0, 4.0);
  return col;
}

float3 render2(float3 ro, float3 rd, float3 nrd) {
  float3 col = render0(ro, rd, nrd);

  float t   = rayPlane(ro, rd, roadDim());
  float3 p    = ro+rd*t;
  float3 n    = roadDim().xyz;
  float3 r    = reflect(rd, n);
  float3 nr   = reflect(nrd, n);
  float fre = 1.0+dot(n, rd);
  fre *= fre;

  float3 ro0 = ro;
  float3 rd0 = rd;
  float3 nrd0= nrd;
  float3 m0 = float3(1.0); 

  if (rd.y < -0.12) {
    ro0 = p;
    rd0 = r;
    nrd0 = nr;
    float3 fre0 = HSV2RGB(float3(0.8, 0.9, 0.1)); 
    float3 fre1 = HSV2RGB(float3(0.8, 0.3, 0.9)); 
    m0 = mix(fre0, fre1, fre);
  }

  col = render1(col, m0, ro0, rd0, nrd0);
  return col;
}

float3 effect(float2 p, float2 pp) {
  float aa = 2.0/u_resolution.y;
  const float3 ro = float3(0.0, 0.0, 10.0);
  const float3 la = float3(0.0, 2.0, 0.0);
  const float3 up = float3(0.0, 1.0, 0.0);

  const float3 ww = normalize(la - ro);
  const float3 uu = normalize(cross(up, ww ));
  const float3 vv = (cross(ww,uu));
  const float fov = tan(TAU/6.0);
  float2 np = p + float2(aa); 
  float3 rd = normalize(-p.x*uu + p.y*vv + fov*ww);
  float3 nrd = normalize(-np.x*uu + np.y*vv + fov*ww);

  float3 col = render2(ro, rd, nrd);
  col -= 0.0125*float3(1.0, 2.0, 3.0)*(length(pp)+0.25);
  col *= smoothstep(1.75, 0.5, length(pp));
  col = aces_approx(col);
  col = sqrt(col);
  return col;
}

half4 main(float2 fragCoord) {
  fragCoord.y = u_resolution.y - fragCoord.y;
  float2 q = fragCoord/u_resolution.xy; 
  float2 p = -1.0 + 2.0*q;
  float2 pp = p;
  p.x *= u_resolution.x/u_resolution.y;
  float3 col = effect(p, pp);
  return half4(col.r, col.g, col.b, 1.0);
}


)";
