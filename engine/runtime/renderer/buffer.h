#pragma once
#include<renderer/rhi_struct.h>
namespace CGL {
    template <typename T>
    class Buffer {
    public:
        Buffer(const std::vector<T>& data) : _data(data) {}
        //virtual void bind(RenderContext& context) const = 0;
        //virtual void unbind(RenderContext& context) const = 0;
        const T& operator[](size_t index) const { return _data[index]; }
        T& operator[](size_t index) { return _data[index]; }
        size_t size() const { return _data.size(); }
        const std::vector<T>& getData() const { return _data; }

    protected:
        std::vector<T> _data;
    };

    class VertexBuffer : public Buffer<VertexData> {
    public:
       // using Buffer<Vertex>::Buffer; // Inherit constructor
       // void bind(RenderContext& context) const override;
       // void unbind(RenderContext& context) const override;
    };

    class IndexBuffer : public Buffer<unsigned int> {
    public:
      //  using Buffer<unsigned int>::Buffer; // Inherit constructor
       // void bind(RenderContext& context) const override;
       // void unbind(RenderContext& context) const override;
    };



}