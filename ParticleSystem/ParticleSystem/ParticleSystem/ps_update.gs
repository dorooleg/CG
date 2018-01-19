#version 330                                                                        
                                                                                    
layout(points) in;                                                                  
layout(points) out;                                                                 
layout(max_vertices = 30) out;                                                      
                                                                                    
in float Type0[];                                                                   
in vec3 Position0[];                                                                
in vec3 Velocity0[];                                                              
                                                                                    
out float Type1;                                                                    
out vec3 Position1;                                                                 
out vec3 Velocity1;                                                                                                                                    
                                                                                                                                        
uniform float gTime;                                                                
uniform sampler1D gRandomTexture;                                                                                                
                                                                                    
#define PARTICLE_TYPE_LAUNCHER 0.0f                                                 
#define PARTICLE_TYPE_SHELL 1.0f                                                                                         
                                                                                    
vec3 GetRandomDir(float TexCoord)                                                   
{                                                                                   
     vec3 Dir = texture(gRandomTexture, TexCoord).xyz;                              
     Dir -= vec3(0.5, 0.5, 0.5);                                                    
     return Dir;                                                                    
}
                                                                                    
void main()                                                                         
{                         
	Type1 = Type0[0];                                             
	Position1 = Position0[0];
	Velocity1 = Velocity0[0];                                                                                                            
	EmitVertex();                                                               
	EndPrimitive();

	if (Type0[0] == PARTICLE_TYPE_LAUNCHER)
	{
	    Type1 = PARTICLE_TYPE_SHELL;                                            
        Position1 = Position0[0];                                               
        vec3 Dir = GetRandomDir(gTime/1000.0);                                  
        Dir.y = max(Dir.y, 0.5);                                                
        Velocity1 = normalize(Dir) / 20.0;    
		EmitVertex();                                                               
		EndPrimitive();
	}
}                                                                                   
