#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D screenTexture;
uniform sampler2D screenTexture2;
uniform vec3 bgColor;
uniform int shader;

const float offset = 1.0f / 300.0f;

vec3 calculateKernel(float kernel[9], sampler2D currentTexture)
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
      sampleTex[i] = vec3(texture(currentTexture, TexCoord.st + offsets[i]));
   }
   vec3 col = vec3(0.0f);
   for (int i = 0; i < 9; i++)
   {
      col += sampleTex[i] * kernel[i];
   }
   return col;
};

void calculateFinalKernel(float currentKernel[9])
{
   vec4 tex1 = texture(screenTexture, TexCoord);
   vec4 tex2 = texture(screenTexture2, TexCoord);
   
   float threshold = 0.01;
   
   if(distance(tex2.rgb, bgColor) < threshold)
      FragColor = vec4(calculateKernel(currentKernel, screenTexture), 1.0f);
   else
      FragColor = vec4(calculateKernel(currentKernel, screenTexture2), 1.0f);
}

void main()
{
   vec4 tex1 = texture(screenTexture, TexCoord);
   vec4 tex2 = texture(screenTexture2, TexCoord);
   
   float threshold = 0.01;
   switch(shader)
   {
      case 0:

         if(distance(tex2.rgb, bgColor) < threshold)
         {
            FragColor = tex1;
         }
         else
         {
            FragColor = tex2;
         }
         
         break;
      
      case 1:
         
         
         if(distance(tex2.rgb, bgColor) < threshold)
         {
            FragColor = vec4(vec3(1.0f - tex1), 1.0f);
         }
         else
         {
            FragColor = vec4(vec3(1.0f - tex2), 1.0f);
         }
         break;

      case 2:
         
         
         if(distance(tex2.rgb, bgColor) < threshold)
         {
            float average = 0.2126 * tex1.r + 0.7152 * tex1.g + 0.0722 * tex1.b;
            FragColor = vec4(average, average, average, 1.0);
         }
         else
         {
            float average = 0.2126 * tex2.r + 0.7152 * tex2.g + 0.0722 * tex2.b;
            FragColor = vec4(average, average, average, 1.0);
         }
            
         break;

      case 3:
         float kernel[9] = float[]
         (
            -1, -1, -1,
            -1,  9, -1,
            -1, -1, -1
         );

         calculateFinalKernel(kernel);

      
         //FragColor = mix(vec4(calculateKernel(kernel, screenTexture2), 1.0f), vec4(calculateKernel(kernel, screenTexture), 1.0f), 0.5);
         break;

      default:
         FragColor = vec4(1.0, 0.0, 1.0, 1.0);
   }
};

