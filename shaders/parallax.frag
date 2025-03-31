#version 330 core
out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D backgroundTexture;
uniform sampler2D lastFrameTexture;
uniform vec2 uMouse;
uniform float blurStrength;

void main() {
    vec2 parallaxUV = TexCoords + (uMouse * 0.02);
    vec4 currentColor = texture(backgroundTexture, parallaxUV);
    vec4 lastColor = texture(lastFrameTexture, TexCoords);
    FragColor = mix(lastColor, currentColor, 1.0 - blurStrength);
}
