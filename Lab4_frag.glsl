struct Material {
  vec3 aColor;
  vec3 dColor;
  vec3 sColor;
  float shine;
};
varying vec3 vNormal;
varying vec3 vColor;
varying vec3 vertPos;

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform vec3 uLightPos;
uniform vec3 uLColor;
uniform Material uMat;
varying vec3 normalInterp;
varying vec3 vPos;

void main() {

     /*vec3 normal = normalize(normalInterp);
     vec3 lightDir = normalize(uLightPos - vertPos);
     vec3 reflectDir = reflect(lightDir, normal);
     vec3 viewDir = normalize(-vertPos);

     float lambertian = max(dot(lightDir, normal), 0.0);
     float specular = 0.0;

     if(lambertian > 0.0) {
        float specAngle = max(dot(reflectDir, viewDir), 0.0);
        specular = pow(specAngle, uMat.shine);
     }
     gl_FragColor = vec4(uLColor * lambertian* uMat.dColor +
      uLColor *specular*uMat.sColor + uMat.aColor * uLColor, 1.0);*/

     vec4 vPosition;
     vec4 light;
  

     /* First model transforms */
     vPosition = uModelMatrix* vec4(vPos.x, vPos.y, vPos.z, 1);
     light = vec4(uLightPos, 1) - vPosition;
     vPosition = uViewMatrix* vPosition;
     //gl_Position = uProjMatrix*vPosition;

     light = normalize(light);
     gl_FragColor = vec4(uLColor * dot(light, vec4(vNormal, 1)) *
	  uMat.dColor + uMat.aColor* uLColor, 1.0);
}

