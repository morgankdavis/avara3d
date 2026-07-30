//
//  InputContext.h
//  avara3d
//
//  Created by Morgan Davis on 10/9/17.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#ifndef AVARA3D_INPUT_INPUTCONTEXT_H
#define AVARA3D_INPUT_INPUTCONTEXT_H

#include <cstdint>
#include <functional>

namespace a3d {

	class RenderContext;
	class Scene;

	class InputContext {

	public:
		/// Public Types ///

		struct UpdateInfo {

			// index of the Runner update containing this input update
			std::uint64_t updateIndex{0};

			// monotonic Runner elapsed time for the containing update
			double elapsedTime{0.0};

			// monotonic delta for the containing Runner update
			double deltaTime{0.0};
		};

		using DidUpdateCallback = std::function<void(InputContext& inputContext,
		                                             const UpdateInfo& info)>;

		/// Public Lifecycle Functions ///

		InputContext();

		InputContext(const InputContext&) = delete;
		InputContext& operator=(const InputContext&) = delete;

		InputContext(InputContext&&) = delete;
		InputContext& operator=(InputContext&&) = delete;

		virtual ~InputContext();

		/// Public Member Functions ///

		DidUpdateCallback	didUpdateCallback() const;
		void				didUpdateCallback(DidUpdateCallback callback);

		/// Internal Member Functions ///

		virtual void		update() = 0;
		virtual void		attachedToScene(Scene& scene) = 0;
		virtual void		visualWorldAttachedToScene(Scene& scene) = 0;

	private:
		/// Private Member Variables ///

		DidUpdateCallback	_didUpdateCallback;
	};
}

#endif /* AVARA3D_INPUT_INPUTCONTEXT_H */
