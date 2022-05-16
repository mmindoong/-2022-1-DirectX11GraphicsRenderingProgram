//--------------------------------------------------------------------------------------
// File: VoxelShaders.fx
//
// Copyright (c) Kyung Hee University.
//--------------------------------------------------------------------------------------
#define NUM_LIGHTS (2)

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbChangeOnCameraMovement

  Summary:  Constant buffer used for view transformation and shading
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
cbuffer cbCqhangeOnCameraMovement : register(b0)
{
    matrix View;
    float4 CameraPosition;
}


/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbChangeOnResize

  Summary:  Constant buffer used for projection transformation
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
cbuffer cbChangeOnResize : register(b1)
{
    matrix Projection;
};


/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbChangesEveryFrame

  Summary:  Constant buffer used for world transformation, and the 
            color of the voxel
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
cbuffer cbChangesEveryFrame : register(b2)
{
    matrix World;
    float4 OutputColor;
};


/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbLights

  Summary:  Constant buffer used for shading
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/

cbuffer cbLights : register(b3)
{
    float4 LightPositions[NUM_LIGHTS];
    float4 LightColors[NUM_LIGHTS];
};

//--------------------------------------------------------------------------------------
/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Struct:   VS_INPUT

  Summary:  Used as the input to the vertex shader, 
            instance data included
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
struct VS_INPUT
{
    float4 Position : POSITION;
    float3 TexCoord : TEXCOORD;
    float3 Normal : NORMAL;
    row_major matrix Transform : INSTANCED_TRANSFORM;
   
};

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Struct:   PS_INPUT

  Summary:  Used as the input to the pixel shader, output of the 
            vertex shader
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float3 TexCoord : TEXCOORD;
    float3 Normal : NORMAL;
    float3 WorldPosition : WORLDPOS;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VSVoxel(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;

   // Update the position of the vertices based on the data for this particular instance.
    output.Position = mul(input.Position, input.Transform);
    output.Position = mul(output.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    
    // Store the texture coordinates for the pixel shader.
    output.TexCoord = input.TexCoord;
    output.Normal = normalize(mul(float4(input.Normal, 1), World).xyz);
    
    output.WorldPosition = (mul(input.Position, input.Transform)).xyz;
    return output;
    
}

//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PSVoxel(PS_INPUT input) : SV_Target
{
    float3 ambient = float3(0.1f, 0.1f, 0.1f);
    float3 diffuse = float3(0.0f, 0.0f, 0.0f);
    
    for (uint i = 0; i < NUM_LIGHTS; ++i)
    {
        ambient += float3(0.1f, 0.1f, 0.1f) * LightColors[i].xyz;
    }
    
    for (uint it = 0; it < NUM_LIGHTS; ++it)
    {
        float3 lightDirection = normalize(LightPositions[it].xyz - input.WorldPosition);
        diffuse += saturate(dot(input.Normal, lightDirection)) * LightColors[it].xyz;
    }
    return float4((ambient + diffuse) * OutputColor.xyz, 1.0f);
}