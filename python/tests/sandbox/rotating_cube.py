from avara3d import *
from avara3d.math import radians, uvec2, vec3


class App(Application):
    def init(self):
        self.window = Window(
            uvec2(800, 600),
            False,
            True,
            Antialiasing.Msaa4X,
        )

        self.angular_velocity = vec3(
            radians(20.0),
            radians(45.0),
            0.0,
        )

        visual_world = VisualWorld(self.window)
        scene = Scene(visual_world)

        material = Material.diffuse_material(Color.blue())
        mesh = Box.mesh(1.5, 1.5, 1.5, material=material)

        self.cube = Node.mesh_node(mesh)
        self.cube.euler_angles = vec3(
            radians(-20.0),
            radians(30.0),
            0.0,
        )
        scene.root_node.add_child(self.cube)

        ambient = AmbientLight(Color(0.1, 0.1, 0.1))
        scene.root_node.add_child(Node.light_node(ambient))

        point = PointLight(Color(1.0, 1.0, 1.0))
        point_node = Node.light_node(point)
        point_node.position = vec3(2.0, 2.0, 4.5)
        scene.root_node.add_child(point_node)

        self.window.open()
        return scene

    def should_continue(self, scene):
        return self.window.is_open()

    def frame_did_begin(self, runner, scene, visual_world, info):
        angles = self.cube.euler_angles

        angles.x += self.angular_velocity.x * info.update_delta_time
        angles.y += self.angular_velocity.y * info.update_delta_time
        angles.z += self.angular_velocity.z * info.update_delta_time

        self.cube.euler_angles = angles


raise SystemExit(run(App()))
