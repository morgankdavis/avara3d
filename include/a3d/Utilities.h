//
//  Utilities.h
//  avara3d
//
//  Created by Morgan Davis on 12/23/16.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_UTILITIES_H
#define AVARA3D_UTILITIES_H

#include <chrono>
#include <filesystem>
#include <fstream>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include "a3d/scene/Scene.h"

struct GLFWmonitor;

namespace a3d {
	class Buffer;
	class Color;
	class CubeImage;
	class Font;
	class Image;
	class Mesh;
	class Node;
	class RenderContext;
	class Scene;
}

// TODO: put these somewhere else

#define A3D_PP_CAT2(a,b) a##b
#define A3D_PP_CAT(a,b)  A3D_PP_CAT2(a,b)

// run only on first invocation per call site
#define A3D_ONCE(...) A3D_ONCE_IMPL(__COUNTER__, __VA_ARGS__)
#define A3D_ONCE_IMPL(ctr, ...) \
do { \
  static std::once_flag A3D_PP_CAT(_a3d_once_flag_, ctr); \
  std::call_once(A3D_PP_CAT(_a3d_once_flag_, ctr), (__VA_ARGS__)); \
} while (0)

//namespace a3d::utils::detail {
//
//	template<class Clock, class Rep, class Period>
//	inline bool every_tick(typename Clock::time_point &last,
//						   std::chrono::duration<Rep, Period> interval) {
//		const auto now = Clock::now();
//		if (now - last >= interval) {
//			last = now;          // "at most once" behavior (no catch-up)
//			return true;
//		}
//		return false;
//	}
//}
//
//// run at most every 'interval' (duration)
//#define A3D_EVERY(interval_expr)                                                 \
//			if ([&]() -> bool {                                                          \
//					struct State {                                                       \
//						std::chrono::steady_clock::time_point last;                      \
//						std::chrono::steady_clock::duration interval;                    \
//						explicit State(std::chrono::steady_clock::duration i)            \
//							: last(std::chrono::steady_clock::now() - i), interval(i) {} \
//					};                                                                    \
//					static State s{ (interval_expr) };                                   \
//					return a3d::utils::detail::every_tick<std::chrono::steady_clock>( \
//						s.last, s.interval);                                             \
//				}())
//
//// run exactly once on 'invocation'th call, not before or after
//#define A3D_ON(invocation_expr)                                                  \
//			if ([&]() -> bool {                                                          \
//					struct State {                                                       \
//						long long count = 0;                                             \
//						long long target;                                                \
//						bool done = false;                                               \
//						explicit State(long long t) : target(t < 1 ? 1 : t) {}           \
//					};                                                                    \
//					static State s{ static_cast<long long>(invocation_expr) };           \
//					if (s.done) return false;                                            \
//					if (++s.count == s.target) { s.done = true; return true; }           \
//					return false;                                                        \
//				}())
//
//// skip the first 'invocations' calls, the run each thereafter
//#define A3D_AFTER(invocations_expr)                                              \
//			if ([&]() -> bool {                                                          \
//					struct State {                                                       \
//						long long count = 0;                                             \
//						long long target;                                                \
//						explicit State(long long t) : target(t) {}                       \
//					};                                                                    \
//					static State s{ static_cast<long long>(invocations_expr) };          \
//					return (s.count++ >= s.target);                                      \
//				}())




namespace a3d::utils::detail {

	template<class Clock, class Rep, class Period>
	inline bool every_tick(typename Clock::time_point& last,
						   std::chrono::duration<Rep, Period> interval) {
		const auto now = Clock::now();
		if (now - last >= interval) {
			last = now; // "at most once" behavior (no catch-up)
			return true;
		}
		return false;
	}

} // namespace a3d::utils::detail

// run at most every 'interval' (duration)
#define A3D_EVERY(interval_expr, ...)                                           \
    do {                                                                        \
        auto&& _a3d_fn = (__VA_ARGS__);                                         \
        if ([&]() -> bool {                                                     \
                struct State {                                                  \
                    std::chrono::steady_clock::time_point last;                 \
                    std::chrono::steady_clock::duration interval;               \
                    explicit State(std::chrono::steady_clock::duration i)       \
                        : last(std::chrono::steady_clock::now() - i),           \
                          interval(i) {}                                        \
                };                                                              \
                static State s{ (interval_expr) };                              \
                return a3d::utils::detail::every_tick<std::chrono::steady_clock>( \
                    s.last, s.interval);                                        \
            }()) {                                                              \
            _a3d_fn();                                                          \
        }                                                                       \
    } while (0)

// run exactly once on 'invocation'th call, not before or after
#define A3D_ON(invocation_expr, ...)                                            \
    do {                                                                        \
        auto&& _a3d_fn = (__VA_ARGS__);                                         \
        if ([&]() -> bool {                                                     \
                struct State {                                                  \
                    long long count = 0;                                        \
                    long long target;                                           \
                    bool done = false;                                          \
                    explicit State(long long t) : target(t < 1 ? 1 : t) {}      \
                };                                                              \
                static State s{ static_cast<long long>(invocation_expr) };      \
                if (s.done) return false;                                       \
                if (++s.count == s.target) { s.done = true; return true; }      \
                return false;                                                   \
            }()) {                                                              \
            _a3d_fn();                                                          \
        }                                                                       \
    } while (0)

// skip the first 'invocations' calls, then run each thereafter
#define A3D_AFTER(invocations_expr, ...)                                        \
    do {                                                                        \
        auto&& _a3d_fn = (__VA_ARGS__);                                         \
        if ([&]() -> bool {                                                     \
                struct State {                                                  \
                    long long count = 0;                                        \
                    long long target;                                           \
                    explicit State(long long t) : target(t) {}                  \
                };                                                              \
                static State s{ static_cast<long long>(invocations_expr) };     \
                return (s.count++ >= s.target);                                 \
            }()) {                                                              \
            _a3d_fn();                                                          \
        }                                                                       \
    } while (0)





