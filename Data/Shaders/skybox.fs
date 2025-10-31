#version 330
in vec3 fragTexCoord;
out vec4 finalColor;
uniform samplerCube environmentMap;
void main() {
    vec3 texCoord = normalize(fragTexCoord);
    finalColor = texture(environmentMap, texCoord);
}
