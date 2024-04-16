//
//  Renderer.cc
//	avara3d
//
//  Created by Morgan Davis on 4/22/18.
//  Copyright © 2024 Morgan K Davis. All rights reserved.
//

#include "a3d/rendering/Renderer.h"

#include "a3d/diagnostic/logging/Logger.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/rendering/VisualWorld.h"
#include "a3d/rendering/context/RenderContext.h"
#include "a3d/rendering/material/Material.h"
#include "a3d/scene/Scene.h"


using namespace a3d;
using namespace std;


/*********************************************************************************************
	Lifescycle
 *********************************************************************************************/

Renderer::Renderer() { }

Renderer::~Renderer() {
	A3D_LOG_D("Destroying Renderer {:p}", static_cast<void*>(this));
}
