// Based on "Temporal Antialiasing In Uncharted 4" by Ke XU

float4 ResolveTAA(float2 texCoord, Texture2D tex_history, Texture2D tex_current, Texture2D tex_velocity, SamplerState sampler_bilinear)
{
	float2 velocity			= GetVelocity(texCoord, tex_current, sampler_bilinear);
    float2 texCoord_history	= texCoord - velocity;
	
	// For non-existing and lighting change cases, clamp out too different history colors 
	float4 samples[9];
	samples[0] = tex_current.Sample(sampler_bilinear, texCoord + float2(-1, -1));
	samples[1] = tex_current.Sample(sampler_bilinear, texCoord + float2(0, -1));
	samples[2] = tex_current.Sample(sampler_bilinear, texCoord + float2(1, -1));
	samples[3] = tex_current.Sample(sampler_bilinear, texCoord + float2(-1, 0));
	samples[4] = tex_current.Sample(sampler_bilinear, texCoord + float2(0, 0));
	samples[5] = tex_current.Sample(sampler_bilinear, texCoord + float2(1, 0));
	samples[6] = tex_current.Sample(sampler_bilinear, texCoord + float2(-1, 1));
	samples[7] = tex_current.Sample(sampler_bilinear, texCoord + float2(0, 1));
	samples[8] = tex_current.Sample(sampler_bilinear, texCoord + float2(1, 1));
	float4 sampleMin = samples[0];
	float4 sampleMax = samples[0];
	[unroll]
	for (uint i = 1; i < 9; ++i)
	{
		sampleMin = min(sampleMin, samples[i]);
		sampleMax = max(sampleMax, samples[i]);
	}

	// Get current color
	float4 color_current = samples[4];

	// Compute history color
	float4 color_history 	= tex_history.Sample(sampler_bilinear, texCoord_history);
	//color_history 			= clamp(color_history, sampleMin, sampleMax);

	// Compute blend factor
	float blendfactor = 0.05f;
	//blendfactor = any(texCoord_history - saturate(texCoord_history)) ? 1.0f : blendfactor;
   
	// Resolve color
	float4 color_resolved = lerp(color_history, color_current, blendfactor);

	return color_resolved;
}