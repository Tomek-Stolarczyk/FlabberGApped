cbuffer cbPerObject
{
    float4x4 WVP;
};

struct VOut
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

VOut main(float4 position : POSITION, float4 color : COLOR)
{
	VOut output;

    output.position = mul(position, WVP);

	if (position.y > 0)
		output.color = float4(0.0f, 0.8f, 0.0f, 0.0f);
	else
		output.color =  float4(0.0f, 0.0f, 0.0f, 0.0f);
    output.color = color;

	return output;
}
