struct VS_Input
{
    float3 pos : POSITION;
    float4 color : COLOR;
};

struct VS_Output
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

VS_Output VS(VS_Input input)
{
    VS_Output output;
    
    output.pos = float4(input.pos, 1.0f);
    output.color = input.color;

    return output;
}

float4 PS(VS_Output input) : SV_Target
{
    return input.color;
}