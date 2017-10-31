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

func NSStringFromSCNVector3(_ vec: SCNVector3) -> NSString {
	return NSString(format: "{ %f, %f, %f }", vec.x, vec.y, vec.z)
}

func NSStringFromSCNVector4(_ vec: SCNVector4) -> NSString {
	return NSString(format: "{ %f, %f, %f, %f }", vec.x, vec.y, vec.z, vec.w)
}

func NSStringFromSCNMatrix4(_ m: SCNMatrix4) -> NSString {
	return NSString(format: "%.2f\t%.2f\t%.2f\t%.2f\n%.2f\t%.2f\t%.2f\t%.2f\n%.2f\t%.2f\t%.2f\t%.2f\n%.2f\t%.2f\t%.2f\t%.2f\n",
		m.m11, m.m21, m.m31, m.m41,
		m.m12, m.m22, m.m32, m.m42,
		m.m13, m.m23, m.m33, m.m43,
		m.m14, m.m24, m.m34, m.m44)
}


class GameViewController: NSViewController {
    
	override func viewDidLoad() {
		super.viewDidLoad()
		
		
		
		let MATRIX_TEST = false
		enum TestCase {
			case matrix
			case convenience1
			case euler
			case reverseEuler
			case rotationAnimation
		}
		
		let TEST: TestCase = .euler
		
		
		if (TEST == .matrix) {

			// test using raw matrix manipulation
			
			let aScene = SCNScene(named: "art.scnassets/dragon.obj")!
			let aNode = aScene.rootNode.childNodes[0]
			aNode.name = "A"
			let aTranslate = 	SCNMatrix4MakeTranslation(10.0, 0.0, 0.0)
			let aScale = 		SCNMatrix4MakeScale(2.0, 2.0, 2.0)
			let aRotate = 		SCNMatrix4MakeRotation(CGFloat(D2R(45.0)), 0.0, 0.0, 1.0)
			let aTIn =			SCNMatrix4Mult(SCNMatrix4Mult(aTranslate, aScale), aRotate)
			NSLog("aTIn:\n\(NSStringFromSCNMatrix4(aTIn))")
			aNode.transform = 	aTIn
			NSLog("aNode transform:\n\(NSStringFromSCNMatrix4(aNode.transform))")
			NSLog("aNode orientation: \(NSStringFromSCNVector4(aNode.orientation))")
			NSLog("aNode rotation: \(NSStringFromSCNVector4(aNode.rotation))")
			NSLog("aNode eulerRangles: \(NSStringFromSCNVector3(aNode.eulerAngles))")
			
			
			
			let bScene = SCNScene(named: "art.scnassets/dragon.obj")!
			let bNode = bScene.rootNode.childNodes[0]
			bNode.name = "B"
			let bTranslate = 	SCNMatrix4MakeTranslation(0.0, -20.0, -7.0)
			let bScale = 		SCNMatrix4MakeScale(1.0, 1.0, 1.5)
			let bRotate = 		SCNMatrix4MakeRotation(CGFloat(D2R(30.0)), 1.0, 1.0, 0.0)
			let bTIn =			SCNMatrix4Mult(SCNMatrix4Mult(bTranslate, bScale), bRotate)
			NSLog("bTIn:\n\(NSStringFromSCNMatrix4(bTIn))")
			bNode.transform = bTIn
			NSLog("bNode transform:\n\(NSStringFromSCNMatrix4(bNode.transform))")
			NSLog("bNode orientation: \(NSStringFromSCNVector4(bNode.orientation))")
			NSLog("bNode rotation: \(NSStringFromSCNVector4(bNode.rotation))")
			NSLog("bNode eulerRangles: \(NSStringFromSCNVector3(bNode.eulerAngles))")
			aNode.addChildNode(bNode);
			
			
			
			let fScene = SCNScene(named: "art.scnassets/dragon.obj")!
			let fNode = fScene.rootNode.childNodes[0]
			fNode.name = "F"
			let fTranslate = 	SCNMatrix4MakeTranslation(-16.0, 0.0, -60.0)
			let fScale = 		SCNMatrix4MakeScale(2.0, 5.0, 3.0)
			let fRotate = 		SCNMatrix4MakeRotation(CGFloat(D2R(-90.0)), 0.0, 1.0, 3.0)
			let fTIn =			SCNMatrix4Mult(SCNMatrix4Mult(fTranslate, fScale), fRotate)
			NSLog("fTIn:\n\(NSStringFromSCNMatrix4(fTIn))")
			fNode.transform = fTIn
			NSLog("fNode transform:\n\(NSStringFromSCNMatrix4(fNode.transform))")
			NSLog("fNode orientation: \(NSStringFromSCNVector4(fNode.orientation))")
			NSLog("fNode rotation: \(NSStringFromSCNVector4(fNode.rotation))")
			NSLog("fNode eulerRangles: \(NSStringFromSCNVector3(fNode.eulerAngles))")
			bNode.addChildNode(fNode);
			
			
			
			let pScene = SCNScene(named: "art.scnassets/cartoon_palm_tree.obj")!
			let pNode = pScene.rootNode.childNodes[0]
			pNode.name = "P"
			let pTranslate = 	SCNMatrix4MakeTranslation(0.0, -7.0, 0.0)
			let pScale = 		SCNMatrix4MakeScale(1.0, 1.0, 10.0)
			let pRotate = 		SCNMatrix4MakeRotation(CGFloat(D2R(50.0)), 1.0, 1.0, 0.0)
			let pTIn =			SCNMatrix4Mult(SCNMatrix4Mult(pScale, pRotate), pTranslate)
			NSLog("pTIn:\n\(NSStringFromSCNMatrix4(pTIn))")
			pNode.transform = pTIn
			NSLog("pNode transform:\n\(NSStringFromSCNMatrix4(pNode.transform))")
			NSLog("pNode orientation: \(NSStringFromSCNVector4(pNode.orientation))")
			NSLog("pNode rotation: \(NSStringFromSCNVector4(pNode.rotation))")
			NSLog("pNode eulerRangles: \(NSStringFromSCNVector3(pNode.eulerAngles))")
			fNode.addChildNode(pNode);
			
			
			
			let gScene = SCNScene(named: "art.scnassets/ConvaliaBouquet.obj")!
			let gNode = gScene.rootNode.childNodes[0]
			gNode.name = "P"
			let gTranslate = 	SCNMatrix4MakeTranslation(0.0, -7.0, 0.0)
			let gScale = 		SCNMatrix4MakeScale(1.0, 1.0, 1.0)
			let gRotate = 		SCNMatrix4MakeRotation(CGFloat(D2R(-50.0)), 1.0, 1.0, 0.0)
			let gTIn =			SCNMatrix4Mult(SCNMatrix4Mult(gRotate, gTranslate), gScale)
			NSLog("gTIn:\n\(NSStringFromSCNMatrix4(gTIn))")
			gNode.transform = gTIn
			NSLog("gNode transform:\n\(NSStringFromSCNMatrix4(gNode.transform))")
			NSLog("gNode orientation: \(NSStringFromSCNVector4(gNode.orientation))")
			NSLog("gNode rotation: \(NSStringFromSCNVector4(gNode.rotation))")
			NSLog("gNode eulerRangles: \(NSStringFromSCNVector3(gNode.eulerAngles))")
			aNode.addChildNode(gNode);
		
		
		
			let scene = SCNScene()
			scene.rootNode.addChildNode(aNode)
			
			NSLog("aNode worldTransform:\n\(NSStringFromSCNMatrix4(aNode.worldTransform))")
			NSLog("bNode worldTransform:\n\(NSStringFromSCNMatrix4(bNode.worldTransform))")
			NSLog("fNode worldTransform:\n\(NSStringFromSCNMatrix4(fNode.worldTransform))")
			NSLog("pNode worldTransform:\n\(NSStringFromSCNMatrix4(pNode.worldTransform))")
			
			let cameraNode = SCNNode()
			cameraNode.camera = SCNCamera()
			cameraNode.camera?.xFov = 30.0
			cameraNode.camera?.yFov = 30.0
			cameraNode.camera?.zNear = 0.01
			cameraNode.camera?.zFar = 1000.0
			scene.rootNode.addChildNode(cameraNode)
			cameraNode.position = SCNVector3(x: 0, y: 5, z: 100)
			
			let scnView = self.view as! SCNView
			scnView.scene = scene
			scnView.backgroundColor = NSColor.darkGray
			scnView.autoenablesDefaultLighting = true
			
		}
		else if (TEST == .convenience1 ) {

			// test using constituent parts manipulation
			
			
			let xNode = SCNNode();
			

			let aScene = SCNScene(named: "art.scnassets/dragon.obj")!
			let aNode = aScene.rootNode.childNodes[0]
			aNode.name = "A"
			aNode.position = SCNVector3(20.0, 0.0, 0.0)
			aNode.scale = SCNVector3(2.0, 3.0, 2.0)
			aNode.rotation = SCNVector4(0.0, 1.0, 3.0, CGFloat(D2R(45.0)))
			xNode.addChildNode(aNode)
			
			
			
			let bScene = SCNScene(named: "art.scnassets/ConvaliaBouquet.obj")!
			let bNode = bScene.rootNode.childNodes[0]
			bNode.name = "B"
			bNode.position = SCNVector3(0.0, -3.0, 0.0)
			bNode.rotation = SCNVector4(3.0, 1.0, 2.0, CGFloat(D2R(574.0)))
			bNode.scale = SCNVector3(15.0, 1.0, 1.0)
			aNode.addChildNode(bNode)
			NSLog("bNode.rotation: \(bNode.rotation)")



			let cScene = SCNScene(named: "art.scnassets/cartoon_palm_tree.obj")!
			let cNode = cScene.rootNode.childNodes[0]
			cNode.name = "C"
			cNode.rotation = SCNVector4(3.0, 13.0, 3.0, CGFloat(D2R(-110.0)))
			cNode.scale = SCNVector3(0.5, 0.5, -2.0)
			cNode.position = SCNVector3(-2.0, 1.0, -2.0)
			bNode.addChildNode(cNode)



			// FAILS
			let dScene = SCNScene(named: "art.scnassets/dragon.obj")!
			let dNode = dScene.rootNode.childNodes[0]
			dNode.name = "D"
			dNode.position = SCNVector3(-15.0, 10.0, 20.0)
			dNode.eulerAngles = SCNVector3(CGFloat(D2R(45.0)), CGFloat(D2R(60.0)), CGFloat(D2R(30.0)))
			xNode.addChildNode(dNode)
			
			
			
			let scene = SCNScene()
			scene.rootNode.addChildNode(xNode)
			
			NSLog("aNode worldTransform:\n\(NSStringFromSCNMatrix4(aNode.worldTransform))")
//			NSLog("bNode worldTransform:\n\(NSStringFromSCNMatrix4(bNode.worldTransform))")
//			NSLog("cNode worldTransform:\n\(NSStringFromSCNMatrix4(cNode.worldTransform))")
//			NSLog("dNode worldTransform:\n\(NSStringFromSCNMatrix4(dNode.worldTransform))")
			
			let cameraNode = SCNNode()
			cameraNode.camera = SCNCamera()
			cameraNode.camera?.xFov = 30.0
			cameraNode.camera?.yFov = 30.0
			cameraNode.camera?.zNear = 0.01
			cameraNode.camera?.zFar = 1000.0
			scene.rootNode.addChildNode(cameraNode)
			cameraNode.position = SCNVector3(x: 0, y: 5, z: 100)
			
			let scnView = self.view as! SCNView
			scnView.scene = scene
			scnView.backgroundColor = NSColor.darkGray
			scnView.autoenablesDefaultLighting = true
		}
		else if (TEST == .euler) {
			
			let rootNode = SCNNode()
			
			let aScene = SCNScene(named: "art.scnassets/teapot.dae")!
			let aNode = aScene.rootNode.childNodes[1]
			aNode.name = "A"
			aNode.position = SCNVector3(-25.0, 25.0, 0.0)
			aNode.eulerAngles = SCNVector3(CGFloat(D2R(45.0)), 0, 0)
			
			
			let bScene = SCNScene(named: "art.scnassets/teapot.dae")!
			let bNode = bScene.rootNode.childNodes[1]
			bNode.name = "B"
			bNode.position = SCNVector3(25.0, 25.0, 0.0)
			bNode.eulerAngles = SCNVector3(0, CGFloat(D2R(45.0)), 0)
			
			
			let cScene = SCNScene(named: "art.scnassets/teapot.dae")!
			let cNode = cScene.rootNode.childNodes[1]
			cNode.name = "C"
			cNode.position = SCNVector3(-25.0, -25.0, 0.0)
			cNode.eulerAngles = SCNVector3(0, 0, CGFloat(D2R(45.0)))
			
			
			let dScene = SCNScene(named: "art.scnassets/teapot.dae")!
			let dNode = dScene.rootNode.childNodes[1]
			dNode.name = "D"
			dNode.position = SCNVector3(25.0, -25.0, 0.0)
			dNode.eulerAngles = SCNVector3(0, 0, CGFloat(D2R(45.0)))
			dNode.eulerAngles = SCNVector3(CGFloat(D2R(30.0)), CGFloat(D2R(45.0)), CGFloat(D2R(60.0)))
			
			
			let eScene = SCNScene(named: "art.scnassets/teapot.dae")!
			let eNode = eScene.rootNode.childNodes[1]
			eNode.name = "E"
			eNode.position = SCNVector3(0.0, 0.0, 0.0)
			eNode.eulerAngles = SCNVector3(0, 0, CGFloat(D2R(45.0)))
			eNode.eulerAngles = SCNVector3(CGFloat(D2R(-130.0)), CGFloat(D2R(70.0)), CGFloat(D2R(20.0)))
			
			
			rootNode.addChildNode(aNode)
			rootNode.addChildNode(bNode)
			rootNode.addChildNode(cNode)
			rootNode.addChildNode(dNode)
			rootNode.addChildNode(eNode)
			
			
			let scene = SCNScene()
			scene.rootNode.addChildNode(rootNode)
			
			NSLog("aNode worldTransform:\n\(NSStringFromSCNMatrix4(aNode.worldTransform))")
			NSLog("bNode worldTransform:\n\(NSStringFromSCNMatrix4(bNode.worldTransform))")
			NSLog("cNode worldTransform:\n\(NSStringFromSCNMatrix4(cNode.worldTransform))")
			NSLog("dNode worldTransform:\n\(NSStringFromSCNMatrix4(dNode.worldTransform))")
			NSLog("eNode worldTransform:\n\(NSStringFromSCNMatrix4(eNode.worldTransform))")
			
			let cameraNode = SCNNode()
			cameraNode.camera = SCNCamera()
			cameraNode.camera?.xFov = 30.0
			cameraNode.camera?.yFov = 30.0
			cameraNode.camera?.zNear = 0.01
			cameraNode.camera?.zFar = 1000.0
			scene.rootNode.addChildNode(cameraNode)
			cameraNode.position = SCNVector3(x: 0, y: 10, z: 150)
			
			let scnView = self.view as! SCNView
			scnView.scene = scene
			scnView.backgroundColor = NSColor.darkGray
			scnView.autoenablesDefaultLighting = true
		}
		else if (TEST == .reverseEuler) {
			
			let rootNode = SCNNode()
			
			let aScene = SCNScene(named: "art.scnassets/teapot.dae")!
			let aNode = aScene.rootNode.childNodes[1]
			aNode.name = "A"
			aNode.position = SCNVector3(-25.0, 25.0, 0.0)
			aNode.orientation = SCNQuaternion(1.0, 0.0, 0.0, CGFloat(D2R(45.0)))
			NSLog("aNode eulerRangles: \(NSStringFromSCNVector3(aNode.eulerAngles))")
			
			
			
			let bScene = SCNScene(named: "art.scnassets/teapot.dae")!
			let bNode = bScene.rootNode.childNodes[1]
			bNode.name = "B"
			bNode.position = SCNVector3(25.0, 25.0, 0.0)
			bNode.eulerAngles = SCNVector3(0, CGFloat(D2R(45.0)), 0)
			bNode.orientation = SCNQuaternion(0.0, 1.0, 0.0, CGFloat(D2R(45.0)))
			NSLog("bNode eulerRangles: \(NSStringFromSCNVector3(bNode.eulerAngles))")
			
			
			
			let cScene = SCNScene(named: "art.scnassets/teapot.dae")!
			let cNode = cScene.rootNode.childNodes[1]
			cNode.name = "C"
			cNode.position = SCNVector3(-25.0, -25.0, 0.0)
			cNode.eulerAngles = SCNVector3(0, 0, CGFloat(D2R(45.0)))
			cNode.orientation = SCNQuaternion(0.0, 0.0, 1.0, CGFloat(D2R(45.0)))
			NSLog("cNode eulerRangles: \(NSStringFromSCNVector3(cNode.eulerAngles))")
			
			
			
			let dScene = SCNScene(named: "art.scnassets/teapot.dae")!
			let dNode = dScene.rootNode.childNodes[1]
			dNode.name = "D"
			dNode.position = SCNVector3(25.0, -25.0, 0.0)
			dNode.eulerAngles = SCNVector3(0, 0, CGFloat(D2R(45.0)))
			dNode.eulerAngles = SCNVector3(CGFloat(D2R(30.0)), CGFloat(D2R(45.0)), CGFloat(D2R(60.0)))
			dNode.orientation = SCNQuaternion(0.5, 0.25, 0.35, CGFloat(D2R(45.0)))
			NSLog("dNode eulerRangles: \(NSStringFromSCNVector3(dNode.eulerAngles))")
			
			
			
			
			rootNode.addChildNode(aNode)
			rootNode.addChildNode(bNode)
			rootNode.addChildNode(cNode)
			rootNode.addChildNode(dNode)
			//rootNode.addChildNode(eNode)
			
			
			
			let scene = SCNScene()
			scene.rootNode.addChildNode(rootNode)
			
			NSLog("aNode worldTransform:\n\(NSStringFromSCNMatrix4(aNode.worldTransform))")
//			NSLog("bNode worldTransform:\n\(NSStringFromSCNMatrix4(bNode.worldTransform))")
//			NSLog("cNode worldTransform:\n\(NSStringFromSCNMatrix4(cNode.worldTransform))")
//			NSLog("dNode worldTransform:\n\(NSStringFromSCNMatrix4(dNode.worldTransform))")
//			NSLog("eNode worldTransform:\n\(NSStringFromSCNMatrix4(eNode.worldTransform))")
			
			let cameraNode = SCNNode()
			cameraNode.camera = SCNCamera()
			cameraNode.camera?.xFov = 30.0
			cameraNode.camera?.yFov = 30.0
			cameraNode.camera?.zNear = 0.01
			cameraNode.camera?.zFar = 1000.0
			scene.rootNode.addChildNode(cameraNode)
			cameraNode.position = SCNVector3(x: 0, y: 10, z: 150)
			
			let scnView = self.view as! SCNView
			scnView.scene = scene
			scnView.backgroundColor = NSColor.darkGray
			scnView.autoenablesDefaultLighting = true
		}
		else if (TEST == .rotationAnimation) {
			
		}
	}
}
