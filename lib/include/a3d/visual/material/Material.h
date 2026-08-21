//
//  Material.h
//  avara3d
//
//  Created by Morgan Davis on 10/21/16.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_VISUAL_MATERIAL_MATERIAL_H
#define AVARA3D_VISUAL_MATERIAL_MATERIAL_H

#include <climits>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "a3d/Color.h"
#include "a3d/Id.h"
#include "a3d/util/Bitmask.h"

namespace a3d {

    class Texture;

    /**
     * @brief Describes the surface appearance and render state of Mesh geometry.
     *
     * Ambient, diffuse, specular, and emission properties may be absent, a constant
     * Color, or a shared Texture. Copying a Material copies its values while retaining
     * shared ownership of any Texture objects referenced by its properties.
     */
    class Material {

    public:
        // [Public Types]

        /** @brief Material property contents: absent, a shared Texture, or a constant Color. */
        using Property = std::variant<std::monostate, std::shared_ptr<Texture>, Color>;

        /** @brief Identifies one of the material's lighting properties. */
        enum class PropertyType : uint8_t {
            Ambient  = 0, ///< Ambient reflectance.
            Diffuse  = 1, ///< Diffuse reflectance and base surface color.
            Specular = 2, ///< Specular reflectance.
            Emission = 3  ///< Self-emissive surface content.
        };

        /** @brief Selects polygon rasterization mode. */
        enum class FillMode {
            Fill,   ///< Rasterize filled polygons.
            Lines,  ///< Rasterize polygon edges.
            Points  ///< Rasterize polygon vertices as points.
        };

        /** @brief Selects how material alpha participates in render-pass selection. */
        enum class AlphaMode : uint8_t {
            /** @brief Selects the opaque render pass. */
            Opaque, // no discard, no blending
            /** @brief Selects the cutout/masked render pass. */
            Mask, // uses discard/alpha threshold
            /** @brief Selects the transparent blending render pass. */
            Blend
        }; // real transparency

        /** @brief Selects the blending function used for transparent materials. */
        enum class BlendFunction : uint8_t {
            Disabled,           ///< Do not explicitly enable blending.
            Alpha,              ///< Conventional source-alpha blending.
            Additive,           ///< Add source color to the destination.
            PremultipliedAlpha  ///< Alpha blending for premultiplied source color.
        };

        // [Public Static Member Functions]

        /**
         * @brief Returns the shared mutable fallback Material used for geometry without a usable material.
         *
         * The same Material instance is returned on each call, so modifying it changes
         * the fallback appearance for all users of the default material.
         */
        static std::shared_ptr<Material> DefaultMaterial();

        /** @brief Creates a Material containing only the supplied diffuse @p property. */
        static std::shared_ptr<Material> DiffuseMaterial(Property property);

        /** @brief Creates a Material containing only the supplied emission @p property. */
        static std::shared_ptr<Material> EmissionMaterial(Property property);

        // [Public Lifecycle Functions]

        /** @brief Creates a Material with no lighting properties and default render state. */
        Material();

        /** @brief Creates a Material with the supplied ambient, diffuse, and specular properties. */
        Material(const Property& ambient, const Property& diffuse, const Property& specular);

        /** @brief Creates a Material with the supplied lighting properties. */
        Material(const Property& ambient,
                 const Property& diffuse,
                 const Property& specular,
                 const Property& emission);

        Material(const Material& other);
        Material& operator=(const Material& other);

        Material(Material&& other) noexcept;
        Material& operator=(Material&& other) noexcept;

        ~Material();

        // [Public Member Functions]

        /** @brief Returns the optional material name. */
        const std::optional<std::string>& name() const;

        /** @brief Sets the material name. */
        void                              name(const std::string& name);

        /** @brief Returns the ambient material property. */
        const Property&                   ambient() const;

        /** @brief Sets the ambient material property. */
        void                              ambient(const Property& ambient);

        /** @brief Returns the diffuse material property. */
        const Property&                   diffuse() const;

        /** @brief Sets the diffuse material property. */
        void                              diffuse(const Property& diffuse);

        /** @brief Returns the specular material property. */
        const Property&                   specular() const;

        /** @brief Sets the specular material property. */
        void                              specular(const Property& specular);

        /** @brief Returns the self-emissive material property. */
        const Property&                   emission() const;

