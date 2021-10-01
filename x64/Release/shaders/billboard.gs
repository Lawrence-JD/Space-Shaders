// Billboard Geometry Shader
// Converts the billboards from single points to quads.
layout(points) in;
layout(triangle_strip,max_vertices=4) out;

out vec2 g_texcoord;

void main()
{
    vec4 center = gl_in[0].gl_Position;
	center = center * viewMatrix;
    vec4 p = center;
	p.x -= billboardSize.x;
	p.y += billboardSize.y;
    gl_Position = p * projMatrix;
	g_texcoord = vec2(0,1);
    EmitVertex();
	p.y = center.y - billboardSize.y;
    gl_Position = p * projMatrix;
	g_texcoord = vec2(0,0);
    EmitVertex();
	p.x = center.x + billboardSize.x;
	p.y = center.y + billboardSize.y;
    gl_Position = p * projMatrix;
	g_texcoord = vec2(1,1);
    EmitVertex();
	p.y = center.y - billboardSize.y;
    gl_Position = p * projMatrix;
    g_texcoord = vec2(1,0);
    EmitVertex();}
