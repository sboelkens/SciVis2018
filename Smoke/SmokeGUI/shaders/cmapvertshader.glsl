#version 410
// Vertex shader

layout (location = 0) in vec2 vertcoords_world_vs;
layout (location = 1) in vec3 vertcolour_in;
layout (location = 2) in float vertattrib_in;

uniform mat4 modelviewmatrix;
uniform mat4 projectionmatrix;

layout (location = 1) out vec3 vertcolour_out;
layout (location = 2) out float vertattrib_out;

void main() {

  gl_Position = projectionmatrix * modelviewmatrix * vec4(vertcoords_world_vs.x, vertcoords_world_vs.y, -1.0, 1.0);
  vertcolour_out = vertcolour_in;
  vertattrib_out = vertattrib_in;
}
