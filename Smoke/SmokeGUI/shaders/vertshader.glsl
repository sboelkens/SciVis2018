#version 410
// Vertex shader

layout (location = 0) in vec3 vertcoords_world_vs;
layout (location = 1) in vec3 vertcolour_in;
layout (location = 2) in vec3 vertnormal_in;

uniform mat4 modelviewmatrix;
uniform mat4 projectionmatrix;
uniform mat3 normalmatrix;

layout (location = 0) out vec3 vertcoords_out;
layout (location = 1) out vec3 vertcolour_out;
layout (location = 2) out vec3 vertnormal_out;

void main() {

  gl_Position = projectionmatrix * modelviewmatrix * vec4(vertcoords_world_vs.x, vertcoords_world_vs.y, vertcoords_world_vs.z, 1.0);
  vertcoords_out = vec3(modelviewmatrix * vec4(vertcoords_world_vs.x, vertcoords_world_vs.y, vertcoords_world_vs.z, 1.0));
  vertcolour_out = vertcolour_in;
  vertnormal_out = normalmatrix * vertnormal_in;
}
