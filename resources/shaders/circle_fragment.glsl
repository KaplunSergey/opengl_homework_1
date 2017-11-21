#version 330 core

uniform vec2 windowSize;
uniform float radius;
uniform vec4 color;
uniform vec2 center;
out vec4 outColor;

void main(void)
{

      float x = (gl_FragCoord.x - windowSize.x / 2.0f) / (windowSize.x / 2.0f);
      float y = (gl_FragCoord.y - windowSize.y / 2.0f) / (windowSize.y / 2.0f);
      float len = sqrt(pow(center.x - x, 2) + pow(center.y - y, 2));

      if (len > radius) {
         discard;
      } else {
          outColor = color;
      }

}
