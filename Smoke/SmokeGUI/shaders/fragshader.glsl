#version 410
// Fragment shader

layout (location = 0) in vec3 vertcoords_in;
layout (location = 1) in vec3 vertcolour_in;
layout (location = 2) in vec3 vertnormal_in;

uniform int phong;

out vec4 fColor;

void main() {

    if (phong == 1)
    {
        vec3 lightpos = vec3(0.0, 0.0, -1.0);
        vec3 lightcolour = vec3(1.0);
        //vec3 lightpostwo = vec3(-3.0, 0.0, -1.0);


        vec3 matcolour = vertcolour_in;
        vec3 matspeccolour = vec3(1.0);

        float matambientcoeff = 0.2;
        float matdiffusecoeff = 0.7;
        float matspecularcoeff = 0.0;

        vec3 normal;
        normal = normalize(vertnormal_in);

        vec3 surftolight = normalize(lightpos - vertcoords_in);
        //vec3 surftolighttwo = normalize(lightpostwo - vertcoords_in);
        float diffusecoeff = max(0.0, dot(surftolight, normal));
        //float diffusecoeff = max(max(0.0, dot(surftolight, normal)),dot(surftolighttwo, normal));

        vec3 camerapos = vec3(0.0);
        vec3 surftocamera = normalize(camerapos - vertcoords_in);
        vec3 reflected = 2 * diffusecoeff * normal - surftolight;
        float specularcoeff = max(0.0, dot(reflected, surftocamera));

        vec3 compcolour = min(1.0, matambientcoeff + matdiffusecoeff * diffusecoeff) * lightcolour * matcolour;
             //compcolour = vertnormal_in;
             //compcolour += matspecularcoeff * specularcoeff * lightcolour * matspeccolour;


      fColor = vec4(compcolour, 1.0);
    }
    else
    {
        fColor = vec4(vertcolour_in, 1.0);
    }

}
