layout(location=0) in vec3 position_in;

out gl_PerVertex {
  vec4 gl_Position;
};

layout(location=0) out vec4 out_color;

void main() {
  gl_Position = vec4(position_in, 1.0);
  out_color = vec4(COLOR1, 1.0);
}