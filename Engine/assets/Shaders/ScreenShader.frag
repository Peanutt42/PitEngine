#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
    // Normal
    //vec3 col = texture(screenTexture, TexCoords).rgb;
    //FragColor = vec4(col, 1.0);

    // Inversion
    //FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);

    // Greyscale
    FragColor = texture(screenTexture, TexCoords);
    float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    FragColor = vec4(average, average, average, 1.0);
} 