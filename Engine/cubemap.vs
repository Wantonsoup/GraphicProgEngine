////////////////////////////////////////////////////////////////////////////////
// Filename: cubemap.vs
////////////////////////////////////////////////////////////////////////////////


/////////////
// GLOBALS //
/////////////
cbuffer MatrixBuffer
{
	matrix worldMatrix; //NOT USED AS THE CUBEMAP REQUIRES NO POSITION IN THE WORLD
	matrix viewMatrix;
	matrix projectionMatrix;
};


//////////////
// TYPEDEFS //
//////////////
struct VS_INPUT
{
    float4 position : POSITION;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float3 texcoord : TEXCOORD0; //Notice that it's a float3 for the texture sampling
};


////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
////////////////////////////////////////////////////////////////////////////////
VS_OUTPUT VS_Skybox(VS_INPUT input)
{
    VS_OUTPUT output;

    //Don't use the worldMatrix as the skybox doesn't need a position in space
    output.position = mul(input.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

   // output.position.z = output.position.w; //You'll see this hack around, hopefully not needed with depth buffer being turned off

    // The direction vector serves as the cubemap lookup coordinate
    output.texcoord.x = input.position.x; //Negate to flip
    output.texcoord.yz = input.position.yz;

    return output;
}