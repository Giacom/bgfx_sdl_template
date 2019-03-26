$input a_position, a_color0, a_texcoord0
$output v_color0, v_texcoord0

#include "bgfx/examples/common/common.sh"

void main()
{
	gl_Position = vec4(a_position, 1);
	v_color0 = a_color0;
	v_texcoord0 = a_texcoord0;
}