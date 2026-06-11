#version 410 core

layout (vertices = 16) out;

in vec3 vertexPos[];
out vec3 tcsPos[];

uniform float tessLevelOuterLeft;
uniform float tessLevelOuterRight;
uniform float tessLevelOuterBottom;
uniform float tessLevelOuterTop;
uniform float tessLevelInner;

void main()
{
	tcsPos[gl_InvocationID] = vertexPos[gl_InvocationID];

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

	if(gl_InvocationID == 0)
    {
        gl_TessLevelOuter[0] = tessLevelOuterLeft;
        gl_TessLevelOuter[1] = tessLevelOuterBottom;
        gl_TessLevelOuter[2] = tessLevelOuterRight;
        gl_TessLevelOuter[3] = tessLevelOuterTop;

        gl_TessLevelInner[0] = tessLevelInner;
        gl_TessLevelInner[1] = tessLevelInner;
    }
}