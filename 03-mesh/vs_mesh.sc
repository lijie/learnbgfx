$input a_position, a_normal
$output v_pos, v_normal, v_view

#include <bgfx_shader.sh>

void main()
{
    gl_Position = mul(u_modelViewProj, vec4(a_position, 1.0));

    // 这个模型法线在模型空间, 范围在 [0, 1], 变换到 [-1, 1]
    vec3 normal = a_normal.xyz * 2.0 - 1.0;

    // position at view space
    v_view = mul(u_modelView, vec4(a_position, 1)).xyz;
    // normal at view space
    v_normal = mul(u_modelView, vec4(normal, 0)).xyz;
}
