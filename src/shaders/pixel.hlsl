Texture2D colorMap : register( t0 );
SamplerState colorSampler : register( s0 );

struct PS_Input
{
    float4 pos : SV_POSITION;
    float2 tex0 : TEXCOORD0;
    float3 norm : TEXCOORD1;
};
float4 PS_Main( PS_Input frag ) : SV_TARGET
{
    float3 lightDir = float3(1, -0.5f, 0);
    float intesity = dot(frag.norm, lightDir);
    if(intesity < 0.2f) {
        intesity = 0.2f;
    }
    float4 color = colorMap.Sample(colorSampler, frag.tex0.xy);
    color.r *= intesity;
    color.g *= intesity;
    color.b *= intesity;
    return color;
}
