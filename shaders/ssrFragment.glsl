#version 410 core
// ============================================================================
// Screen-space ray tracing post-process: reflections (SSR) + ambient occlusion.
//
// This is "fast, not realistic" ray tracing. We do NOT trace the scene geometry.
// Instead we trace rays through the G-buffer (world-space position + normal that
// the gBuffer prepass rendered), so every "ray" is just a march that reprojects
// world points back to screen UVs and reads what is already on screen.
//
// macOS is capped at OpenGL 4.1 (GLSL 410): no compute shaders, no SSBOs, no
// hardware RT. A full-screen fragment pass like this is the only way to trace
// in-engine, and it reuses the deferred buffers we already produce.
// ============================================================================

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D gPosition;   // world-space fragment position (RGB16F), 0 = background
uniform sampler2D gNormal;     // world-space normal (RGB16F),       0 = background
uniform sampler2D sceneColor;  // the lit forward-render result we reflect/shade

uniform mat4 view;
uniform mat4 projection;
uniform vec3 viewPos;

// toggles / tunables (driven from GameState via the UI)
uniform int   ssrEnabled;
uniform int   aoEnabled;
uniform int   shadowEnabled;      // screen-space ray-traced contact shadows
uniform float reflectionStrength; // overall reflection blend  (~0.5)
uniform float stepScale;          // march step as a fraction of view distance (~1.0)
uniform int   maxSteps;           // ray march iterations (~48)
uniform float thicknessScale;     // hit tolerance vs step length (~2.0)
uniform float aoStrength;         // how much AO darkens the image (~1.0)
uniform float aoRadiusScale;      // AO sample radius vs view distance (~0.02)
uniform float shadowStrength;     // how dark contact shadows get (~0.8)
uniform float shadowStepScale;    // shadow march step vs view distance (~0.5)
uniform int   shadowMaxSteps;     // shadow march iterations (~32)
uniform vec3  lightPos;           // world-space position we march the shadow ray toward

// Is this texel real geometry? Background was cleared to 0 so its normal is ~0.
bool isGeometry(vec3 n) { return dot(n, n) > 0.25; }

// Linear view-space depth (distance in front of the camera) of a world point.
float viewDepth(vec3 worldPos) { return -(view * vec4(worldPos, 1.0)).z; }

// Project a world point to screen UV; returns false if behind camera / off-screen.
bool projectToUV(vec3 worldPos, out vec2 uv)
{
    vec4 clip = projection * view * vec4(worldPos, 1.0);
    if (clip.w <= 0.0) return false;
    uv = (clip.xy / clip.w) * 0.5 + 0.5;
    return uv.x >= 0.0 && uv.x <= 1.0 && uv.y >= 0.0 && uv.y <= 1.0;
}

