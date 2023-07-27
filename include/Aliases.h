//
// Created by mkd on 7/27/23.
//

#ifndef AVARA_ENGINE_ALIASES_H
#define AVARA_ENGINE_ALIASES_H


#include <memory>


namespace ae {

	//class (AABB);
	class Box;
	class Buffer;
	class BulletDebugDrawer;
	class BulleyPhysicsSimulator;
	class Camera;
	class Capsule;
	class Color;
	class Cone;
	class CubeImage;
	class Cylinder;
	class Font;
	class Geometry;
	class GeometryElement;
	class HitTestResult;
	class Image;
	class InputManager;
	class Light;
	class Line;
	class Logger;
	class Material;
	class MaterialProperty;
	class MaterialPropertyContents;
	class Node;
	class OpenGLRenderer;
	class PhysicsBody;
	class PhysicsContact;
	class PhysicsShape;
	class PhysicsSimulator;
	class PhysicsWorld;
	class Plane;
	class Point;
	class Program;
	class Pyramid;
	class RenderContext;
	class Renderer;
	class Scene;
	class SkyboxGeometry;
	class SkyboxGeometryElement;
	class SkyboxMaterial;
	class Sphere;
	class Torus;
	class Tube;
	class Window;
	class WindowInputManager;
	

//	 using (AABB)UPtr = std::unique_ptr<ae::AABB>;
//	 using (AABB)SPtr = std::shared_ptr<ae::AABB>;

	 using BoxUPtr = std::unique_ptr<ae::Box>;
	 using BoxSPtr = std::shared_ptr<ae::Box>;

	 using BufferUPtr = std::unique_ptr<ae::Buffer>;
	 using BufferSPtr = std::shared_ptr<ae::Buffer>;

	 using BulletDebugDrawerUPtr = std::unique_ptr<ae::BulletDebugDrawer>;
	 using BulletDebugDrawerSPtr = std::shared_ptr<ae::BulletDebugDrawer>;

	 using BulleyPhysicsSimulatorUPtr = std::unique_ptr<ae::BulleyPhysicsSimulator>;
	 using BulleyPhysicsSimulatorSPtr = std::shared_ptr<ae::BulleyPhysicsSimulator>;

	 using CameraUPtr = std::unique_ptr<ae::Camera>;
	 using CameraSPtr = std::shared_ptr<ae::Camera>;

	 using CapsuleUPtr = std::unique_ptr<ae::Capsule>;
	 using CapsuleSPtr = std::shared_ptr<ae::Capsule>;

	 using ColorUPtr = std::unique_ptr<ae::Color>;
	 using ColorSPtr = std::shared_ptr<ae::Color>;

	 using ConeUPtr = std::unique_ptr<ae::Cone>;
	 using ConeSPtr = std::shared_ptr<ae::Cone>;

	 using CubeImageUPtr = std::unique_ptr<ae::CubeImage>;
	 using CubeImageSPtr = std::shared_ptr<ae::CubeImage>;

	 using CylinderUPtr = std::unique_ptr<ae::Cylinder>;
	 using CylinderSPtr = std::shared_ptr<ae::Cylinder>;

	 using FontUPtr = std::unique_ptr<ae::Font>;
	 using FontSPtr = std::shared_ptr<ae::Font>;

	 using GeometryUPtr = std::unique_ptr<ae::Geometry>;
	 using GeometrySPtr = std::shared_ptr<ae::Geometry>;

	 using GeometryElementUPtr = std::unique_ptr<ae::GeometryElement>;
	 using GeometryElementSPtr = std::shared_ptr<ae::GeometryElement>;

	 using HitTestResultUPtr = std::unique_ptr<ae::HitTestResult>;
	 using HitTestResultSPtr = std::shared_ptr<ae::HitTestResult>;

	 using ImageUPtr = std::unique_ptr<ae::Image>;
	 using ImageSPtr = std::shared_ptr<ae::Image>;

	 using InputManagerUPtr = std::unique_ptr<ae::InputManager>;
	 using InputManagerSPtr = std::shared_ptr<ae::InputManager>;

	 using LightUPtr = std::unique_ptr<ae::Light>;
	 using LightSPtr = std::shared_ptr<ae::Light>;

	 using LineUPtr = std::unique_ptr<ae::Line>;
	 using LineSPtr = std::shared_ptr<ae::Line>;

	 using LoggerUPtr = std::unique_ptr<ae::Logger>;
	 using LoggerSPtr = std::shared_ptr<ae::Logger>;

