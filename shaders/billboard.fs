// Billboard Fragment Shader
out vec4 color;
layout(binding=0) uniform sampler2DArray tex;
in vec2 g_texcoord;
void main()
{
	vec4 c1 = texture( tex, vec3(g_texcoord,floor(texSlice) ) );
	vec4 c2 = texture( tex, vec3(g_texcoord,ceil(texSlice) ) );
	color = mix(c1,c2,fract(texSlice));
	
	if( color.a < 0.01 )
		discard;
}
