#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in vec3 aColor; // vertex color desteği

out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoords;
out mat3 TBN;
out vec3 VertexColor; // fragment shadera renk bilgisi

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix; 

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = normalMatrix * aNormal; // normal hesabı yapma
    TexCoords = aTexCoords;
    VertexColor = aColor;

    // TBN matrisi hesaplama 
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 B = normalize(normalMatrix * aBitangent);
    vec3 N = normalize(Normal);
    
    // yeniden ortogonalizasyon
    //vektörler kümelerinmin dik hale gelmesi işlemi
    T = normalize(T - dot(T, N) * N);
    B = normalize(cross(N, T)); 
    
    TBN = mat3(T, B, N);

    gl_Position = projection * view * vec4(FragPos, 1.0);
}