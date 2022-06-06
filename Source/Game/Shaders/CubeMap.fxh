//--------------------------------------------------------------------------------------
// File: CubeMap.fxh
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License (MIT).
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
/*--------------------------------------------------------------------
  TODO: Declare a diffuse texture and a sampler state (remove the comment)
--------------------------------------------------------------------*/
TextureCube SkyCube : register(t0);
SamplerState diffuseSampler : register(s0);

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbChangeOnCameraMovement

  Summary:  Constant buffer used for view transformation and shading
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
/*--------------------------------------------------------------------
  TODO: cbChangeOnCameraMovement definition (remove the comment)
--------------------------------------------------------------------*/
cbuffer cbChangeOnCameraMovement : register(b0)
{
    matrix View;
    float4 CameraPosition;
};

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbChangeOnResize

  Summary:  Constant buffer used for projection transformation
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
/*--------------------------------------------------------------------
  TODO: cbChangeOnResize definition (remove the comment)
--------------------------------------------------------------------*/
cbuffer cbChangeOnResize : register(b1)
{
    matrix Projection;
};

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Cbuffer:  cbChangesEveryFrame

  Summary:  Constant buffer used for world transformation, and the 
            color of the voxel
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
/*--------------------------------------------------------------------
  TODO: cbChangesEveryFrame definition (remove the comment)
--------------------------------------------------------------------*/
cbuffer cbChangesEveryFrame : register(b2)
{
    matrix World;
    float4 OutputColor;
    bool HasNormalMap;
};

//--------------------------------------------------------------------------------------
/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Struct:   VS_INPUT

  Summary:  Used as the input to the vertex shader
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
/*--------------------------------------------------------------------
  TODO: VS_INPUT definition (remove the comment)
--------------------------------------------------------------------*/
struct VS_INPUT
{
    float4 Position : POSITION;
    float3 Normal : NORMAL;
};

/*C+C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C+++C
  Struct:   PS_INPUT

  Summary:  Used as the input to the pixel shader, output of the 
            vertex shader
C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C---C-C*/
/*--------------------------------------------------------------------
  TODO: PS_INPUT definition (remove the comment)
--------------------------------------------------------------------*/
struct PS_INPUT
{
    float4 Position : SV_POSITION;
    float3 TexCoord : TEXCOORD0;
    float3 WorldPosition : WORLDPOS;
    float3 Normal : NORMAL;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
/*--------------------------------------------------------------------
  TODO: Vertex Shader function VSCubeMap definition (remove the comment)
--------------------------------------------------------------------*/
PS_INPUT VSCubeMap(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
   
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);

    output.TexCoord = input.Position.xyz;

    return output;
}


PS_INPUT VSEnvironmentMap(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;

    output.Position = mul(input.Position, World);
    output.WorldPosition = output.Position;
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    
    output.Normal = mul(float4(input.Normal, 0.0f), World).xyz;
    
    //float3 worldPosition = mul(input.Position, World).xyz;
    //float3 incident = normalize(worldPosition - (float) CameraPosition);

	// Reflection Vector for cube map: R = I - 2*N * (I.N)
    //output.ReflectionVector = reflect(incident, normal);
	
    return output;
}



//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
/*--------------------------------------------------------------------
  TODO: Pixel Shader function PSCubeMap definition (remove the comment)
--------------------------------------------------------------------*/
float4 PSCubeMap(PS_INPUT input) : SV_Target
{
    return SkyCube.Sample(diffuseSampler, input.TexCoord);
}

float4 PSEnvironmentMap(PS_INPUT input) : SV_TARGET
{
    float4 output = (float4) 0;

    float3 incident = normalize(input.WorldPosition - (float) CameraPosition);
    float3 normal = normalize(mul(float4(input.Normal, 0), World).xyz);
    // Reflection Vector for cube map: R = I - 2*N * (I.N)
    float3 ReflectionVector = reflect(incident, normal);
    
    //float4 color = SkyCube.Sample(diffuseSampler, 0.1f);
    //float3 ambient = (float) OutputColor * color.rgb;
    float3 environment = SkyCube.Sample(diffuseSampler, ReflectionVector);

    return float4(environment, 0.5f);
    //return float4(saturate(lerp(ambient, environment, 0.6f), color.a);

}
