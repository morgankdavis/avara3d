//
//  GameViewController.swift
//  test03-input-camera
//
//  Created by Morgan Davis on 10/17/17.
//  Copyright © 2017 Morgan Davis. All rights reserved.
//

import SceneKit
import QuartzCore


class GameViewController: NSViewController, SCNSceneRendererDelegate {
	
	
	private			var accumMouseDelta = 					CGPoint()
	private         var lastRenderTime:                     Double?
	private			var cameraNode = 						SCNNode()
	private			var cameraRotation =					SCNVector3(x: 0, y: 0, z: 0)
	
	
	public func mouseDelta(_ delta: CGPoint) {
		//NSLog("Mouse delta: \(delta.x), \(delta.y)")
		accumMouseDelta.x += delta.x
		accumMouseDelta.y += delta.y
		
		(self.view as! SCNView).play(self) // force redraw
	}
	
	
	
	/*****************************************************************************************************/
	// MARK:    SCNSceneRendererDelegate
	/*****************************************************************************************************/
	
	public func renderer(_ renderer: SCNSceneRenderer, updateAtTime time: TimeInterval) {
		//NSLog("renderer(%@, updateAtTime: %f)", renderer.description, time)
		
		if let lastTime = self.lastRenderTime {
			lastRenderTime = time

			let dT = Double(time - lastTime)

			//dispatch_async(dispatch_get_main_queue(),{
			self.gameLoop(dT)
			//})
		}
		else {
			lastRenderTime = time
		}
	}
	
	
	private func gameLoop(_ dT: Double) {
		//NSLog("gameLoop");
		
		
		
		let mouseSensitivity: Float = 0.005
		
		cameraRotation.x += CGFloat(mouseSensitivity * Float(accumMouseDelta.x))
		cameraRotation.y += CGFloat(mouseSensitivity * Float(accumMouseDelta.y))
		
		let rotX = SCNMatrix4MakeRotation(cameraRotation.x, 0.0, -1.0, 0.0)
		let rotY = SCNMatrix4MakeRotation(cameraRotation.y, 1.0, 0.0, 0.0)
		
		let rot = SCNMatrix4Mult(rotX, rotY)
//		let transform = SCNMatrix4Mult(cameraNode.transform, rot);
//		cameraNode.transform = transform

		
		let posMat = SCNMatrix4MakeTranslation(0, 0, 1.45)
		let transform = SCNMatrix4Mult(rot, posMat);
		cameraNode.transform = transform
		//cameraNode.transform = (SCNMatrix4Mult(cameraNode.transform, transform))
		
		let rotateion = cameraNode.rotation
		//NSLog("rotateion: \(rotateion.x), \(rotateion.y), \(rotateion.z), \(rotateion.w)")
		
//		// look
//
//		#if os(OSX)
//			let viewDistanceFactor = 1.0/(MOUSELOOK_SENSITIVITY*MOUSELOOK_SENSITIVITY_MULTIPLIER)
//		#else
//			let viewDistanceFactor = 1.0/(THUMBLOOK_SENSITIVITY*THUMBLOOK_SENSITIVITY_MULTIPLIER)
//		#endif
//
//		let dP = acos(CGFloat(lookDelta.x) / viewDistanceFactor) - CGFloat(M_PI_2)
//		let dY = acos(CGFloat(lookDelta.y) / viewDistanceFactor) - CGFloat(M_PI_2)
//
//		var nAngles = SCNVector3(
//			x: node.eulerAngles.x + MKDFloat(dY),
//			y: node.eulerAngles.y - MKDFloat(dP),
//			z: node.eulerAngles.z)
//
//		nAngles.x = max(-MKDFloat(VERT_CLAMP), min(MKDFloat(VERT_CLAMP), nAngles.x)) // clamp vertical angle
//
//		node.eulerAngles = nAngles

		
		
		accumMouseDelta.x = 0
		accumMouseDelta.y = 0
	}
	
	
	
    override func viewDidLoad() {
        super.viewDidLoad()
		
		
		
		
        
        // create a new scene
        let scene = SCNScene(named: "art.scnassets/importTest.dae")!
        

		//let cameraNode = SCNNode()
		cameraNode.camera = SCNCamera()
		scene.rootNode.addChildNode(cameraNode)
		
		// place the camera
		cameraNode.position = SCNVector3(x: 0, y: 0, z: 1.45)
		

        // retrieve the SCNView
        let scnView = self.view as! SCNView
		scnView.delegate = self
        
        // set the scene to the view
        scnView.scene = scene

    }
	
}
