#type vertex

#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main() {
    TexCoords = aTexCoords;
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}


#type fragment

#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

void main() {
    // Normal
    //vec3 col = texture(screenTexture, TexCoords).rgb;
    FragColor = vec4(texture(screenTexture, TexCoords).rgb, 1.0);

    // Inversion
    //FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);

    // Greyscale
    //FragColor = texture(screenTexture, TexCoords);
    //float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    //FragColor = vec4(average, average, average, 1.0);
}