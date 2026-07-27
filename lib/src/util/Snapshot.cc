//
//  Snapshot.cc
//  avara3d
//
//  Created by Morgan Davis on 1/1/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/util/Snapshot.h"

#include <filesystem>

#include "a3d/Image.h"
#include "a3d/render/context/RenderContext.h"
#include "a3d/log/Log.h"
#include "a3d/util/Filesystem.h"
#include "a3d/util/String.h"

void a3d::util::snapshot::SaveSnapshot(RenderContext& context) {

	auto execDir = a3d::util::filesystem::ExecutableDirectory();
	if (execDir) {
		auto filename = std::format("Snapshot_{}.png", a3d::util::string::DateTimeString());
		log::i()("Saving snapshot to '{}'",
				(*execDir/std::filesystem::path(filename)).string());
		auto image = context.snapshot();
		auto fullPath = *execDir / filename;
		image->writePNG(fullPath);
	}
	else {
		log::e()("Failed to save snapshot.  Couldn't locate executable directory.");
	}
}

void a3d::util::snapshot::StartGIFRecording(RenderContext& context,
								   a3d::math::uvec2 fitInside,
								   unsigned maxFramerate) {

	auto execDir = a3d::util::filesystem::ExecutableDirectory();
	if (execDir) {
		auto filename = std::format("Recording_{}.gif", a3d::util::string::DateTimeString());
		log::i()("Starting GIF recording at '{}'",
				(*execDir/std::filesystem::path(filename)).string());
		auto fullPath = *execDir / filename;
		context.startGIFRecording(fullPath.string(), fitInside, maxFramerate);
	}
	else {
		log::w()("Couldn't locate executable directory.");
	}
}

void a3d::util::snapshot::StopGIFRecording(RenderContext& context) {

	context.stopGIFRecording();
}
