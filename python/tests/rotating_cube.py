import math

import avara3d as a3d


class App(a3d.Application):
    def init(self):
        self.window = a3d.Window(
            a3d.UVec2(800, 600),
            False,
            True,
            a3d.Antialiasing.Msaa4X,
        )

        visual_world = a3d.VisualWorld(self.window)
        scene = a3d.Scene(visual_world)

        material = a3d.Material.diffuseMaterial(a3d.Color.blue())
        mesh = a3d.Box.mesh(1.5, 1.5, 1.5, material=material)

        self.cube = a3d.Node.meshNode(mesh)
        self.cube.eulerAngles = a3d.Vec3(
            math.radians(-20.0),
            math.radians(30.0),
            0.0,
        )
        scene.rootNode.addChild(self.cube)

        ambient = a3d.AmbientLight(a3d.Color(0.1, 0.1, 0.1))
        scene.rootNode.addChild(a3d.Node.lightNode(ambient))

        point = a3d.PointLight(a3d.Color(1.0, 1.0, 1.0))
        point_node = a3d.Node.lightNode(point)
        point_node.position = a3d.Vec3(2.0, 2.0, 4.5)
        scene.rootNode.addChild(point_node)

        self.window.open()
        return scene

    def shouldContinue(self, scene):
        return self.window.isOpen()


raise SystemExit(a3d.run(App()))
