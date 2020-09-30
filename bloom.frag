#version 330
in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture1;

out vec4 finalColor;

const vec2 size = vec2(640.0f, 480.0f);
const int range = 3;
const int samples = (2 * range + 1) * (2 * range + 1);
const float bloomLevel = 4.0f;
const float quality = 0.5f;

void main() {
  vec2 sizeFactor = vec2(1.0f) / size * quality;
  vec4 source = texture(texture1, fragTexCoord);
  vec4 sum = vec4(0.0f);

  for (int x = -range; x <= range; ++x)
  for (int y = -range; y <= range; ++y)
    sum += texture(texture1, fragTexCoord + vec2(x, y) * sizeFactor);

  finalColor = bloomLevel * sum / samples + source;
}
