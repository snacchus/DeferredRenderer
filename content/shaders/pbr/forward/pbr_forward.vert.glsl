#version 330
#pragma type vertex

#include "common/uniforms.glh"
#include "common/lighting.glh"
#include "common/input.glh"
#include "pbr_forward_common.glh"

out vertex_output v_output;

void main()
{
	vec4 pos = vec4(v_input.position, 1.0);
	vec4 normal = vec4(v_input.normal, 0.0);
	vec4 tangent = vec4(v_input.tangent, 0.0);

	// world space position
	vec4 wp = cm_mat_world * pos;

	// world space light vector
	v_output.lightVec = light_vec(wp.xyz);

	// world space view vector
	v_output.viewVec = cm_cam_pos - wp.xyz;

	// world space normal
	vec4 wn = cm_mat_tiworld * normal;
	v_output.normal = normalize(wn.xyz);

	// world space tangent
	vec4 wt = cm_mat_tiworld * tangent;
	v_output.tangent = normalize(wt.xyz);

	// world space bitangent from cross product
	v_output.bitangent = cross(v_output.tangent, v_output.normal);

	// texture coordinate
	v_output.uv = v_input.uv;

	// clip space position
	gl_Position = cm_mat_wvp * pos;
}