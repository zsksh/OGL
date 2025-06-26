#pragma once
#include <limits>
#include<vector>
#include<unordered_map>
#include<core/vector.h>
#include<renderer/rhi_struct.h>
#include<renderer/texture.h>
namespace CGL {
    const std::vector<vec2> msaa_offsets_4x = {
    {-1.0f / 4.0f, -3.0f / 4.0f}, {3.0f / 4.0f, -1.0f / 4.0f},
    {-3.0f / 4.0f,  1.0f / 4.0f}, {1.0f / 4.0f,  3.0f / 4.0f}
    };
    enum class AttachmentPoint {
        COLOR0, COLOR1, COLOR2, COLOR3, COLOR4,
        DEPTH,
        STENCIL,
        DEPTH_STENCIL
    };

    

  //  class ColorBuffer {
   // public:
  //      ColorBuffer(int width, int height);
   //     void setPixel(int x, int y, const Color& color);
   //     Color getPixel(int x, int y) const;
   //     void clear(const Color& color);

  //  private:
  //      int width;
   //     int height;
   //     std::vector<Color> data;
   // };

    class StencilBuffer {
    public:
        StencilBuffer(int width, int height);
        void setStencil(int x, int y, uint8_t stencil);
        uint8_t getStencil(int x, int y) const;
        void clear(uint8_t stencil = 0);

    private:
        int width;
        int height;
        std::vector<uint8_t> data;
    };
    class Framebuffer {
    public:
        Framebuffer() = default;
        Framebuffer(int width, int height) :m_width(width), m_height(height) {
            m_sample_count = 4;
            m_color_buffer.resize(width * height * 4, 0);
            m_depth_buffer.resize(width * height, 1);
            m_multi_color_buffer.resize(width * height * 4 * m_sample_count, 0);
            m_multi_depth_buffer.resize(width * height * m_sample_count, 1);
        }
        void initialize(int width, int height) {
            m_sample_count = 4;
            m_color_buffer.assign(width * height * 4, 0);
            m_depth_buffer.assign(width * height, 1);
            m_multi_color_buffer.resize(width * height * 4 * m_sample_count, 0);
            m_multi_depth_buffer.resize(width * height * m_sample_count, 1);
            m_width = width;
            m_height = height;
        }
        void attachDepthBuffer(std::vector<float>& depth_buffer) {
            m_depth_buffer.assign(depth_buffer.begin(), depth_buffer.end());
        }
        std::shared_ptr<Texture2D> getColorTexture();

        void resolve();


        //void write_color(unsigned x, unsigned y, unsigned sample_id, Color& color);
        void write_color(unsigned x, unsigned y);
        void write_color(unsigned x, unsigned y, float depth);
        void write_color(unsigned x, unsigned y, float depth, FragmentData& frag);
        void write_color(unsigned x, unsigned y, float depth, const Color& color);

    private:
        //std::unordered_map<AttachmentPoint, std::shared_ptr<IAttachment>> attachments;

    public:
        int m_height;
        int m_width;
        int m_sample_count;
        std::vector<uint32_t> m_color_buffer;
        std::vector<float> m_depth_buffer;

        std::vector<uint32_t> m_multi_color_buffer;
        std::vector<float> m_multi_depth_buffer;


       // std::shared_ptr<Texture2D> m_texture;
        vec3 m_color = vec3(100, 100, 100);
    };

  

}