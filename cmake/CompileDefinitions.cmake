
function(a3d_set_compile_definitions target)

	if (A3D_WINDOWS)
		target_compile_definitions(${target} PUBLIC A3D_WINDOWS)
		target_compile_definitions(${target} PRIVATE NOMINMAX WIN32_LEAN_AND_MEAN)
	endif()

	if (A3D_WIN32)
		target_compile_definitions(${target} PUBLIC A3D_WIN32)
	endif()

	if (A3D_WIN64)
		target_compile_definitions(${target} PUBLIC A3D_WIN64)
	endif()

	if (A3D_LINUX)
		target_compile_definitions(${target} PUBLIC A3D_LINUX)
	endif()

	if (A3D_MACOS)
		target_compile_definitions(${target} PUBLIC A3D_MACOS)
	endif()

	if (A3D_IOS)
		target_compile_definitions(${target} PUBLIC A3D_IOS)
	endif()

	if (A3D_ANDROID)
		target_compile_definitions(${target} PUBLIC A3D_ANDROID)
	endif()

	if (A3D_DESKTOP)
		target_compile_definitions(${target} PUBLIC A3D_DESKTOP)
	endif()

	if (A3D_MOBILE)
		target_compile_definitions(${target} PUBLIC A3D_MOBILE)
	endif()

	if (A3D_WEB)
		target_compile_definitions(${target} PUBLIC A3D_WEB)
	endif()

	if (A3D_POSIX)
		target_compile_definitions(${target} PUBLIC A3D_POSIX)
	endif()

	if (A3D_GL_DESKTOP)
		target_compile_definitions(${target} PUBLIC A3D_GL_DESKTOP)
	endif()

	if (A3D_GL_ES)
		target_compile_definitions(${target} PUBLIC A3D_GL_ES)
	endif()

	if (A3D_GL_WEB)
		target_compile_definitions(${target} PUBLIC A3D_GL_WEB)
	endif()

endfunction()
