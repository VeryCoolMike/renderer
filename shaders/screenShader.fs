#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D screenTexture;
uniform int shader;

const float offset = 1.0f / 300.0f;

vec3 calculateKernel(float kernel[9])
{
   vec2 offsets[9] = vec2[]
   (
      vec2(-offset, offset),
      vec2(0.0f, offset),
      vec2(offset, offset),
      vec2(-offset, 0.0f),
      vec2(0.0f, 0.0f),
      vec2(offset, 0.0f),
      vec2(-offset, -offset),
      vec2(0.0f, -offset),
      vec2(offset, -offset)
   );

   vec3 sampleTex[9];
   for (int i = 0; i < 9; i++)
   {
      sampleTex[i] = vec3(texture(screenTexture, TexCoord.st + offsets[i]));
   }
   vec3 col = vec3(0.0f);
   for (int i = 0; i < 9; i++)
   {
      col += sampleTex[i] * kernel[i];
   }
   return col;
};

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

      case 3:
         float kernel[9] = float[]
         (
            -1, -1, -1,
            -1,  9, -1,
            -1, -1, -1
         );

         FragColor = vec4(calculateKernel(kernel), 1.0f);
         break;

      default:
         FragColor = vec4(1.0, 0.0, 1.0, 1.0);
   }
};

