//
//  ALFImporter.cc
//  avara3d
//
//  Created by Morgan Davis on 3/21/26.
//  Copyright © 2026 Morgan K Davis. All rights reserved.
//

#include "a3d/app/ALFImporter.h"

#include <iostream>
#include <string>
#include <utility>

#include <pugixml/pugixml.hpp>

#include "a3d/Color.h"
#include "a3d/log/Log.h"
#include "a3d/mesh/Mesh.h"
#include "a3d/mesh/MeshElement.h"
#include "a3d/mesh/primitive/Box.h"
#include "a3d/scene/Node.h"
#include "a3d/scene/Scene.h"
#include "a3d/scene/importer/GlTFImporter.h"
#include "a3d/util/filesystem.h"
#include "a3d/visual/VisualWorld.h"
#include "a3d/visual/light/AmbientLight.h"
#include "a3d/visual/light/DirectionalLight.h"
#include "a3d/visual/material/Material.h"

using namespace a3d;
using namespace a3d::math;
using namespace std;

/// Internal Lifecycle Functions ///

ALFImporter::ALFImporter(const filesystem::path& path):
		_path{path} {}

/// Internal Member Functions ///

unique_ptr<Scene> ALFImporter::scene(VisualWorld& visualWorld) {

	//auto mapPath = util::filesystem::AuxFilePath("Icebox", "alf");
	auto mapPath = _path;
//	if (!mapPath.has_value()) {
//		log::app::e()("Could not find map file.");
//		return nullptr;
//	}

	pugi::xml_document doc;

	pugi::xml_parse_result ok = doc.load_file(mapPath.c_str());
	//pugi::xml_parse_result ok = doc.load_file("Icebox.alf");

	if (!ok) {
		std::cerr << "XML parse failed: " << ok.description() << "\n";
		return nullptr;
	}

	auto scene = make_unique<Scene>();

	auto ambientLight = make_shared<AmbientLight>(Color::Gray());
	auto ambientLightNode = Node::LightNode(ambientLight);
	scene->rootNode()->addChild(ambientLightNode);

	auto directionalLight1 = make_shared<DirectionalLight>(Color::DarkGray()); // shining left->right
	auto directionalLight1Node = Node::LightNode(directionalLight1);
	directionalLight1Node->eulerAngles({math::radians(-30), math::radians(-90), 0});
	scene->rootNode()->addChild(directionalLight1Node);

	auto directionalLight2 = make_shared<DirectionalLight>(Color::DarkGray()); // shining right->left
	auto directionalLight2Node = Node::LightNode(directionalLight2);
	directionalLight2Node->eulerAngles({math::radians(-30), math::radians(90), 0});
	scene->rootNode()->addChild(directionalLight2Node);

	pugi::xml_node map = doc.child("map");
	for (pugi::xml_node node: map.children()) {
		std::string tag = node.name();

		// <SkyColor color="#0080ff" color.1="#0048aa" />
		// <GroundColor color="#010e68" />

		if (tag == "set") {

			auto mapName = node.attribute("information").as_string();
			scene->name(mapName);
		}
		else if (tag == "SkyColor") {
			auto color = make_shared<Color>(node.attribute("color").as_string());
			auto color1 = make_shared<Color>(node.attribute("color.1").as_string());

			visualWorld.background(color);
		}
		else if (tag == "GroundColor") {
			auto color = make_shared<Color>(node.attribute("color").as_string());

			const float PLANE_LENGTH = 500.0;
			const float PLANE_WIDTH = 500.0;
			auto planeNode = make_shared<Node>("Ground plane node");
			planeNode->mesh(Box::Mesh(PLANE_LENGTH, 0, PLANE_WIDTH));
			shared_ptr<Material> planeMaterial = Material::EmissionMaterial(color);

			planeMaterial->doubleSided(false);
			planeNode->mesh()->addMaterial(planeMaterial);
			planeNode->position({planeNode->position().x, 0, planeNode->position().z});

			scene->rootNode()->addChild(planeNode);
		}
		else if (tag == "Wall") {
			float x = node.attribute("x").as_float();
			float z = node.attribute("z").as_float();
			float w = node.attribute("w").as_float();
			float d = node.attribute("d").as_float();
			float h = node.attribute("h").as_float();
			float y = node.attribute("y").as_float(0.0f);
			auto color = make_shared<Color>(node.attribute("color").as_string());
			auto color1 = make_shared<Color>(node.attribute("color.1").as_string());

			log::d()("Wall: x: {}, z: {}, w: {}, d: {}, h: {}, y: {}",
					x, z, w, d, h, y);

			// center = (x, y + h * 0.5f, z)
			// size   = (w, h, d)

			auto node = Node::MeshNode(Box::Mesh(w, h, d));
			node->position({x, y + h * 0.5f, z});

			auto material = Material::DiffuseMaterial(color);
			auto material1 = Material::DiffuseMaterial(color1);

			node->mesh()->addMaterial(material);
			node->mesh()->addMaterial(material1);

			scene->rootNode()->addChild(node);
		}
		else if (tag == "FreeSolid") {
			std::string shape = node.attribute("shape").as_string();
			float cx = node.attribute("cx").as_float(); // center x?
			float cz = node.attribute("cz").as_float(); // center z?
			float y = node.attribute("y").as_float(0.0f);
			float angle = node.attribute("angle").as_float(0.0f); // rotation around Y in degrees?
			auto color = make_shared<Color>(node.attribute("color").as_string());
			auto color1 = make_shared<Color>(node.attribute("color.1").as_string());

			// shotPower="10" customGravity="0.2" color="#a8d6ff" color.1="#000000"

			log::d()("FreeSolid: shape: {}, cx: {}, cz: {}, y: {}, angle: {}",
					 shape, cx, cz, y, angle);
//			log::d()("FreeSolid: shape: {}, cx: {}, cz: {}, y: {}, angle: {}, color: {}, color1: {}",
//					shape, cx, cz, y, angle, color.u8rgb(), color1.u8rgb());

			auto meshSubpath = filesystem::path(shape) / filesystem::path(shape + "");
			auto meshPath = util::filesystem::AuxiliaryFilePath(meshSubpath.string(), "gltf");
			static auto mesh = GlTFImporter(*meshPath, Scene::ImportOptions::ImportMeshes).firstMesh();

			auto node = Node::MeshNode(mesh);

			node->position({cx, y, cz});

			// ************
			// in CAD (maybe DXF?) Z is usually up. it's also up in Blender.
			// in glTF, Y is usually up.
			// so in the future, first rotate +=90 degrees on X (donno on sign yet), then apply
			// "angle" from the XML (around Y? donno!)
			// ************

			//node->rotation({0, 1, 0}, radians(angle));
			node->rotation({1, 0, 0}, radians(90));

			auto material = Material::DiffuseMaterial(color);
			auto material1 = Material::DiffuseMaterial(color1);

			node->mesh()->removeMaterial(0); // remove default material
			node->mesh()->addMaterial(material);
			//node->mesh()->addMaterial(material1);

			scene->rootNode()->addChild(node);
		}
	}

	return std::move(scene);
}
