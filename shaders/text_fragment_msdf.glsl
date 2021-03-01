#version 120

//This is a shader using some stuff from the msdf-gen README, and works alright but I couldn't figure out how to get a border

varying vec2 texCoord;
uniform sampler2D msdf;
uniform float magic; //This is calculated by character_size_onscreen_in_pixels / character_size_in_pixels * pixel_distance 
uniform vec4 color;
float width = 0.5;
float border_width = 0.5;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main() {
   //gl_FragColor = texture2D(msdf, vec2(1, -1) * texCoord.xy + vec2(0, 1));   
   vec3 msd = texture2D(msdf, vec2(1, -1) * texCoord.xy + vec2(0, 1)).rgb;
   float sd = magic * (median(msd.r, msd.g, msd.b) - (1.0 - width));
   float opacity = clamp(sd + (1.0 - width), 0.0, 1.0);
   gl_FragColor = color * vec4(1, 1, 1, opacity);
}