        /**
         * @brief Sets the self-emissive material property.
         *
         * When emission content is present, the standard material shader renders it
         * without evaluating Scene lights.
         */
        void                              emission(const Property& emission);

        /** @brief Returns the Phong specular exponent controlling highlight sharpness. */
        float                             specularExponent() const;

        /** @brief Sets the Phong specular exponent controlling highlight sharpness. */
        void                              specularExponent(float exponent);

        /** @brief Returns whether diffuse content replaces ambient content during lighting. */
        bool                              locksAmbientWithDiffuse() const;

        /** @brief Sets whether diffuse content replaces ambient content during lighting. */
        void                              locksAmbientWithDiffuse(bool flag);

        /** @brief Returns whether both sides of polygons are rendered. */
        bool                              doubleSided() const;

        /** @brief Sets whether both sides of polygons are rendered. */
        void                              doubleSided(bool flag);

        /** @brief Returns the polygon rasterization mode. */
        FillMode                          fillMode() const;

        /**
         * @brief Sets the polygon rasterization mode.
         *
         * Support for non-filled modes is rendering-backend dependent.
         */
        void                              fillMode(FillMode mode);

        /**
         * @brief Returns the uniform texture-coordinate scale.
         *
         * Texture coordinates are divided by this value; values greater than one
         * enlarge texture features and values below one increase repetition.
         */
        float                             uvScale() const;

        /**
         * @brief Sets the uniform texture-coordinate scale.
         *
         * @throws std::invalid_argument if @p scale is not greater than zero.
         */
        void                              uvScale(float scale);

        /** @brief Returns the alpha render mode. */
        AlphaMode                         alphaMode() const;

        /** @brief Sets the alpha render mode. */
        void                              alphaMode(AlphaMode mode);

        // ! NOT IMPLEMENTED !
        /** @brief Returns the stored alpha-mask cutoff; the renderer does not currently consume this value. */
        float                             alphaCutoff() const;

        /** @brief Sets the stored alpha-mask cutoff; the renderer does not currently consume this value. */
        void                              alphaCutoff(float v);

        /** @brief Returns the requested transparent blending function. */
        BlendFunction                     blendFunction() const;

        /**
         * @brief Sets the requested transparent blending function.
         *
         * Opaque and Mask materials render with blending disabled. A Blend material
         * with BlendFunction::Disabled uses conventional alpha blending by default.
         */
        void                              blendFunction(BlendFunction function);

        /** @brief Returns whether depth testing is enabled for the material. */
        bool                              depthTestEnabled() const;

        /** @brief Enables or disables depth testing for the material. */
        void                              depthTestEnabled(bool enabled);

        /** @brief Returns whether rendering the material writes to the depth buffer. */
        bool                              depthWriteEnabled() const;

        /** @brief Enables or disables depth-buffer writes for the material. */
        void                              depthWriteEnabled(bool enabled);

        // [Internal Types]

        using PropertyList = std::vector<std::pair<const Property*, PropertyType>>;

        enum class DirtyMask : uint32_t {
            None = 0,
            All  = UINT_MAX
        };

        // [Internal Static Member Functions]

        static std::shared_ptr<Material> MissingTextureMaterial();
        static Property                  MissingTextureProperty();

        // [Internal Member Functions]

        PropertyList                     properties() const;

        MaterialId                       id() const noexcept;

        DirtyMask                        dirtyMask() const;
        void                             dirtyMask(DirtyMask mask);

    private:
        // [Private Member Variables]

        MaterialId                 _id;
        std::optional<std::string> _name;
        Property                   _ambient;
        Property                   _diffuse;
        Property                   _specular;
        Property                   _emission;
        float                      _specularExponent;
        bool                       _locksAmbientWithDiffuse;
        bool                       _doubleSided;
        FillMode                   _fillMode;
        float                      _uvScale;
        AlphaMode                  _alphaMode;
        float                      _alphaCutoff;
        BlendFunction              _blendFunction;
        bool                       _depthTestEnabled;
        bool                       _depthWriteEnabled;
        DirtyMask                  _dirtyMask;
    };

    namespace util::bitmask {

        template<>
        struct enable_ops<Material::DirtyMask> : std::true_type {};

    }
}

#endif // AVARA3D_VISUAL_MATERIAL_MATERIAL_H
