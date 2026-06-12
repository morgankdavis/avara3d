## References

- `README.md` contains the project intro and build instructions
- [ROADMAP.txt](https://gitlab.mkd.net/a3d/avara3d/-/snippets/90/raw/master/ROADMAP.txt) contains a project plan and current priorities

## Working style

- Before making architectural changes, inspect relevant headers, source files, and call sites
- Preserve existing behavior unless the task explicitly says to change it
- OpenGL work should move toward a minimal working implementation while preserving architectural seams that make a future Vulkan backend realistic
- Do not perform broad rewrites unless explicitly asked
- Prefer staged plans with small, reviewable diffs
- Prefer fixing the smallest relevant scope rather than opportunistic cleanup
- If a task reveals unrelated cleanup, note it separately instead of doing it immediately
- Do not reformat unrelated files
- Do not touch `external/` unless explicitly asked
- Do not add new dependencies unless asked to
- Match existing code style when making changes
- When uncertain, state assumptions before editing

## Architecture preferences

- Run on Linux (Wayland), macOS and Windows
- Public API should feel RealityKit-ish: `World`, `Entity`, components, resources
- Internals may be ECS-ish, but internal handles/registries must not leak into public API
- Never expose internal dependencies in the public API (example: anything in `external/`)
- Rendering should consume extracted snapshots/handles, not live scene objects
- Use as platform-agnostic C++ as possible
- Separate simulation/update concerns from rendering/extraction concerns
- Favor readability and maintainability over raw performance
- Favor simple C++20 over template-heavy or otherwise fancy language features
- When in doubt, copy RealityKit

## Build and SCM expectations

- Use CMake, Ninja and clang
- It's okay to temporarily break editor/tests/samples/demos, but they should be fixed before merging topic branches
