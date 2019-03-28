#version 410
// Vertex shader

layout (location = 0) in vec3 vertcoords_world_vs;
layout (location = 1) in float vertattrib_in;
layout (location = 2) in vec3 vertnormals_world_vs;

uniform mat4 modelviewmatrix;
uniform mat4 projectionmatrix;
uniform mat3 normalmatrix;

layout (location = 0) out vec3 vertcoords_out;
layout (location = 1) out float vertattrib_out;
layout (location = 2) out vec3 vertnormal_out;

void main() {
  gl_Position = projectionmatrix * modelviewmatrix * vec4(vertcoords_world_vs.x, vertcoords_world_vs.y, vertcoords_world_vs.z, 1.0);
  vertcoords_out = vec3(modelviewmatrix * vec4(vertcoords_world_vs.x, vertcoords_world_vs.y, vertcoords_world_vs.z, 1.0));
  vertattrib_out = vertattrib_in;
  vertnormal_out = normalmatrix * vertnormals_world_vs;
}
