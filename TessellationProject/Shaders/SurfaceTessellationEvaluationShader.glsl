#version 410 core

layout (quads, equal_spacing, ccw) in;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

in vec3 tcsPos[];

out vec2 texCoords;
out vec3 normal;
out vec3 fragPos;

vec3 Bezier(float t, vec3 p0, vec3 p1, vec3 p2, vec3 p3)
{
	vec3 v01 = mix(p0, p1, t);
	vec3 v11 = mix(p1, p2, t);
	vec3 v21 = mix(p2, p3, t);

	vec3 v02 = mix(v01, v11, t);
	vec3 v12 = mix(v11, v21, t);

	return mix(v02, v12, t);
}

vec3 BezierDerivative(float t, vec3 p0, vec3 p1, vec3 p2, vec3 p3)
{
    vec3 d0 = 3.0 * (p1 - p0);
    vec3 d1 = 3.0 * (p2 - p1);
    vec3 d2 = 3.0 * (p3 - p2);

    vec3 q0 = mix(d0, d1, t);
    vec3 q1 = mix(d1, d2, t);

    return mix(q0, q1, t);
}

void main()
{
    texCoords = gl_TessCoord.xy;

	float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec3 curve0 = Bezier(u,
        tcsPos[0],
        tcsPos[1],
        tcsPos[2],
        tcsPos[3]);

    vec3 curve1 = Bezier(u,
        tcsPos[4],
        tcsPos[5],
        tcsPos[6],
        tcsPos[7]);

    vec3 curve2 = Bezier(u,
        tcsPos[8],
        tcsPos[9],
        tcsPos[10],
        tcsPos[11]);

    vec3 curve3 = Bezier(u,
        tcsPos[12],
        tcsPos[13],
        tcsPos[14],
        tcsPos[15]);

    vec3 position = Bezier(v,
        curve0,
        curve1,
        curve2,
        curve3);

    gl_Position =
        projection *
        view *
        model *
        vec4(position, 1.0);

            // dP/du
    vec3 dCurve0 = BezierDerivative(u,
        tcsPos[0],
        tcsPos[1],
        tcsPos[2],
        tcsPos[3]);

    vec3 dCurve1 = BezierDerivative(u,
        tcsPos[4],
        tcsPos[5],
        tcsPos[6],
        tcsPos[7]);

    vec3 dCurve2 = BezierDerivative(u,
        tcsPos[8],
        tcsPos[9],
        tcsPos[10],
        tcsPos[11]);

    vec3 dCurve3 = BezierDerivative(u,
        tcsPos[12],
        tcsPos[13],
        tcsPos[14],
        tcsPos[15]);

    vec3 dPdu = Bezier(v,
        dCurve0,
        dCurve1,
        dCurve2,
        dCurve3);

    // dP/dv
    vec3 dPdv = BezierDerivative(v,
        curve0,
        curve1,
        curve2,
        curve3);

    // Surface normal
    normal = normalize(cross(dPdu, dPdv));
    fragPos = position;
}