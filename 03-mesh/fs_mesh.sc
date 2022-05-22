$input v_normal, v_view

#include <bgfx_shader.sh>

vec2 blinn(vec3 _lightDir, vec3 _normal, vec3 _viewDir)
{
	float ndotl = dot(_normal, _lightDir);
	vec3 reflected = _lightDir - 2.0*ndotl*_normal; // reflect(_lightDir, _normal);
	float rdotv = dot(reflected, _viewDir);
	return vec2(ndotl, rdotv);
}

vec4 lit(float ndotl, float rdotv)
{
    float diff = max(0, ndotl);
    float spec = step(0.0, ndotl) * max(0.0, rdotv);
    return vec4(1.0, diff, spec, 1.0);
}

float fresnel(float ndotl)
{
    float facing = (1.0 - ndotl);
    return max(pow(facing, 5.0), 0.0);
}

void main()
{
    vec3 color = vec3(1.0, 1.0, 1.0);
    vec3 lightDir = vec3(0, 0, -1);
    vec3 normal = normalize(v_normal);
    vec3 view = normalize(v_view);

    vec2 blin = blinn(lightDir, normal, view);
    vec4 lc = lit(blin.x, blin.y);
    float fres = fresnel(blin.x);

    // color with gamma
    gl_FragColor.xyz = pow( vec3(0.07, 0.06, 0.08) + color * lc.y + fres * pow(lc.z, 128), vec3_splat(1.0/2.0) );
    // gl_FragColor.xyz = color * lc.y;
    gl_FragColor.w = 1.0;
}