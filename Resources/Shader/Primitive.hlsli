struct VertexShaderOutput
{
    float4 position : SV_POSITION;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};
struct GSOutput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL0;
    float2 texcoord : TEXCOORD0;
};