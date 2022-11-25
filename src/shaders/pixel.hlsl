Texture2D colorMap : register( t0 );
SamplerState colorSampler : register( s0 );

struct PS_Input
{
    float4 pos : SV_POSITION;
    float2 tex0 : TEXCOORD0;
    float3 norm : TEXCOORD1;
    float3 viewPos : TEXCOORD2;
    float3 fragPos : TEXCOORD3;
};
float4 PS_Main( PS_Input frag ) : SV_TARGET
{
    float3 color = colorMap.Sample(colorSampler, frag.tex0.xy).rgb;
    float3 lightPos = float3(-10, 6, 40);
    float3 lightColor = float3(0.8f, 0.7f, 0.2f);
    float3 lightDir = normalize(frag.fragPos - lightPos);

    // ambient
    float ambientStrength = 0.2f;
    float3 ambient = mul(lightColor, ambientStrength);
    // diffuse
    float diffuseStrength = max(dot(frag.norm, lightDir), 0.0f);
    float3 diffuse = mul(lightColor, diffuseStrength);
    // specular
    float specularStrength = 1.0f;
    float3 viewDir = normalize(frag.fragPos - frag.viewPos);
    float3 reflectDir = reflect(-lightDir, frag.norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 32.0f); 
    float3 specular = mul(spec * lightColor, specularStrength);

    float3 result = (ambient + diffuse + specular) * color;

    return float4(result, 1.0f);
}