// Cheap per-pixel hash to rotate the AO kernel and hide banding without a noise texture.
float hash(vec2 p)
{
    return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

// 12 fixed hemisphere directions (z-up); rotated into the surface frame per pixel.
const vec3 AO_KERNEL[12] = vec3[12](
    vec3( 0.42, -0.12, 0.30), vec3(-0.34,  0.28, 0.45), vec3( 0.18,  0.51, 0.22),
    vec3(-0.49, -0.21, 0.18), vec3( 0.05, -0.46, 0.55), vec3( 0.62,  0.20, 0.41),
    vec3(-0.27,  0.55, 0.61), vec3( 0.33, -0.58, 0.27), vec3(-0.61, -0.30, 0.52),
    vec3( 0.51,  0.44, 0.70), vec3(-0.15,  0.10, 0.85), vec3( 0.09, -0.05, 0.40)
);

// ---- Screen-space reflection -----------------------------------------------
// Reflect the view ray off the surface normal and march in world space. Each step
// reprojects to screen and compares the ray's view depth against the stored scene
// depth at that UV; a small positive gap (the ray just slipped behind a surface)
// counts as a hit, and we read the already-lit colour there.
vec3 traceReflection(vec3 P, vec3 N, out float hitMask)
{
    hitMask = 0.0;
    vec3 V = normalize(P - viewPos);
    vec3 R = normalize(reflect(V, N));

    // Scale-invariant: step is a fraction of how far the surface is from the camera,
    // so the same settings work for both tiny props and huge scenes (e.g. Sponza).
    float dist = length(P - viewPos);
    float step = max(dist * 0.01 * stepScale, 1e-4);
    float thickness = step * thicknessScale;

    vec3 ray = P + N * step;          // start slightly off the surface to avoid self-hit
    for (int i = 0; i < maxSteps; ++i)
    {
        ray += R * step;

        vec2 uv;
        if (!projectToUV(ray, uv)) break;          // left the screen -> no reflection data

        vec3 sN = texture(gNormal, uv).xyz;
        if (!isGeometry(sN)) continue;             // sky / empty - keep marching

        vec3 sP = texture(gPosition, uv).xyz;
        float gap = viewDepth(ray) - viewDepth(sP);
        if (gap > 0.0 && gap < thickness)
        {
            // fade near screen edges so reflections don't pop at the borders
            vec2 e = smoothstep(0.0, 0.15, uv) * (1.0 - smoothstep(0.85, 1.0, uv));
            hitMask = e.x * e.y;
            return texture(sceneColor, uv).rgb;
        }
    }
    return vec3(0.0);
}

// ---- Screen-space ambient occlusion ----------------------------------------
float computeAO(vec3 P, vec3 N)
{
    float radius = max(length(P - viewPos) * aoRadiusScale, 1e-4);

    // build a surface-aligned frame, randomly spun per pixel
    float a = hash(TexCoords) * 6.2831853;
    vec3 rnd = normalize(vec3(cos(a), sin(a), 0.0));
    vec3 tangent = normalize(rnd - N * dot(rnd, N));
    vec3 bitangent = cross(N, tangent);
    mat3 TBN = mat3(tangent, bitangent, N);

    float occlusion = 0.0;
    for (int i = 0; i < 12; ++i)
    {
        vec3 samplePos = P + (TBN * AO_KERNEL[i]) * radius;
        vec2 uv;
        if (!projectToUV(samplePos, uv)) continue;

        vec3 oN = texture(gNormal, uv).xyz;
        if (!isGeometry(oN)) continue;

        float occluderDepth = viewDepth(texture(gPosition, uv).xyz);
        float sampleDepth   = viewDepth(samplePos);

        // occluded only when a nearer surface sits in front of the sample point,
        // and range-checked so distant geometry doesn't cause haloing
        float rangeCheck = smoothstep(0.0, 1.0, radius / max(abs(sampleDepth - occluderDepth), 1e-4));
        if (occluderDepth < sampleDepth - radius * 0.05)
            occlusion += rangeCheck;
    }
    return occlusion / 12.0;
}

// ---- Screen-space ray-traced contact shadows -------------------------------
// March a ray from the surface toward the light. If it slips behind a nearer
// surface (an occluder sitting between this point and the light) before reaching
// the light, the point is shadowed. Returns 1.0 = occluded, 0.0 = lit.
// Same machinery as the reflection march, just a different ray direction.
float traceShadow(vec3 P, vec3 N, vec3 L)
{
    float dist = length(P - viewPos);
    float step = max(dist * 0.01 * shadowStepScale, 1e-4);
    float thickness = step * thicknessScale;

    vec3 ray = P + N * step * 2.0;       // bias off the surface to avoid self-shadow acne
    for (int i = 0; i < shadowMaxSteps; ++i)
    {
        ray += L * step;

        vec2 uv;
        if (!projectToUV(ray, uv)) break;          // ran off screen unobstructed -> lit

        vec3 sN = texture(gNormal, uv).xyz;
        if (!isGeometry(sN)) continue;             // sky gap -> keep marching

        float gap = viewDepth(ray) - viewDepth(texture(gPosition, uv).xyz);
        if (gap > 0.0 && gap < thickness)
            return 1.0;                            // occluder found between point and light
    }
    return 0.0;
}

void main()
{
    vec3 base = texture(sceneColor, TexCoords).rgb;
    vec3 N = texture(gNormal, TexCoords).xyz;

    // Background / sky: nothing to trace, pass the lit colour through untouched.
    if (!isGeometry(N))
    {
        FragColor = vec4(base, 1.0);
        return;
    }

    N = normalize(N);
    vec3 P = texture(gPosition, TexCoords).xyz;
    vec3 result = base;

    if (aoEnabled == 1)
    {
        float ao = computeAO(P, N);
        result *= (1.0 - clamp(ao * aoStrength, 0.0, 1.0));
    }

    if (shadowEnabled == 1)
    {
        vec3 L = normalize(lightPos - P);
        float occ = traceShadow(P, N, L);
        result *= (1.0 - clamp(occ * shadowStrength, 0.0, 1.0));
    }

    if (ssrEnabled == 1)
    {
        float hitMask;
        vec3 refl = traceReflection(P, N, hitMask);
        // Schlick-style Fresnel: reflections strengthen at grazing angles.
        vec3 V = normalize(P - viewPos);
        float fresnel = pow(1.0 - max(dot(-V, N), 0.0), 3.0);
        fresnel = mix(0.15, 1.0, fresnel); // keep a little reflection head-on
        result += refl * hitMask * fresnel * reflectionStrength;
    }

    FragColor = vec4(result, 1.0);
}
