layout(location=0) in vec4 position_in;

out gl_PerVertex {
  vec4 gl_Position;
};

layout(location=0) out vec4 out_color;

void main() {
  gl_Position = position_in;
  out_color = vec4(COLOR1, 1.0);
}