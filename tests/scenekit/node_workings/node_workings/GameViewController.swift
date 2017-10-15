//
//  GameViewController.swift
//  node_workings
//
//  Created by Morgan Davis on 10/11/17.
//  Copyright © 2017 Morgan Davis. All rights reserved.
//

import SceneKit
import QuartzCore



func StringFromSCNVector3(_ vec: SCNVector3) -> String {
	return String(format: "{ %f, %f, %f }", vec.x, vec.y, vec.z)
}

func StringFromSCNVector4(_ vec: SCNVector4) -> String {
	return String(format: "{ %f, %f, %f, %f }", vec.x, vec.y, vec.z, vec.w)
}

func StringFromSCNMatrix4(_ mat: SCNMatrix4) -> String {
	return String(format: "{ %.2f, %.2f, %.2f, %.2f \n %.2f, %.2f, %.2f, %.2f \n %.2f, %.2f, %.2f, %.2f \n %.2f, %.2f, %.2f, %.2f }",
	                mat.m11, mat.m12, mat.m13, mat.m14,
					mat.m21, mat.m22, mat.m23, mat.m24,
					mat.m31, mat.m32, mat.m33, mat.m34,
					mat.m41, mat.m42, mat.m43, mat.m44)
}



class GameViewController: NSViewController {
    
    override func viewDidLoad() {
        super.viewDidLoad()
		
		let pi: CGFloat = 3.1415

		var node = SCNNode();
		
//		node.transform = SCNMatrix4MakeTranslation(1, 2, 3)
//		NSLog("postion: " + StringFromSCNVector3(node.position))
//
//		node.position = SCNVector3Make(1, 2, 3)
//		NSLog("transform:\n" + StringFromSCNMatrix4(node.transform))
		
		
//		node.transform = SCNMatrix4MakeRotation(pi / 2.0, 1.0, 0, 0)
//		NSLog("rotation: " + StringFromSCNVector4(node.rotation))
		
//		node.rotation = SCNVector4Make(1, 0, 0, pi / 2.0)
//		NSLog("transform:\n" + StringFromSCNMatrix4(node.transform))
		
		
		let rotateX45 = SCNMatrix4MakeRotation(pi / 2.0, 1.0, 0, 0)
		let scaleXYZ2 = SCNMatrix4MakeScale(2.0, 2.0, 2.0)
		let translateX2 = SCNMatrix4MakeTranslation(1.0, 0.0, 0.0)
		
		
//		node.rotation = SCNVector4Make(1, 0, 0, pi / 2.0)
//		node.position = SCNVector3Make(node.position.x + 1.0, node.position.y, node.position.z)
		
//		node.rotation = SCNVector4Make(1, 0, 0, pi / 2.0)
//		node.scale = SCNVector3Make(2.0, 2.0, 2.0)
//		node.position = SCNVector3Make(node.position.x + 2.0, node.position.y, node.position.z)
//		NSLog("postion: " + StringFromSCNVector3(node.position))
//		NSLog("transform:\n" + StringFromSCNMatrix4(node.transform))
		
		node.transform = SCNMatrix4Mult(rotateX45, node.transform)
		node.transform = SCNMatrix4Mult(translateX2, node.transform)
		NSLog("transform:\n" + StringFromSCNMatrix4(node.transform))
		
		
//		node.position = SCNVector3Make(node.position.x + 2.0, node.position.y, node.position.z)
//		node.scale = SCNVector3Make(2.0, 2.0, 2.0)
//		node.rotation = SCNVector4Make(1, 0, 0, pi / 2.0)
//		NSLog("postion: " + StringFromSCNVector3(node.position))
//		NSLog("transform:\n" + StringFromSCNMatrix4(node.transform))
	}
}
