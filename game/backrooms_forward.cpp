#include "backrooms_forward.h"

void ForwardPassInit(forward_pass* Pass)
{
    TextureInit(&Pass->Output, 1280, 720, TextureFormat_R8G8B8A8_Unorm, TextureUsage_RTV);
    TextureInit(&Pass->Depth, 1280, 720, TextureFormat_D32_Float, TextureUsage_DSV);
    TextureInitRTV(&Pass->Output);
    TextureInitDSV(&Pass->Depth);

    ShaderInit(&Pass->ForwardShader, "data/shaders/forward/Vertex.hlsl", "data/shaders/forward/Fragment.hlsl");

    rhi_material_config MaterialConfig;
    MaterialConfig.FrontFaceCCW = true;
    MaterialConfig.CullMode = CullMode_Back;
    MaterialConfig.FillMode = FillMode_Fill;
    MaterialConfig.CompareOP = CompareOP_Less;
    MaterialInit(&Pass->ForwardMaterial, MaterialConfig);
}

void ForwardPassRender(forward_pass* Pass, frame_graph_scene* Scene)
{
    TextureResetRTV();

    TextureBindRTV(&Pass->Output, &Pass->Depth, HMM_Vec4(0.1f, 0.2f, 0.3f, 1.0f));
    ShaderBind(&Pass->ForwardShader);
    MaterialBind(&Pass->ForwardMaterial);
    BufferBindUniform(&Scene->CameraBuffer, 0, UniformBind_Vertex);

    for (gpu_mesh Mesh : Scene->Meshes) {
        for (gltf_primitive Primitive : Mesh.Primitives) {
            BufferBindVertex(&Primitive.VertexBuffer);
            BufferBindIndex(&Primitive.IndexBuffer);
            BufferBindUniform(&Primitive.InstanceBuffer, 1, UniformBind_Vertex);
            VideoDrawIndexed(Primitive.IndexCount, 0);
        }
    }
}

void ForwardPassResize(forward_pass* Pass, u32 Width, u32 Height)
{
    TextureFree(&Pass->Depth);
    TextureFree(&Pass->Output);

    TextureInit(&Pass->Output, Width, Height, TextureFormat_R8G8B8A8_Unorm, TextureUsage_RTV);
    TextureInit(&Pass->Depth, Width, Height, TextureFormat_D32_Float, TextureUsage_DSV);
    TextureInitRTV(&Pass->Output);
    TextureInitDSV(&Pass->Depth);
}

void ForwardPassFree(forward_pass* Pass)
{
    MaterialFree(&Pass->ForwardMaterial);
    ShaderFree(&Pass->ForwardShader);
    TextureFree(&Pass->Depth);
    TextureFree(&Pass->Output);
}