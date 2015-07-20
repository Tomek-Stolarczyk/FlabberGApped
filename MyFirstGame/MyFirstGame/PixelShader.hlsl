Texture2D ObjTexture;
SamplerState ObjSamplerState;

float4 main(float4 position : SV_POSITION, float2 TexCoord : TEXCOORD) : SV_TARGET
{
	return ObjTexture.Sample(ObjSamplerState, TexCoord);
}