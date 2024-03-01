//
//  GameViewController.swift
//  04-primitivegeometries
//
//  Created by Morgan Davis on 10/31/17.
//  Copyright © 2017 Morgan Davis. All rights reserved.
//

import SceneKit
import QuartzCore

class GameViewController: NSViewController {
    
    override func viewDidLoad() {
        super.viewDidLoad()
		
        //let scene = SCNScene(named: "art.scnassets/ship.scn")!
		let scene = SCNScene()
		
//		let planeGeo = SCNPlane(width: 2.0, height: 1.0)
//		let planeNode = SCNNode()
//		planeNode.geometry = planeGeo
//		scene.rootNode.addChildNode(planeNode)
//		let planeMaterial = SCNMaterial()
//		planeMaterial.diffuse.contents = NSColor.red
//		planeGeo.firstMaterial = planeMaterial
		
		
//		let boxGeo = SCNBox(width: 1, height: 2, length: 3, chamferRadius: 0)
//		let boxNode = SCNNode()
//		boxNode.mesh = boxGeo
//		scene.rootNode.addChildNode(boxNode)
//		let boxMaterial = SCNMaterial()
//		boxMaterial.diffuse.contents = NSColor.red
//		boxGeo.firstMaterial = boxMaterial
		
		
//		let sphereGeo = SCNSphere(radius: 1.0)
//		let sphereNode = SCNNode()
//		sphereNode.mesh = sphereGeo
//		scene.rootNode.addChildNode(sphereNode)
//		let sphereMaterial = SCNMaterial()
//		sphereMaterial.diffuse.contents = NSColor.red
//		sphereGeo.firstMaterial = sphereMaterial


		

        
		let cameraNode = SCNNode()
		cameraNode.camera = SCNCamera()
		cameraNode.camera?.xFov = 30.0
		cameraNode.camera?.yFov = 30.0
		cameraNode.camera?.zNear = 0.01
		cameraNode.camera?.zFar = 1000.0
		scene.rootNode.addChildNode(cameraNode)
		cameraNode.position = SCNVector3(x: 0, y: 0, z: 10)
		
        
        // retrieve the SCNView
        let scnView = self.view as! SCNView
        scnView.scene = scene
        scnView.showsStatistics = true
        scnView.backgroundColor = NSColor.darkGray
	}
}
