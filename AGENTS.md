## References

- `README.md` contains the project introduction and build instructions
- Treat the user’s explicit request as the source of truth for immediate priorities
- The legacy external roadmap is not authoritative

## Current project direction

* A3D is a cross-platform real-time engine for applied simulation
* The current showcase project is a quadrotor simulation running natively and in the browser
* Prioritize fixed-step simulation, physics correctness, control-system integration, telemetry, and clear visualization
* Game-oriented features are secondary unless explicitly requested

## Working style

- Before making architectural changes, inspect relevant headers, source files, and call sites
- Preserve existing behavior unless the task explicitly says to change it
- Do not perform broad rewrites unless explicitly asked
- Prefer staged plans with small, reviewable diffs
- Prefer fixing the smallest relevant scope rather than opportunistic cleanup
- If a task reveals unrelated cleanup, note it separately instead of doing it immediately
- Do not reformat unrelated files
- Do not touch `data/external/` unless explicitly asked
- Do not add new dependencies unless asked to
- Match existing code format/style when making changes
- Organize declarations using the existing /// Group Title /// pattern; place public engine-internal APIs in an appropriate /// Internal ... /// group
- Implement functions in the same order in which their declarations appear, and under the same section/group titles
- Use friend only for test access. Cross-class engine access must use public methods grouped as internal
- Avoid empty or 'detail' namespaces unless there is a really good reason to use one
- Prefer a3d::math functions over standard library functions
- OpenGL work should favor a minimal, correct, and maintainable implementation
- Do not introduce rendering-backend abstraction work unless explicitly requested
- When uncertain, state assumptions before editing

## Architecture preferences

- Run on Linux, macOS, Windows, and modern web browsers through WebAssembly
- Public API should feel RealityKit-ish: `World`, `Entity`, components, resources
- Internals may be ECS-ish, but internal handles/registries must not leak into public API
- Never expose internal dependencies in the public API (example: anything in `data/external/`)
- Rendering should consume extracted snapshots/handles, not live scene objects
- Use as platform-agnostic C++ as possible
- Separate simulation/update concerns from rendering/extraction concerns
- Favor readability and maintainability over raw performance
- Favor simple C++20 over template-heavy or otherwise fancy language features
- When in doubt, copy RealityKit

## Build and SCM expectations

- Use CMake, Ninja and clang
- It's okay to temporarily break editor/tests/demos, but they should be fixed before merging topic branches
