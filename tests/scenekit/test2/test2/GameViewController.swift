//
//  GameViewController.swift
//  test2
//
//  Created by Morgan Davis on 9/26/17.
//  Copyright © 2017 Morgan Davis. All rights reserved.
//

import SceneKit
import QuartzCore


func D2R(_ d: Float) -> Float {
	return d * (3.1415 / 180.0)
}


func NSStringFromSCNMatrix4(_ m: SCNMatrix4) -> NSString {
	return NSString(format: "%03.3f, %03.3f, %03.3f, %3.3f\n%03.3f, %03.3f, %03.3f, %3.3f\n%03.3f, %03.3f, %03.3f, %3.3f\n%03.3f, %03.3f, %03.3f, %3.3f\n",
//	                m.m11, m.m12, m.m13, m.m14,
//					m.m21, m.m22, m.m23, m.m24,
//					m.m31, m.m32, m.m33, m.m34,
//					m.m41, m.m42, m.m43, m.m44)
		m.m11, m.m21, m.m31, m.m41,
		m.m12, m.m22, m.m32, m.m42,
		m.m13, m.m23, m.m33, m.m43,
		m.m14, m.m24, m.m34, m.m44)
}


class GameViewController: NSViewController {
    
	override func viewDidLoad() {
		super.viewDidLoad()
		
		
		// BoxA: load, translate +10x, scale 2xyz, rotate 45deg on z
		
		let boxAScene = SCNScene(named: "art.scnassets/dragon.obj")!
		//var boxANode = boxAScene.rootNode.childNode(withName: "teapot", recursively: false)
		var boxANode = boxAScene.rootNode.childNodes[0]

		boxANode.name = "A"
		
		NSLog("boxANode: \(NSStringFromSCNMatrix4(boxANode.transform))")
		
		
//		let boxATranslate = 	SCNMatrix4MakeTranslation(10.0, 0.0, 0.0)
//		let boxAScale = 		SCNMatrix4MakeScale(2.0, 2.0, 2.0)
//		let boxARotate = 		SCNMatrix4MakeRotation(CGFloat(D2R(45.0)), 0.0, 0.0, 1.0)
//		boxANode?.transform = 	SCNMatrix4Mult(SCNMatrix4Mult(boxATranslate, boxAScale), boxARotate)
		//boxANode?.transform = 	SCNMatrix4Mult(boxARotate, SCNMatrix4Mult(boxAScale, boxATranslate))
		
//		NSLog("boxATranslate:\n\(NSStringFromSCNMatrix4(boxATranslate))")
//		NSLog("boxAScale:\n\(NSStringFromSCNMatrix4(boxAScale))")
//		NSLog("boxARotate:\n\(NSStringFromSCNMatrix4(boxARotate))")
//		NSLog("boxANode?.transform:\n\(NSStringFromSCNMatrix4(boxANode!.transform))")
		
		
		// BoxB: load, translate -10x, -5y : attach to BoxA
		
//		let boxBScene = SCNScene(named: "art.scnassets/teapot.dae")!
//		var boxBNode = boxBScene.rootNode.childNode(withName: "teapot", recursively: false)
//		boxBNode?.name = "B"
//		let boxBTranslate = SCNMatrix4MakeTranslation(-15.0, -5.0, 0.0)
//		boxBNode?.transform = boxBTranslate
//		boxANode?.addChildNode(boxBNode!)
		
		
		// BoxC: load, translate -10z : attach to BoxB
		
//		let boxCScene = SCNScene(named: "art.scnassets/teapot.dae")!
//		var boxCNode = boxCScene.rootNode.childNode(withName: "teapot", recursively: false)
//		boxCNode?.name = "C"
//		let boxCTranslate = SCNMatrix4MakeTranslation(0.0, 0.0, -10.0)
//		boxCNode?.transform = boxCTranslate
//		boxBNode?.addChildNode(boxCNode!)
		
		
		
		
		var scene = SCNScene()
		scene.rootNode.addChildNode(boxANode)
		
		let cameraNode = SCNNode()
		cameraNode.camera = SCNCamera()
		cameraNode.camera?.xFov = 30.0
		cameraNode.camera?.yFov = 30.0
		cameraNode.camera?.zNear = 0.0001
		cameraNode.camera?.zFar = 1000.0
		scene.rootNode.addChildNode(cameraNode)
		cameraNode.position = SCNVector3(x: 0, y: 5, z: 50)
		
		
		
		
		
		let scnView = self.view as! SCNView
		scnView.scene = scene
		//scnView.allowsCameraControl = true
		scnView.backgroundColor = NSColor.darkGray
		scnView.autoenablesDefaultLighting = true
		

		
		
	}
}
