cbuffer cbPerObject
{
    float4x4 WVP;
};

struct VOut
{
	float4 position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

VOut main(float4 position : POSITION, float2 inTexCoord: TEXCOORD)
{
	VOut output;

    output.position = mul(position, WVP);
	output.TexCoord = inTexCoord;

	return output;
}
