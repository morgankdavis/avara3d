//
//  GLTypes.h
//  avara3d
//
//  Created by Morgan Davis on 12/31/25.
//  Copyright © 2025 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_RENDER_BACKEND_OPENGL_GLTYPES_H
#define AVARA3D_RENDER_BACKEND_OPENGL_GLTYPES_H

#include <cstdint>
#include <cstddef>   // std::ptrdiff_t

namespace a3d::gl {

	using enum_t     = std::uint32_t;   // GLenum
	using uint_t     = std::uint32_t;   // GLuint
	using int_t      = std::int32_t;    // GLint
	using sizei_t    = std::int32_t;    // GLsizei
	using boolean_t  = std::uint8_t;    // GLboolean
	using bitfield_t = std::uint32_t;   // GLbitfield

	using sizeiptr_t = std::ptrdiff_t;  // GLsizeiptr
	using intptr_t   = std::ptrdiff_t;  // GLintptr

	using char_t     = char;            // GLchar
	using float_t    = float;           // GLfloat
	using double_t   = double;          // GLdouble

	using int64_t    = std::int64_t;    // GLint64
	using uint64_t   = std::uint64_t;   // GLuint64

	namespace value {
		inline constexpr enum_t false_ = 0u;
		inline constexpr enum_t true_  = 1u;

		inline constexpr enum_t unsigned_byte  = 0x1401u; // GL_UNSIGNED_BYTE
		inline constexpr enum_t unsigned_short = 0x1403u; // GL_UNSIGNED_SHORT
		inline constexpr enum_t unsigned_int   = 0x1405u; // GL_UNSIGNED_INT
		inline constexpr enum_t float32        = 0x1406u; // GL_FLOAT

		inline constexpr enum_t invalid_index  = 0xFFFFFFFFu; // GL_INVALID_INDEX
		inline constexpr uint_t null_handle    = 0u;          // “0” object name
	}

	enum class index_type : enum_t {
		u8  = value::unsigned_byte,
		u16 = value::unsigned_short,
		u32 = value::unsigned_int,
	};

	// helpers
	template <class E>
	constexpr enum_t raw(E e) noexcept {
		return static_cast<enum_t>(e);
	}

	constexpr boolean_t gl_bool(bool b) noexcept {
		return b ? static_cast<boolean_t>(value::true_)
				 : static_cast<boolean_t>(value::false_);
	}

	// strong-typed handles (zero-cost)
	template <class Tag>
	struct handle {
		uint_t id = value::null_handle;
		constexpr explicit operator bool() const noexcept { return id != value::null_handle; }
		friend constexpr bool operator==(handle, handle) = default;
	};

	struct buffer_tag {};
	struct texture_tag {};
	struct vao_tag {};
	struct program_tag {};
	struct shader_tag {};
	struct framebuffer_tag {};

	using buffer      = handle<buffer_tag>;
	using texture     = handle<texture_tag>;
	using vao         = handle<vao_tag>;
	using program     = handle<program_tag>;
	using shader      = handle<shader_tag>;
	using framebuffer = handle<framebuffer_tag>;
}

#endif // AVARA3D_RENDER_BACKEND_OPENGL_GLTYPES_H
