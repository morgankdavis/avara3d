//
//  OGLMemoryTracker.h
//  avara3d
//

#ifndef AVARA3D_RENDER_BACKEND_OPENGL_OGLMEMORYTRACKER_H
#define AVARA3D_RENDER_BACKEND_OPENGL_OGLMEMORYTRACKER_H

#include <cstdint>
#include <string>
#include <unordered_map>

#include "a3d/profile/FrameStats.h"
#include "a3d/render/backend/opengl/GLTypes.h"

namespace a3d {
class OGLMemoryTracker {

public:
    // [Internal Types]

    enum class ObjectNamespace : std::uint8_t {
        Buffer,
        Texture,
        Renderbuffer, // unused
        Synthetic,
    };

    enum class Source : std::uint8_t {
        A3D,
        ImGui,
    };

    enum class Category : std::uint8_t {
        Texture,
        VertexBuffer,
        IndexBuffer,
        UniformBuffer,
        RenderTarget,
        Other,
    };

    struct AllocationKey {
        ObjectNamespace type {};
        gl::uint_t      id {};

        friend bool     operator==(const AllocationKey&, const AllocationKey&) = default;
    };

    // [Internal Member Functions]

    void              setAllocation(AllocationKey key,
                                    Source        source,
                                    Category      category,
                                    std::uint64_t bytes,
                                    std::string   label = {});

    void              removeAllocation(AllocationKey key);
    void              clearSource(Source source);

    RenderMemoryStats stats() const;

private:
    // [Private Types]

    struct AllocationKeyHash {
        std::size_t operator()(const AllocationKey& key) const noexcept;
    };

    struct Entry {
        Source        source {};
        Category      category {};
        std::uint64_t bytes {};
        std::string   label {};
    };

    // [Private Member Functions]

    std::uint64_t                                               currentTotalBytes() const;

    // [Private Member Variables]

    std::unordered_map<AllocationKey, Entry, AllocationKeyHash> _allocations;
    std::uint64_t                                               _peakTotalBytes {0};
};
} // namespace a3d

#endif // AVARA3D_RENDER_BACKEND_OPENGL_OGLMEMORYTRACKER_H
