/*
Copyright(c) 2016-2020 Panos Karabelas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
copies of the Software, and to permit persons to whom the Software is furnished
to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

//= INCLUDES =====================      
#include "BRDF.hlsl"              
#include "ShadowMapping.hlsl"
#include "VolumetricLighting.hlsl"
//================================

struct PixelOutputType
{
	float3 diffuse		: SV_Target0;
	float3 specular		: SV_Target1;
	float3 volumetric	: SV_Target2;
};

PixelOutputType mainPS(Pixel_PosUv input)
{
	PixelOutputType light_out;
    light_out.diffuse       = 0.0f;
	light_out.specular 		= 0.0f;
    light_out.volumetric    = 0.0f;

    // Sample normal
    float4 normal_sample = tex_normal.Sample(sampler_point_clamp, input.uv);

    // Fill surface struct
    Surface surface;
    surface.uv                      = input.uv;
    surface.depth                   = tex_depth.Sample(sampler_point_clamp, surface.uv).r;
    surface.position                = get_position(surface.depth, surface.uv);
    surface.normal                  = normal_decode(normal_sample.xyz);
    surface.camera_to_pixel         = normalize(surface.position - g_camera_position.xyz);
    surface.camera_to_pixel_length  = length(surface.position - g_camera_position.xyz);

    // Create material
    Material material;
    {
        float4 sample_albedo    = tex_albedo.Sample(sampler_point_clamp, input.uv);
        float4 sample_material  = tex_material.Sample(sampler_point_clamp, input.uv);
        
        material.albedo         = sample_albedo.rgb;
        material.roughness      = sample_material.r;
        material.metallic       = sample_material.g;
        material.emissive       = sample_material.b;
        material.occlusion      = min(normal_sample.a, tex_ssao.Sample(sampler_point_clamp, input.uv).r); // min(occlusion, ssao)
        material.F0             = lerp(0.04f, material.albedo, material.metallic);
        material.is_transparent = sample_albedo.a != 1.0f;
        material.is_sky         = sample_material.a == 0.0f;
    }
    
    // Fill light struct
    Light light;
    light.color 	                = color.xyz;
    light.position 	                = position.xyz;
    light.intensity 			    = intensity_range_angle_bias.x;
    light.range 				    = intensity_range_angle_bias.y;
    light.angle 				    = intensity_range_angle_bias.z;
    light.bias					    = intensity_range_angle_bias.w;
    light.normal_bias 			    = normalBias_shadow_volumetric_contact.x;
    light.cast_shadows 		        = normalBias_shadow_volumetric_contact.y;
    light.cast_contact_shadows 	    = normalBias_shadow_volumetric_contact.z;
    light.cast_transparent_shadows  = color.w;
    light.is_volumetric 	        = normalBias_shadow_volumetric_contact.w;
    light.distance_to_pixel         = length(surface.position - light.position);
    #if DIRECTIONAL
    light.array_size    = 4;
    light.direction	    = direction.xyz; 
    light.attenuation   = 1.0f;
    #elif POINT
    light.array_size    = 1;
    light.direction	    = normalize(surface.position - light.position);
    light.attenuation   = saturate(1.0f - (light.distance_to_pixel / light.range)); light.attenuation *= light.attenuation;    
    #elif SPOT
    light.array_size    = 1;
    light.direction	    = normalize(surface.position - light.position);
    float cutoffAngle   = 1.0f - light.angle;
    float theta         = dot(direction.xyz, light.direction);
    float epsilon       = cutoffAngle - cutoffAngle * 0.9f;
    light.attenuation   = saturate((theta - cutoffAngle) / epsilon); // attenuate when approaching the outer cone
    light.attenuation   *= saturate(1.0f - light.distance_to_pixel / light.range); light.attenuation *= light.attenuation;
    #endif
    light.intensity     *= light.attenuation;
    
    // Shadow 
    {
        float4 shadow = 1.0f;
        
        // Shadow mapping
        [branch]
        if (light.cast_shadows)
        {
            shadow = Shadow_Map(surface, light, material.is_transparent);

            // Volumetric lighting (requires shadow maps)
            [branch]
            if (light.is_volumetric)
            {
                light_out.volumetric.rgb = VolumetricLighting(surface, light);
            }
        }
        
        // Screen space shadows
        [branch]
        if (light.cast_contact_shadows)
        {
            shadow.a = min(shadow.a, ScreenSpaceShadows(surface, light));
        }
    
        // Occlusion from texture and ssao
        shadow.a = min(shadow.a, material.occlusion);
        
        // Modulate light intensity and color
        light.intensity *= shadow.a;
        light.color     *= shadow.rgb;
    }

    // Reflectance equation
    [branch]
    if (light.intensity > 0.0f && !material.is_sky)
    {
        // Compute some stuff
        float3 l		= -light.direction;
        float3 v        = -surface.camera_to_pixel;
        float3 h 		= normalize(v + l);
        float v_dot_h 	= saturate(dot(v, h));
        float n_dot_v   = saturate(dot(surface.normal, v));
        float n_dot_l   = saturate(dot(surface.normal, l));
        float n_dot_h   = saturate(dot(surface.normal, h));
        float3 radiance	= light.color * light.intensity * n_dot_l;
    
        // BRDF components
        float3 F 			= 0.0f;
        float3 cDiffuse 	= BRDF_Diffuse(material, n_dot_v, n_dot_l, v_dot_h);	
        float3 cSpecular 	= BRDF_Specular(material, n_dot_v, n_dot_l, n_dot_h, v_dot_h, F);

        // SSR
        float3 light_reflection = 0.0f;
        float2 sample_ssr       = tex_ssr.Sample(sampler_point_clamp, input.uv).xy;
        [branch]
        if (g_ssr_enabled && (sample_ssr.x * sample_ssr.y) != 0.0f)
        {
            light_reflection = saturate(tex_frame.Sample(sampler_bilinear_clamp, sample_ssr.xy).rgb * F);
        }
    
        light_out.diffuse.rgb   = cDiffuse * radiance * energy_conservation(F, material.metallic);
        light_out.specular.rgb	= cSpecular * radiance + light_reflection;
    }

	return light_out;
}
