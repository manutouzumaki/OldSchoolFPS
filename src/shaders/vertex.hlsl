cbuffer CBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix proj;
    float3 viewPos;
}

struct VS_Input
{
   float3 pos : POSITION;
   float2 tex0 : TEXCOORD0;
   float3 norm : NORMAL;
};
struct PS_Input
{
   float4 pos : SV_POSITION;
   float2 tex0 : TEXCOORD0;
   float3 norm : TEXCOORD1;
   float3 viewPos : TEXCOORD2;
   float3 fragPos : TEXCOORD3;
};
PS_Input VS_Main( VS_Input vertex )
{
   PS_Input vsOut = ( PS_Input )0;

    float4 worldPos = mul(float4(vertex.pos, 1.0f), world);
    vsOut.pos = mul(worldPos, view);
    vsOut.pos = mul(vsOut.pos, proj);

    vsOut.tex0 = vertex.tex0;

    vsOut.norm = mul(vertex.norm, (float3x3)world);
    vsOut.norm = normalize(vsOut.norm);

    vsOut.viewPos = viewPos;

    vsOut.fragPos = mul(float4(vertex.pos, 1.0f), world).xyz;

    return vsOut;
}
