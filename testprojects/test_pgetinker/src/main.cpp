#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_PGEX_GRAPHICS3D
#include "olcPGEX_Graphics3D.h" 

//#include "J7Model.h"

// Override base class with your custom functionality
class Example : public olc::PixelGameEngine
{
public:
    Example()
    {
        // Name your application
        sAppName = "My3dTest";
    }
    
public:
	olc::GFX3D::PipeLine renderer;

	olc::GFX3D::vec3d vUp = { 0, 1, 0 };
	olc::GFX3D::vec3d vCamera = { 0, 0, -3 };
	olc::GFX3D::vec3d vLookDir = { 0, 0,-1 };
	olc::GFX3D::vec3d vSun = {0, 0, -1};

	float fYaw = 0.0f;		// FPS Camera rotation in XZ plane
	float fTheta = 0.0f;	// Spins World transform
    
    //J7* j7;
    olc::GFX3D::mesh model;

    bool OnUserCreate() override
    {
        model.LoadOBJFile("models/J7_body.obj");

        for(auto& tri : model.tris)
        {
            std::swap(tri.p[0], tri.p[2]);
        }
        //j7 = new J7();

        olc::GFX3D::ConfigureDisplay();

        renderer.SetProjection(90.0f, (float)ScreenHeight() / (float)ScreenWidth(), 0.1f, 1000.0f, 0.0f, 0.0f, ScreenWidth(), ScreenHeight());

        return true;
    }
    
    bool OnUserUpdate(float fElapsedTime) override
    {
        fTheta+=fElapsedTime*1.0f;
        // Start of 3D Stuff
        olc::GFX3D::mat4x4 matRotateY = olc::GFX3D::Math::Mat_MakeRotationY(fTheta);
        
        olc::GFX3D::mat4x4 matTrans;
        matTrans = olc::GFX3D::Math::Mat_MakeTranslation(0.0f, 0.0f, 0.0f); //no need translating for now

        olc::GFX3D::mat4x4 matWorld;
        matWorld = olc::GFX3D::Math::Mat_MakeIdentity();;	// Form World Matrix
        matWorld = olc::GFX3D::Math::Mat_MultiplyMatrix(matWorld, matRotateY); // Transform by rotation
        matWorld = olc::GFX3D::Math::Mat_MultiplyMatrix(matWorld, matTrans); // Transform by translation

        // Create a "Point At"
        olc::GFX3D::vec3d vTarget = { 0,0,1 };
        olc::GFX3D::mat4x4 matCameraRot = olc::GFX3D::Math::Mat_MakeRotationY(fYaw);
        vLookDir = olc::GFX3D::Math::Mat_MultiplyVector(matCameraRot, vTarget);
        vTarget = olc::GFX3D::Math::Vec_Add(vCamera, vLookDir);
       
        renderer.SetCamera(vCamera, vTarget, vUp);

        olc::GFX3D::vec3d vForward = olc::GFX3D::Math::Vec_Mul(vLookDir, 8.0f * fElapsedTime);

        Clear(olc::DARK_CYAN);

        olc::GFX3D::ClearDepth();

        renderer.SetTransform(matWorld);
        renderer.SetLightSource(1, olc::GFX3D::LIGHTS::LIGHT_DIRECTIONAL, olc::WHITE, vSun);
        
        //auto tris = j7->GetTris();

        renderer.Render(model.tris, olc::GFX3D::RENDERFLAGS::RENDER_FLAT|olc::GFX3D::RENDERFLAGS::RENDER_DEPTH|olc::GFX3D::RENDERFLAGS::RENDER_LIGHTS);
        //renderer.Render(model.tris, olc::GFX3D::RENDERFLAGS::RENDER_WIRE|olc::GFX3D::RENDERFLAGS::RENDER_DEPTH|olc::GFX3D::RENDER_CULL_CW);

        if (GetKey(olc::Key::UP).bHeld)
            vCamera.y += 8.0f * fElapsedTime;	// Travel Upwards
        
        if (GetKey(olc::Key::DOWN).bHeld)
            vCamera.y -= 8.0f * fElapsedTime;	// Travel Downwards


        // Dont use these two in FPS mode, it is confusing :P
        if (GetKey(olc::Key::LEFT).bHeld)
            vCamera.x -= 8.0f * fElapsedTime;	// Travel Along X-Axis

        if (GetKey(olc::Key::RIGHT).bHeld)
            vCamera.x += 8.0f * fElapsedTime;	// Travel Along X-Axis

            // Standard FPS Control scheme, but turn instead of strafe
        if (GetKey(olc::Key::W).bHeld)
            vCamera = olc::GFX3D::Math::Vec_Add(vCamera, vForward);

        if (GetKey(olc::Key::S).bHeld)
            vCamera = olc::GFX3D::Math::Vec_Sub(vCamera, vForward);

        if (GetKey(olc::Key::A).bHeld)
            fYaw -= 2.0f * fElapsedTime;

        if (GetKey(olc::Key::D).bHeld)
            fYaw += 2.0f * fElapsedTime;
        
        auto size = GetWindowSize();
        std::cout << "Size: " << size.x << " " << size.y << std::endl;

        return true;
    }
};

int main()
{
    Example demo;
    if (demo.Construct(256, 240, 2, 2))
        demo.Start();
    return 0;
}