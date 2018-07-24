#ifndef post_MRT_light_struct_VSH
#define post_MRT_light_struct_VSH



#ifndef LIGTS_VECTOR_SIZE_DEFAULT
#define LIGTS_VECTOR_SIZE_DEFAULT 40
#endif

float4	lightAmbient = float4 (0.1f, 0.1f, 0.1f, 1.0f);
const int LIGHTS_VECTOR_SIZE = LIGTS_VECTOR_SIZE_DEFAULT;
float4 lightsVector[ LIGTS_VECTOR_SIZE_DEFAULT ];
int lightsNumber = 0;


//-----------------------------------------------------------------------------
// Application of lights vector
//-----------------------------------------------------------------------------
//
//  int i = 0;
//  for (int j = 0; j < lightsNumber; ++j)
//  {
//      switch ((int) lightsVector[i + 1].w)
//      {
//          case 0:
//          {
//              // directional light calculation //
//
//              lightsVector[i + 0] = normalised direction;
//              lightsVector[i + 1].xyz = diffuse;
//              lightsVector[i + 2].xyz = specular;
//              i += 3;
//
//              break;
//          }
//
//          case 1:
//          {
//              // point light calculation //
//
//              lightsVector[i + 0] = position in world space;
//              lightsVector[i + 1].xyz = diffuse;
//              lightsVector[i + 2].xyz = specular;
//              lightsVector[i + 3].x = range in world space;
//              lightsVector[i + 3].y = attenuation0;
//              lightsVector[i + 3].z = attenuation1;
//              lightsVector[i + 3].w = attenuation2;
//              i += 4;
//
//              break;
//          }
//
//          case 2:
//          {
//              // spot light calculation //
//
//              lightsVector[i + 0] = position in world space;
//              lightsVector[i + 1].xyz = diffuse;
//              lightsVector[i + 2].xyz = specular;
//              lightsVector[i + 3].x = range in world space;
//              lightsVector[i + 3].y = attenuation0;
//              lightsVector[i + 3].z = attenuation1;
//              lightsVector[i + 3].w = attenuation2;
//              lightsVector[i + 4].x = cosine of hotspot angle;
//              lightsVector[i + 4].y = cosine of cutoff angle;
//              lightsVector[i + 4].z = falloff value;
//              lightsVector[i + 4].w = empty (reserved);
//              lightsVector[i + 5] = normalised direction;
//              i += 6;
//
//              break;
//          }
//      }
//  }



#endif //post_MRT_light_struct_VSH