	 using MaterialUPtr = std::unique_ptr<Material>;
	 using MaterialSPtr = std::shared_ptr<Material>;

	 using MaterialPropertyUPtr = std::unique_ptr<ae::MaterialProperty>;
	 using MaterialPropertySPtr = std::shared_ptr<ae::MaterialProperty>;

	 using MaterialPropertyContentsUPtr = std::unique_ptr<ae::MaterialPropertyContents>;
	 using MaterialPropertyContentsSPtr = std::shared_ptr<ae::MaterialPropertyContents>;

	 using NodeUPtr = std::unique_ptr<ae::Node>;
	 using NodeSPtr = std::shared_ptr<ae::Node>;

	 using OpenGLRendererUPtr = std::unique_ptr<ae::OpenGLRenderer>;
	 using OpenGLRendererSPtr = std::shared_ptr<ae::OpenGLRenderer>;

	 using PhysicsBodyUPtr = std::unique_ptr<ae::PhysicsBody>;
	 using PhysicsBodySPtr = std::shared_ptr<ae::PhysicsBody>;

	 using PhysicsContactUPtr = std::unique_ptr<ae::PhysicsContact>;
	 using PhysicsContactSPtr = std::shared_ptr<ae::PhysicsContact>;

	 using PhysicsShapeUPtr = std::unique_ptr<ae::PhysicsShape>;
	 using PhysicsShapeSPtr = std::shared_ptr<ae::PhysicsShape>;

	 using PhysicsSimulatorUPtr = std::unique_ptr<ae::PhysicsSimulator>;
	 using PhysicsSimulatorSPtr = std::shared_ptr<ae::PhysicsSimulator>;

	 using PhysicsWorldUPtr = std::unique_ptr<ae::PhysicsWorld>;
	 using PhysicsWorldSPtr = std::shared_ptr<ae::PhysicsWorld>;

	 using PlaneUPtr = std::unique_ptr<ae::Plane>;
	 using PlaneSPtr = std::shared_ptr<ae::Plane>;

	 using PointUPtr = std::unique_ptr<ae::Point>;
	 using PointSPtr = std::shared_ptr<ae::Point>;

	 using ProgramUPtr = std::unique_ptr<ae::Program>;
	 using ProgramSPtr = std::shared_ptr<ae::Program>;

	 using PyramidUPtr = std::unique_ptr<ae::Pyramid>;
	 using PyramidSPtr = std::shared_ptr<ae::Pyramid>;

	 using RenderContextUPtr = std::unique_ptr<ae::RenderContext>;
	 using RenderContextSPtr = std::shared_ptr<ae::RenderContext>;

	 using RendererUPtr = std::unique_ptr<ae::Renderer>;
	 using RendererSPtr = std::shared_ptr<ae::Renderer>;

	 using SceneUPtr = std::unique_ptr<ae::Scene>;
	 using SceneSPtr = std::shared_ptr<ae::Scene>;

	 using SkyboxGeometryUPtr = std::unique_ptr<ae::SkyboxGeometry>;
	 using SkyboxGeometrySPtr = std::shared_ptr<ae::SkyboxGeometry>;

	 using SkyboxGeometryElementUPtr = std::unique_ptr<ae::SkyboxGeometryElement;
	 using SkyboxGeometryElementSPtr = std::shared_ptr<ae::SkyboxGeometryElement>;

	 using SkyboxMaterialUPtr = std::unique_ptr<ae::SkyboxMaterial>;
	 using SkyboxMaterialSPtr = std::shared_ptr<ae::SkyboxMaterial>;

	 using SphereUPtr = std::unique_ptr<ae::Sphere>;
	 using SphereSPtr = std::shared_ptr<ae::Sphere>;

	 using TorusUPtr = std::unique_ptr<ae::Torus>;
	 using TorusSPtr = std::shared_ptr<ae::Torus>;

	 using TubeUPtr = std::unique_ptr<ae::Tube>;
	 using TubeSPtr = std::shared_ptr<ae::Tube>;

	 using WindowUPtr = std::unique_ptr<ae::Window>;
	 using WindowSPtr = std::shared_ptr<ae::Window>;

	 using WindowInputManagerUPtr = std::unique_ptr<ae::WindowInputManager>;
	 using WindowInputManagerSPtr = std::shared_ptr<ae::WindowInputManager>;
}


#endif //AVARA_ENGINE_ALIASES_H
