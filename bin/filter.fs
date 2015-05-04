uniform sampler2D Texture0;															
uniform sampler2D Texture1;															
uniform vec2 TexSize;																	
uniform int enableMatte;																
float kernal[25];																		
float hardlight(float a, float b)														
{																						
	if (b <= 0.5)																		
	{																					
		return a * b / 0.5;																
	}																					
	else																				
	{																					
		return 1.0 - (1.0 - a) * (1.0 - b) / 0.5;										
	}																					
}																						
float softlight(float a, float b)														
{																						
	if (b <= 0.5)																		
	{																					
		return a * b / 0.5 + (a / 1.0) * (a / 1.0) * (1.0 - 2 * b);						
	}																					
	else																				
	{																					
		return a * (1.0 - b) / 0.5 + sqrt(a / 1.0) * (2.0 * b - 1.0);					
	}																					
}																						
vec4 softlight(vec4 a, vec4 b)															
{																						
	//return vec4(softlight(a.x, b.x), softlight(a.y, b.y), softlight(a.z, b.z), 1.0);	
	return 2.0 * a * b + a * a - 2.0 * a * a * b;										
}																						
vec4 gauss_filter(float kernal[25], int radius, sampler2D image, vec2 uv, vec2 texSize)
{																						
	vec4 c = vec4(0.0, 0.0, 0.0, 0.0);													
   float dx = 1.0 / texSize.x;															
   float dy = 1.0 / texSize.y;															
	for (int n=0, y=-radius; y<=radius; y++)											
	{																					
		for (int x=-radius; x<=radius; x++, n++)										
		{																				
			vec2 _uv = vec2(uv.x + x * dx, uv.y + y * dy);								
			c += texture2D(image, _uv) * kernal[n];										
		}																				
	}																					
	return c;																			
}																						
vec4 alpha_blend(vec4 a, vec4 b)														
{																						
	return a * vec4(1.0 - b.a) + b * vec4(b.a);											
}																						
void main(void)																		
{																						
	int radius = 2;																	
	kernal[0] = float(0.000000); kernal[1] = float(0.000028); kernal[2] = float(0.000208); kernal[3] = float(0.000028); kernal[4] = float(0.000000); 
	kernal[5] = float(0.000028); kernal[6] = float(0.011332); kernal[7] = float(0.083731); kernal[8] = float(0.011332); kernal[9] = float(0.000028); 
	kernal[10] = float(0.000208); kernal[11] = float(0.083731); kernal[12] = float(0.618694); kernal[13] = float(0.083731); kernal[14] = float(0.000208); 
	kernal[15] = float(0.000028); kernal[16] = float(0.011332); kernal[17] = float(0.083731); kernal[18] = float(0.011332); kernal[19] = float(0.000028); 
	kernal[20] = float(0.000000); kernal[21] = float(0.000028); kernal[22] = float(0.000208); kernal[23] = float(0.000028); kernal[24] = float(0.000000); 

	vec2 uv = gl_TexCoord[0].st;														
	vec4 a = texture2D(Texture0, uv);													
	vec4 b = gauss_filter(kernal, radius, Texture0, uv, TexSize);						
   vec4 c = softlight(b, a);															
                                                                                       
	if (enableMatte == 0)																
	{																					
		gl_FragColor = c;																
   }																					
   else																				
	{																					
       vec4 d = texture2D(Texture1, uv);												
		gl_FragColor = alpha_blend(c, d);												
   }																					
}																						
 