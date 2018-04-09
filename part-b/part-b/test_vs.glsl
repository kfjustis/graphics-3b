#version 410

in layout(location = 0) vec3 vPosition;
in layout(location = 1) vec3 vNormal;

uniform mat4 ModelView;
uniform mat4 Projection;

uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform vec4 LightPosition;
uniform float Shininess;

out vec4 vColor;

void main()
{
	// Transform vertex  position into eye coordinates

	gl_Position = Projection * ModelView * vec4(vPosition, 1.0);

	// ambient reflection
	vec4 ambient = AmbientProduct;

	// diffuse reflection
	vec3 pos = (ModelView * vec4(vPosition, 1.0)).xyz; // Transform vtx pos into eye coords
	vec3 L = normalize((ModelView * LightPosition).xyz-pos); // unit vec in dir of light src
	vec3 N = normalize(ModelView * vec4(vNormal, 0.0)).xyz; // Tform vtx norm into eye coords
	float Kd = max(dot(L,N), 0.0);
	vec4 diffuse = Kd * DiffuseProduct;

	// specular reflection
	vec3 E = normalize(-pos); // normalized vector in the dir of the viewer
	vec3 H = normalize(L + E); // halfway vector
	float Ks = pow(max(dot(N,H),0.0), Shininess);
	vec4 specular = Ks * SpecularProduct;
	if (dot(L,N) < 0.0) {
		specular = vec4(0.0, 0.0, 0.0, 1.0);
	}

	vColor = ambient + diffuse + specular;

	vColor.a = 1.0;
}
