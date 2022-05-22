#include <bx/uint32_t.h>
#include "common.h"
#include "bgfx_utils.h"
// #include "logo.h"
#include "imgui/imgui.h"

namespace {

class ExampleMesh : public entry::AppI {
public:
	ExampleMesh(const char* _name, const char* _description, const char* _url)
		: entry::AppI(_name, _description, _url)
	{
	}

    void init(int32_t _argc, const char* const* _argv, uint32_t _width, uint32_t _height) override {

        width_ = _width;
        height_ = _height;
        debug_  = BGFX_DEBUG_NONE;
		reset_  = BGFX_RESET_VSYNC;

        Args args(_argc, _argv);

        bgfx::Init init;
        init.type = args.m_type;
        init.vendorId = args.m_pciId;
        init.resolution.width = width_;
        init.resolution.height = height_;
        init.resolution.reset = reset_;

        bgfx::init(init);
        bgfx::setDebug(debug_);

        bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);

        // load shader
        program_ = loadProgram("vs_mesh", "fs_mesh");

        mesh_ = meshLoad("meshes/bunny.bin");

        imguiCreate();

        timeOffset_ = bx::getHPCounter();
    }

    int shutdown() override {
        imguiDestroy();

		// Cleanup.
		bgfx::destroy(program_);

        meshUnload(mesh_);

		// Shutdown bgfx.
		bgfx::shutdown();

        return 0;
    }

    bool update() override {
        if (!entry::processEvents(width_, height_, debug_, reset_, &mouse_state_) ) {

            const bx::Vec3 at  = { 0.0f, 1.0f,  0.0f };
			const bx::Vec3 eye = { 0.0f, 1.0f, -2.5f };

            float view[16];
            bx::mtxLookAt(view, eye, at);

            float proj[16];
            bx::mtxProj(proj, 60.0f, float(width_) / float(height_), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);

            bgfx::setViewTransform(0, view, proj);
            bgfx::setViewRect(0, 0, 0, uint16_t(width_), uint16_t(height_));

            // This dummy draw call is here to make sure that view 0 is cleared
			// if no other draw calls are submitted to view 0.
			bgfx::touch(0);

            float time = (float)( (bx::getHPCounter()-timeOffset_)/double(bx::getHPFrequency() ) );
            // float mtx[16];
            // bx::mtxIdentity(mtx);

			float mtx[16];
			bx::mtxRotateXY(mtx
				, 0.0f
				, time * 0.33f
			);

            meshSubmit(mesh_, 0, program_, mtx);
            bgfx::frame();
            return true;
        }
        return false;
    }

private:
    uint32_t width_;
    uint32_t height_;
    uint32_t debug_;
	uint32_t reset_;

    Mesh *mesh_;
    bgfx::VertexBufferHandle vb_handle_;
    bgfx::IndexBufferHandle ib_handle_;
    bgfx::ProgramHandle program_;

    entry::MouseState mouse_state_;
    int64_t timeOffset_;
};

}

ENTRY_IMPLEMENT_MAIN(
	  ExampleMesh
	, "Mesh"
	, "Mesh Test"
	, ""
	);
