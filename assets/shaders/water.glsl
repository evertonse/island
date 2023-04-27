#define	AA 2
#define HASHSCALE 0.1031

float hash(vec3 p3)
{
	p3 = fract(p3 * HASHSCALE); 
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.x + p3.y) * p3.z);
}

vec3 fade(vec3 t) { return t*t*t*(t*(6.*t-15.)+10.); }

float grad(float hash, vec3 p) 
{
    int h = int(1e4*hash) & 15;
	float u = h<8 ? p.x : p.y,
 		  v = h<4 ? p.y : h==12||h==14 ? p.x : p.z;
    return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}

//3d perlin noise, based on https://mrl.nyu.edu/~perlin/paper445.pdf and https://mrl.nyu.edu/~perlin/noise/.
float perlinNoise3D(vec3 p)
{
	vec3 pi = floor(p), pf = p - pi, w = fade(pf);
    return mix( mix( mix( grad(hash(pi + vec3(0, 0, 0)), pf - vec3(0, 0, 0)),
                           grad(hash(pi + vec3(1, 0, 0)), pf - vec3(1, 0, 0)), w.x ),
            	      mix( grad(hash(pi + vec3(0, 1, 0)), pf - vec3(0, 1, 0)), 
                	 	   grad(hash(pi + vec3(1, 1, 0)), pf - vec3(1, 1, 0)), w.x ), w.y ),
        		 mix( mix( grad(hash(pi + vec3(0, 0, 1)), pf - vec3(0, 0, 1)), 
                		   grad(hash(pi + vec3(1, 0, 1)), pf - vec3(1, 0, 1)), w.x ),
            		  mix( grad(hash(pi + vec3(0, 1, 1)), pf - vec3(0, 1, 1)), 
                		   grad(hash(pi + vec3(1, 1, 1)), pf - vec3(1, 1, 1)), w.x ), w.y ), w.z );
}

float fbm(vec3 pos, int octaves, float persistence) 
{
    float total = 0.0, frequency = 1.0, amplitude = 1.0, maxValue = 0.0;
    for(int i = 0; i < octaves; ++i) 
    {
        total += perlinNoise3D(pos * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2.0;
    }
    return total / maxValue;
}

float getNoise(vec3 p)
{
    return 0.15 * fbm(p + 0.3*iTime, 4, 0.3);
}

//build a right handed look at view matrix.
mat3 lookAt( in vec3 eye, in vec3 center, in vec3 up )
{
	vec3 f = normalize(center - eye);
	vec3 s = normalize(cross(f, up));
	vec3 u = cross(s, f);
    return mat3(s, u, -f);
}

//distance functions
float sdSphere(vec3 pos, float radius) { return length(pos) - radius; }

float sdBox(vec3 pos, vec3 size) 
{
    vec3 d = abs(pos) - size;
    return min(max(d.x, max(d.y, d.z)), 0.0) + length(max(d, 0.0));
}

//distance operations
vec2 opU(vec2 a, vec2 b) { return a.x < b.x ? a : b; } //union

vec2 mapScene(vec3 pos) 
{ 
    float shapeIndex = 0.0;
    vec2 res = vec2(sdSphere(pos - vec3(-3, 0, 0), 2.0), shapeIndex++);
    res = opU(res, vec2(sdBox(pos - vec3(3, 0, 0), vec3(1.5)), shapeIndex++));
    res.x += getNoise(pos);
    return res;
}

float sdScene(vec3 pos) { return mapScene(pos).x; }

vec3 calculateNormal(in vec3 p) 
{
    const vec3 t = vec3(0.01, 0.0, 0.0);
    float gradX = sdScene(p + t.xyy) - sdScene(p - t.xyy);
    float gradY = sdScene(p + t.yxy) - sdScene(p - t.yxy);
    float gradZ = sdScene(p + t.yyx) - sdScene(p - t.yyx);
    
    vec3 normal = vec3(gradX, gradY, gradZ);
    return normalize(normal);
}

vec2 rayMarch(vec3 rayOri, vec3 rayDir)
{
    const float MAX_TRACE_DISTANCE = 20.0;
    const int MAX_STEPS = 64;
    
    float totalDistance = 0.0, shapeIndex = -1.0;
    for (int i = 0; i < MAX_STEPS; ++i) {
        vec2 res = mapScene(rayOri + totalDistance * rayDir);
        float minHitDistance = 0.0005 * totalDistance;
        if (res.x < minHitDistance) {
            shapeIndex = res.y; break; 
        }
        if (totalDistance > MAX_TRACE_DISTANCE) break;
        totalDistance += res.x;
    }
    
	return vec2(totalDistance, shapeIndex);
}

vec3 render(vec3 rayOri, vec3 rayDir)
{
	vec3 color = pow(texture(iChannel1, rayDir).rgb, vec3(2.2));
    vec2 res = rayMarch(rayOri, rayDir);
    int shapeIndex = int(res.y);
    if (shapeIndex >= 0) {
        vec3 pos = rayOri + rayDir * res.x;
        vec3 normal = calculateNormal(pos);
		vec3 refractDir = refract(rayDir, normal, 0.8);
        
		color = pow(texture(iChannel1, refractDir).rgb, vec3(2.2)) * 0.9;
        if (shapeIndex == 0) color*= vec3(0.3, 0.7, 1.0);
    }
    
    return color;
}

void mainImage(out vec4 fragColor, in vec2 fragCoord) 
{
    vec3 totalColor = vec3(0.0);
    
    vec2 angle = (iMouse.z > 0.5) ? 4.0*(2.0*iMouse.xy/iResolution.xy-1.0) : vec2(0.0);
    vec3 rayOri = vec3(12.0*sin(angle.x), angle.y+5.0, 12.0*cos(angle.x));
    vec3 rayTgt = vec3(0.0);
    
    for (int i = 0; i < AA; ++i)
    for (int k = 0; k < AA; ++k)
    {
        vec2 offset = vec2(float(i) + .5, float(k) + .5) / float(AA) - .5;
        vec2 uv = (fragCoord + offset - iResolution.xy * 0.5) / iResolution.x;
        
    	mat3 viewMat = lookAt(rayOri, rayTgt, vec3(0.0, 1.0, 0.0));
    	vec3 rayDir = normalize(viewMat * vec3(uv, -1.0));

   		vec3 color = render(rayOri, rayDir);
        totalColor += color;
    }         
    
	totalColor /= float(AA * AA);
    totalColor = pow(totalColor, vec3(0.45));
	fragColor = vec4(totalColor, 1.0);
}