#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D screenTexture;
uniform int shader;
void main()
{
   switch(shader)
   {
      case 0:
         FragColor = texture(screenTexture, TexCoord);
         break;
      
      case 1:
         FragColor = vec4(vec3(1.0f - texture(screenTexture, TexCoord)), 1.0f);
         break;

      case 2:
         FragColor = texture(screenTexture, TexCoord);
         float average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
         FragColor = vec4(average, average, average, 1.0);
         break;

      default:
         FragColor = vec4(1.0, 0.0, 1.0, 1.0);
   }
};