// runs 'on_edge()' only on false->true transitions of 'cond'
// runs 'fail_stmt' every time 'cond' is true (e.g. return/continue/break/throw)
// resets when 'cond' becomes false again
#define A3D_EDGE_GUARD(cond, fail_stmt, ...) \
    A3D_EDGE_GUARD_IMPL((cond), fail_stmt, __COUNTER__, __VA_ARGS__)

#define A3D_EDGE_GUARD_IMPL(cond, fail_stmt, ctr, ...) \
do { \
  static bool A3D_PP_CAT(_a3d_latched_, ctr) = false; \
  const bool _a3d_bad = !!(cond); \
  if (_a3d_bad) { \
    if (!A3D_PP_CAT(_a3d_latched_, ctr)) { \
      A3D_PP_CAT(_a3d_latched_, ctr) = true; \
      auto&& _a3d_edge_fn = (__VA_ARGS__); \
      _a3d_edge_fn(); \
    } \
    fail_stmt; \
  } else { \
    A3D_PP_CAT(_a3d_latched_, ctr) = false; \
  } \
} while (0)

namespace a3d::utils {

	// TODO: move

	/// Chronology ///

	namespace chrono {

		double Time(); // TODO: CHANGE THIS?

		std::chrono::milliseconds milliseconds(std::chrono::seconds sec);
		std::chrono::milliseconds milliseconds(std::chrono::nanoseconds ns);
		std::chrono::milliseconds sec_f_to_ms(float secF);
		float ns_to_ms_f(std::chrono::nanoseconds ns);
		int ns_to_ms_i(std::chrono::nanoseconds ns);
	}

	/// Output ///

	std::string StringFromTree(const Node& root);
	std::string DateTimeString();

#ifdef A3D_POSIX
	std::string StackTrace(unsigned dropFunctions = 0);
#endif

	/// String ///

	void 						Replace(std::string& str,
										const std::string& oldStr,
										const std::string& newStr);
	std::vector<std::string>	Split(const std::string& s,
									  std::string delim);

	/// Filesystem ///

	// *** executable and working directories ***

#ifndef ANDROID
	std::optional<std::filesystem::path> 	ExecutablePath();
	std::optional<std::filesystem::path> 	ExecutableDirectory();
	std::optional<std::string> 				ExecutableName();
	std::optional<std::filesystem::path>	CurrentWorkingDirectory();
#endif

	// *** search paths ***

#ifndef ANDROID
	std::vector<std::filesystem::path> 		BaseSearchPaths();
	std::vector<std::filesystem::path> 		ShaderSearchPaths();
	std::vector<std::filesystem::path> 		SceneSearchPaths();
	std::vector<std::filesystem::path> 		ModelSearchPaths();
	std::vector<std::filesystem::path> 		ImageSearchPaths();
	std::vector<std::filesystem::path> 		FontSearchPaths();
	std::optional<std::filesystem::path>	SearchInPaths(const std::string& filename,
														  std::vector<std::filesystem::path> paths);
#endif

	// *** binary and text files ***

#ifdef ANDROID
	std::optional<std::filesystem::path> InternalFilesDirectory();
	std::optional<std::string> TextAsset(const std::string& relPath);
	std::shared_ptr<Buffer> BinaryAsset(const std::string& relPath);
#else
	std::optional<std::string> 		TextFile(const std::filesystem::path& path);
#endif

	// *** shaders ***

	std::optional<std::string> 		ShaderSource(const std::string& name,
												   const std::string& type);

	// *** fonts ***

//		std::unique_ptr<a3d::Font> 		FontNamed(const std::string& filename);
	std::unique_ptr<a3d::Font> 		FontNamed(const std::string& name,
												const std::string& type);

	// ***  images ***

	std::unique_ptr<a3d::Image> 	ImageNamed(const std::string& name,
											  bool flipHorizontal=true,
											  bool flipVertical=false);
	std::unique_ptr<a3d::Image> 	ImageNamed(const std::string& name,
											  const std::string& type,
											  bool flipHorizontal=true,
											  bool flipVertical=false);
	std::unique_ptr<a3d::CubeImage> CubeImageNamed(const std::string& name);
	std::unique_ptr<a3d::CubeImage>	CubeImageNamed(const std::string& name,
													  const std::string& type);

	// *** scenes ***

#ifndef ANDROID
	std::unique_ptr<a3d::Scene> 	SceneNamed(const std::string& name,
											  SceneImportOptions options =
											  SceneImportOptions::ImportAll);
	std::unique_ptr<a3d::Scene> 	SceneNamed(const std::string& name,
											  const std::string& type,
											  SceneImportOptions options =
											  SceneImportOptions::ImportAll);
	std::shared_ptr<a3d::Mesh> 		MeshNamed(const std::string &name,
												MeshImportOptions options =
												MeshImportOptions::ImportMaterials);
	std::shared_ptr<a3d::Mesh> 		MeshNamed(const std::string &name,
												const std::string &type,
												MeshImportOptions options =
												MeshImportOptions::ImportMaterials);
#endif

	/// Misc ///

	void 							SaveSnapshot(RenderContext& context);
	void 							StartGIFRecording(RenderContext& context,
													  math::uvec2 fitInside,
													  unsigned maxFramerate);
	void 							StopGIFRecording(RenderContext& context);
}

#endif /* AVARA3D_UTILITIES_H */
