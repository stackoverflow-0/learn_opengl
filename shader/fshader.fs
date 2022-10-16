#version 330 core
out vec4 FragColor;

in vec4 pos;

uniform int sphere_index;
void main()
{
    switch (sphere_index){
        case 0 :
        FragColor = vec4( 1.0, 0.0 ,0.0,1.0);
        break;
        case 1 :
        FragColor = vec4( 0.0, 1.0 ,0.0,1.0);
        break;
        case 2 :
        FragColor = vec4( 0.0, 0.0 ,1.0,1.0);
        break;
        case 3 :
        FragColor = vec4( 0.0, 1.0 ,1.0,1.0);
        break;
        default:
        FragColor = vec4( 1.0, 1.0 ,1.0,1.0);
    }

}
    