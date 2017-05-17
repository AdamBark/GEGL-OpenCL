static const char* gaussian_blur_selective_cl_source =
"kernel void                                                                   \n"
"cl_gblur_selective(global const float4 *in,                                   \n"
"                   global const float4 *delta,                                \n"
"                   global       float4 *out,                                  \n"
"                          const float   radius,                               \n"
"                          const float   max_delta)                            \n"
"{                                                                             \n"
"  const int gidx       = get_global_id(0);                                    \n"
"  const int gidy       = get_global_id(1);                                    \n"
"  const int iradius    = (int)radius;                                         \n"
"  const int dst_width  = get_global_size(0);                                  \n"
"  const int src_width  = dst_width + iradius * 2;                             \n"
"                                                                              \n"
"  const int center_gid1d = (gidy + iradius) * src_width + gidx + iradius;     \n"
"  const float4 center_pix = in[center_gid1d];                                 \n"
"  const float3 center_delta = delta[center_gid1d].xyz;                        \n"
"                                                                              \n"
"  float3 accumulated = 0.0f;                                                  \n"
"  float3 count       = 0.0f;                                                  \n"
"                                                                              \n"
"  for (int v = -iradius; v <= iradius; v++)                                   \n"
"    {                                                                         \n"
"      for (int u = -iradius; u <= iradius; u++)                               \n"
"        {                                                                     \n"
"          const int i = gidx + iradius + u;                                   \n"
"          const int j = gidy + iradius + v;                                   \n"
"          const int gid1d = i + j * src_width;                                \n"
"                                                                              \n"
"          const float4 src_pix = in[gid1d];                                   \n"
"          const float3 delta_pix = delta[gid1d].xyz;                          \n"
"                                                                              \n"
"          const float gaussian_weight = exp(-0.5f * (u * u + v * v) / radius);\n"
"                                                                              \n"
"          const float weight = gaussian_weight * src_pix.w;                   \n"
"          const float3 diff = center_delta - delta_pix;                       \n"
"          const float3 w = convert_float3 (fabs (diff) <= max_delta);         \n"
"          accumulated += w * weight * src_pix.xyz;                            \n"
"          count += w * weight;                                                \n"
"        }                                                                     \n"
"    }                                                                         \n"
"                                                                              \n"
"  const float3 out_v = select (center_pix.xyz,                                \n"
"                               accumulated / count,                           \n"
"                               count != 0.0f);                                \n"
"  out[gidx + gidy * dst_width] = (float4)(out_v, center_pix.w);               \n"
"}                                                                             \n"
;
