//
//  OGLMemoryTracker.cc
//  avara3d
//

#include "a3d/render/backend/opengl/OGLMemoryTracker.h"

#include <functional>
#include <utility>

using namespace a3d;
using namespace std;

/// Internal Member Functions ///

void OGLMemoryTracker::setAllocation(AllocationKey key,
                                     Source        source,
                                     Category      category,
                                     uint64_t      bytes,
                                     string        label) {

    if (bytes == 0) {
        removeAllocation(key);
        return;
    }

    _allocations.insert_or_assign(key, Entry {
                                           .source = source,
                                           .category = category,
                                           .bytes = bytes,
                                           .label = std::move(label),
                                       });

    const uint64_t currentBytes = currentTotalBytes();

    if (currentBytes > _peakTotalBytes) {
        _peakTotalBytes = currentBytes;
    }
}

void OGLMemoryTracker::removeAllocation(AllocationKey key) {
    _allocations.erase(key);
}

void OGLMemoryTracker::clearSource(Source source) {

    for (auto it = _allocations.begin(); it != _allocations.end();) {
        if (it->second.source == source) {
            it = _allocations.erase(it);
        }
        else {
            ++it;
        }
    }
}

RenderMemoryStats OGLMemoryTracker::stats() const {

    RenderMemoryStats result {};

    auto addUsage = [](RenderMemoryStats::Usage& usage, Category category, uint64_t bytes) {
        switch (category) {
            case Category::Texture:
                usage.textureBytes += bytes;
                break;
            case Category::VertexBuffer:
                usage.vertexBufferBytes += bytes;
                break;
            case Category::IndexBuffer:
                usage.indexBufferBytes += bytes;
                break;
            case Category::UniformBuffer:
                usage.uniformBufferBytes += bytes;
                break;
            case Category::RenderTarget:
                usage.renderTargetBytes += bytes;
                break;
            case Category::Other:
                usage.otherBytes += bytes;
                break;
        }
    };

    for (const auto& [key, entry] : _allocations) {

        auto& usage = entry.source == Source::A3D ? result.a3d : result.imgui;
        addUsage(usage, entry.category, entry.bytes);
    }

    result.peakTotalBytes = _peakTotalBytes;

    return result;
}

/// Private Member Functions ///

uint64_t OGLMemoryTracker::currentTotalBytes() const {

    uint64_t total = 0;
    for (const auto& [key, entry] : _allocations) {
        total += entry.bytes;
    }
    return total;
}

size_t OGLMemoryTracker::AllocationKeyHash::operator()(const AllocationKey& key) const noexcept {

    const size_t typeHash = static_cast<size_t>(key.type) * 0x9e3779b9u;
    const size_t idHash = hash<gl::uint_t> {}(key.id);
    return typeHash ^ idHash;
}
