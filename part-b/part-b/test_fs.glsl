#version 410

in vec4 vColor;
out vec4 fColor;

void main()
{
	//fColor = vec4(1.0, 0.0, 0.0, 1.0);
	fColor = vColor;
}
