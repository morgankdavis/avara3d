//
//  GameViewController.swift
//  DynamicChildTest
//
//  Created by Morgan Davis on 5/25/18.
//  Copyright © 2018 Morgan Davis. All rights reserved.
//

import SceneKit
import QuartzCore

class GameViewController: NSViewController, SCNSceneRendererDelegate {
	
	var sphereNode: SCNNode!
	var boxNode: SCNNode!
    
    override func viewDidLoad() {
        super.viewDidLoad()
        
        // create a new scene
        let scene = SCNScene(named: "art.scnassets/ship.scn")!
		scene.physicsWorld.speed = 0.25
        
        // create and add a camera to the scene
        let cameraNode = SCNNode()
        cameraNode.camera = SCNCamera()
		cameraNode.camera?.fieldOfView = 75
        scene.rootNode.addChildNode(cameraNode)
        
        // place the camera
        cameraNode.position = SCNVector3(x: 0, y: 0, z: 15)
        
        // create and add a light to the scene
        let lightNode = SCNNode()
        lightNode.light = SCNLight()
        lightNode.light!.type = .omni
        lightNode.position = SCNVector3(x: 0, y: 10, z: 10)
        scene.rootNode.addChildNode(lightNode)
        
        // create and add an ambient light to the scene
        let ambientLightNode = SCNNode()
        ambientLightNode.light = SCNLight()
        ambientLightNode.light!.type = .ambient
        ambientLightNode.light!.color = NSColor.darkGray
        scene.rootNode.addChildNode(ambientLightNode)
        
		
		
		
        // retrieve the ship node
        let ship = scene.rootNode.childNode(withName: "ship", recursively: true)!
        
        // animate the 3d object
        ship.runAction(SCNAction.repeatForever(SCNAction.rotateBy(x: 0, y: 2, z: 0, duration: 5)))
		
		
		
		
		
		let sphere = SCNSphere(radius: 0.5);
		sphereNode = SCNNode(geometry: sphere)
		sphereNode.physicsBody = SCNPhysicsBody.dynamic()
		sphereNode.physicsBody?.isAffectedByGravity = false
		
		
		let box = SCNBox(width: 0.5, height: 0.5, length: 2.0, chamferRadius: 0)
		boxNode = SCNNode(geometry: box)
		boxNode.position = SCNVector3(10, 0, 0)
	
		
		boxNode.addChildNode(sphereNode)
		ship.addChildNode(boxNode)
		
		
		//ship.addChildNode(sphereNode)
		
		
		
        
        // retrieve the SCNView
        let scnView = self.view as! SCNView
        
        // set the scene to the view
        scnView.scene = scene
        
        // allows the user to manipulate the camera
        scnView.allowsCameraControl = true
        
        // show statistics such as fps and timing information
        scnView.showsStatistics = true
        
        // configure the view
        scnView.backgroundColor = NSColor.black
        
        // Add a click gesture recognizer
        let click1Gesture = NSClickGestureRecognizer(target: self, action: #selector(mouse1Click(_:)))
		click1Gesture.buttonMask = 1 << 0;
//        var gestureRecognizers = scnView.gestureRecognizers
//        gestureRecognizers.insert(clickGesture, at: 0)
//        scnView.gestureRecognizers = gestureRecognizers
		
		let click2Gesture = NSClickGestureRecognizer(target: self, action: #selector(mouse2Click(_:)))
		click2Gesture.buttonMask = 1 << 1;
		
		
		var gestureRecognizers = scnView.gestureRecognizers
		gestureRecognizers.insert(click1Gesture, at: 0)
		gestureRecognizers.insert(click2Gesture, at: 1)
		scnView.gestureRecognizers = gestureRecognizers
		
		
		
		// SCNSceneRendererDelegate
		
		scnView.delegate = self
		
    }
	
	@objc
	func renderer(_ renderer: SCNSceneRenderer, updateAtTime time: TimeInterval) {
		NSLog("sphereNode!.position: {\(sphereNode.position.x), \(sphereNode.position.y), \(sphereNode.position.z)}")
		var spherePresentation = sphereNode.presentation
		NSLog("sphereNode!.presentation: {\(spherePresentation.position.x), \(spherePresentation.position.y), \(spherePresentation.position.z)}")
		
//		NSLog("boxNode!.position: {\(boxNode.position.x), \(boxNode.position.y), \(boxNode.position.z)}")
//		var boxPresentation = boxNode.presentation
//		NSLog("boxPresentation!.presentation: {\(boxPresentation.position.x), \(boxPresentation.position.y), \(boxPresentation.position.z)}")
	}
    
    @objc
    func mouse1Click(_ gestureRecognizer: NSGestureRecognizer) {
		NSLog("mouse1Click")
		
		//sphereNode!.position = SCNVector3(sphereNode!.position.x, sphereNode!.position.y + 0.25, sphereNode!.position.z)
		
		sphereNode!.physicsBody!.velocity = SCNVector3(0, 0, 0)
		sphereNode!.position = SCNVector3(0, 0, 0)
		
		
//        // retrieve the SCNView
//        let scnView = self.view as! SCNView
//        
//        // check what nodes are clicked
//        let p = gestureRecognizer.location(in: scnView)
//        let hitResults = scnView.hitTest(p, options: [:])
//        // check that we clicked on at least one object
//        if hitResults.count > 0 {
//            // retrieved the first clicked object
//            let result = hitResults[0]
//            
//            // get its material
//            let material = result.node.geometry!.firstMaterial!
//            
//            // highlight it
//            SCNTransaction.begin()
//            SCNTransaction.animationDuration = 0.5
//            
//            // on completion - unhighlight
//            SCNTransaction.completionBlock = {
//                SCNTransaction.begin()
//                SCNTransaction.animationDuration = 0.5
//                
//                material.emission.contents = NSColor.black
//                
//                SCNTransaction.commit()
//            }
//            
//            material.emission.contents = NSColor.red
//            
//            SCNTransaction.commit()
//        }
    }
	
	@objc
	func mouse2Click(_ gestureRecognizer: NSGestureRecognizer) {
		
		NSLog("mouse2Click")
		
		
		boxNode.position = SCNVector3(-boxNode.position.x, 0, 0)
		
		//NSLog("BOX.pivot.position: {\(boxNode!.pivot.position.x), \(spherePresentation.position.y), \(spherePresentation.position.z)}")
	}
}
