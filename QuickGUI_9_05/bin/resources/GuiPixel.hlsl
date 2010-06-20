
sampler image : register(s0);

struct FS_INPUT
{
	float2 UV0 : TEXCOORD0;
	float4 Color : COLOR0;
};

struct FS_OUTPUT
{
	float4 Color : COLOR0;
};

FS_OUTPUT main( FS_INPUT In )
{
	FS_OUTPUT Out;
	Out.Color = tex2D( image, In.UV0 ) * In.Color;
	
	return Out;
}
