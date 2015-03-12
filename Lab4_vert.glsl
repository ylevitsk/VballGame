struct Material {
  vec3 aColor;
  vec3 dColor;
  vec3 sColor;
  float shine;
};

attribute vec3 aPosition;
attribute vec3 aNormal;

uniform mat4 uProjMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uModelMatrix;
uniform vec3 uLightPos;
uniform vec3 uLColor;
uniform Material uMat;
varying vec3 vColor;
varying vec3 vNormal;
varying vec3 vPos;
varying vec3 vertPos;
varying vec3 normalInterp;

void main() {
  vec4 vPosition;
  vec4 light;
  vec3 Refl;
  vec3 Spec, Diffuse;
  
  
  vNormal = vec3(aNormal.x, aNormal.y, aNormal.z);
  vPos = vec3(aPosition.x, aPosition.y, aPosition.z);
  /* First model transforms */
  vPosition = uModelMatrix* vec4(aPosition.x, aPosition.y, aPosition.z, 1);

  
  light = vec4(uLightPos, 1) - vPosition;
  vPosition = uViewMatrix * vPosition;
  gl_Position = uProjMatrix * vPosition;

  vec4 vertPos4 = uModelMatrix * vec4(aPosition, 1.0);
  vertPos = vec3(vertPos4)/ vertPos4.w;
  normalInterp = vec3(uModelMatrix * vec4(aNormal, 0.0));

  vec3 normal = normalize(normalInterp);
  vec3 lightDir = normalize(uLightPos - vertPos);
  vec3 reflectDir = reflect(lightDir, normal);
  vec3 viewDir = normalize(-vertPos);

  float lambertian = max(dot(lightDir, normal), 0.0);
  float specular = 0.0;
  if(lambertian > 0.0) {
     float specAngle = max(dot(reflectDir, viewDir), 0.0);
     specular = pow(specAngle, uMat.shine);
  }
  light = normalize(light);
  vColor = uLColor * lambertian * uMat.dColor + uLColor * specular*uMat.sColor + uMat.aColor * uLColor;
    
}


