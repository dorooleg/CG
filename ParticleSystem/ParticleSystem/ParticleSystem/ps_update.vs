#version 330                                                                        
                                                                                    
layout (location = 0) in float Type;                                                
layout (location = 1) in vec3 Position;                                             
layout (location = 2) in vec3 Velocity;                                                                                              
                                                                                    
out float Type0;                                                                    
out vec3 Position0;                                                                 
out vec3 Velocity0;                                                                                                                                     
         
uniform float gDeltaTimeMillis; 
#define PARTICLE_TYPE_SHELL 1.0f

void main()                                                                         
{
	float DeltaTimeSecs = gDeltaTimeMillis / 1000.0f;
    Type0 = Type;   

	if (Type == PARTICLE_TYPE_SHELL)
	{
		Position0 = Position + DeltaTimeSecs * 4 * Velocity;
		Velocity0 = Velocity + DeltaTimeSecs * DeltaTimeSecs * 0.1 * vec3(0.0f, -9.81f, 0.0);
	}
	else
	{
	    Position0 = Position;                                                           
		Velocity0 = Velocity;
	}                                                                   
}