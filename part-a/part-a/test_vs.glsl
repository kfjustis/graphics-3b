#version 410

in layout(location = 0) vec3 vPosition;
in layout(location = 1) vec3 vNormal;

uniform mat4 ModelView;
uniform mat4 Projection;

void main()
{
	// Transform vertex  position into eye coordinates

	gl_Position = Projection * ModelView * vec4(vPosition, 1.0);

}
