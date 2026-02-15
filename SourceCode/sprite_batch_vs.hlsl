struct VS_IN
{
    float3 pos : POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

struct VS_OUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

VS_OUT VSMain(VS_IN input)
{
    VS_OUT o;
    // Assumes input.pos is already in clip-space (-1..+1)
    o.pos = float4(input.pos, 1.0f);
    o.color = input.color;
    o.uv = input.uv;
    return o;
}
