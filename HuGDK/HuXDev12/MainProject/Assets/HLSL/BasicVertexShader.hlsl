cbuffer CameraBuffer : register(b0)
{
    float4x4 viewProj; // View * Projection çsóÒ
};

struct VSOutput
{
    float4 pos : SV_POSITION;
};

VSOutput BasicVS(float3 pos : POSITION)
{
    VSOutput output;
    output.pos = mul(viewProj, float4(pos, 1.0f));
    return output;
}