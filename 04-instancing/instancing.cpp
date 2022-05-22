#include <bx/uint32_t.h>
#include "common.h"
#include "bgfx_utils.h"
// #include "logo.h"
#include "imgui/imgui.h"

namespace {

// 定义顶点属性
// Position 和 Color
struct PosColorVertex
{
	float m_x;
	float m_y;
	float m_z;
	uint32_t m_abgr;

	static void init()
	{
		ms_layout
			.begin()
			.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
			.add(bgfx::Attrib::Color0,   4, bgfx::AttribType::Uint8, true)
			.end();
	};

	static bgfx::VertexLayout ms_layout;
};

bgfx::VertexLayout PosColorVertex::ms_layout;

// cube, 8个顶点
static PosColorVertex s_cubeVertices[] =
{
	{-1.0f,  1.0f,  1.0f, 0xff000000 },
	{ 1.0f,  1.0f,  1.0f, 0xff0000ff },
	{-1.0f, -1.0f,  1.0f, 0xff00ff00 },
	{ 1.0f, -1.0f,  1.0f, 0xff00ffff },
	{-1.0f,  1.0f, -1.0f, 0xffff0000 },
	{ 1.0f,  1.0f, -1.0f, 0xffff00ff },
	{-1.0f, -1.0f, -1.0f, 0xffffff00 },
	{ 1.0f, -1.0f, -1.0f, 0xffffffff },
};

// 12个三角形
static const uint16_t s_cubeTriList[] =
{
	0, 1, 2, // 0
	1, 3, 2,
	4, 6, 5, // 2
	5, 6, 7,
	0, 2, 4, // 4
	4, 2, 6,
	1, 5, 3, // 6
	5, 7, 3,
	0, 4, 1, // 8
	4, 5, 1,
	2, 3, 6, // 10
	6, 3, 7,
};

class ExampleInstancing : public entry::AppI {
public:
	ExampleInstancing(const char* _name, const char* _description, const char* _url)
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

        PosColorVertex::init();

        vb_handle_ = bgfx::createVertexBuffer(
            bgfx::makeRef(s_cubeVertices, sizeof(s_cubeVertices)),
            PosColorVertex::ms_layout);

        ib_handle_ = bgfx::createIndexBuffer(
            bgfx::makeRef(s_cubeTriList, sizeof(s_cubeTriList)));

        // load shader
        program_ = loadProgram("vs_instancing", "fs_instancing");

        imguiCreate();

        timeOffset_ = bx::getHPCounter();
    }

    int shutdown() override {
        imguiDestroy();

		// Cleanup.
    	bgfx::destroy(ib_handle_);
		bgfx::destroy(vb_handle_);
		bgfx::destroy(program_);

		// Shutdown bgfx.
		bgfx::shutdown();

        return 0;
    }

    bool update() override {
        if (!entry::processEvents(width_, height_, debug_, reset_, &mouse_state_) ) {

            const bx::Vec3 at  = { 0.0f, 0.0f,   0.0f };
			const bx::Vec3 eye = { 0.0f, 0.0f, -35.0f };

            float view[16];
            bx::mtxLookAt(view, eye, at);

            float proj[16];
            bx::mtxProj(proj, 60.0f, float(width_) / float(height_), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);

            bgfx::setViewTransform(0, view, proj);
            bgfx::setViewRect(0, 0, 0, uint16_t(width_), uint16_t(height_));

            // This dummy draw call is here to make sure that view 0 is cleared
			// if no other draw calls are submitted to view 0.
			bgfx::touch(0);

            uint64_t state =
                BGFX_STATE_WRITE_RGB |
                BGFX_STATE_WRITE_A |
                BGFX_STATE_WRITE_Z |
                BGFX_STATE_DEPTH_TEST_LESS |
                BGFX_STATE_CULL_CW |
                BGFX_STATE_MSAA;

            float time = (float)( (bx::getHPCounter()-timeOffset_)/double(bx::getHPFrequency() ) );

            const int instanceStride = 80; // sizeof(float[16]) model_matrix + sizeof(float[4]) color
            const int instanceNum = 121;

            if (bgfx::getAvailInstanceDataBuffer(instanceNum, instanceStride) == instanceNum) {
                bgfx::InstanceDataBuffer idb;
                bgfx::allocInstanceDataBuffer(&idb, instanceNum, instanceStride);

                uint8_t *data = idb.data;

                for (uint32_t yy = 0; yy < 11; yy++) {
                    for (uint32_t xx = 0; xx < 11; xx++) {
                        float *mtx = (float *)data;
                        bx::mtxRotateXY(mtx, 0, time + yy * 0.22f);

                        // position
                        mtx[12] = -15.0f + float(xx)*3.0f;
						mtx[13] = -15.0f + float(yy)*3.0f;
						mtx[14] = 0.0f;

                        float *color = (float *)(data + 64);
                        color[0] = 1.0f;
                        color[1] = 1.0f;
                        color[2] = 1.0f;
                        color[3] = 1.0f;

                        data += instanceStride;
                    }
                }

                bgfx::setVertexBuffer(0, vb_handle_);
                bgfx::setIndexBuffer(ib_handle_);

                bgfx::setInstanceDataBuffer(&idb);

                bgfx::setState(BGFX_STATE_DEFAULT);
                bgfx::submit(0, program_);
            }

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

    bgfx::VertexBufferHandle vb_handle_;
    bgfx::IndexBufferHandle ib_handle_;
    bgfx::ProgramHandle program_;

    entry::MouseState mouse_state_;
    int64_t timeOffset_;
};

}

ENTRY_IMPLEMENT_MAIN(
	  ExampleInstancing
	, "Cube"
	, "Cube Test"
	, ""
	);
