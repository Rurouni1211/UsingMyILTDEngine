Texture2D gTex : register(t0);
SamplerState gSamp : register(s0);

struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
    float2 uv : TEXCOORD;
};

float4 PSMain(PS_IN input) : SV_Target
{
    float4 tex = gTex.Sample(gSamp, input.uv);
    return tex * input.color;
}
