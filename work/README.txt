COMP308 Assignment 1

Name :: Hamish Rae-Hodgson
Student ID :: 300169925


1) List what you have completed:

	Stage 1: 100%
		Core: 
			- Add keyframes (SHIFT + CLICK)
			- Display Catmull-Rom Spline

		Extension:
			- Display speed curve in top left of window
			- (SHIFT + CLICK) on top left box to add control
				point to speed curve. Click elsewhere to add
				point to the tragectory spline.
			- Run shapes along the splines
			- Project spline onto the XZ plane and run our
				skeleton along it. (ROTATE CAMERA with LEFT 
				MOUSE BUTTON + DRAG)
	Stage 2: 100%
		- Reads and renders an ASF file (priman) from command line
		- If missing args, prompts for required args
		- CTRL + LEFT CLICK to select a joint.
		- Selecting a joint deselects any other selected joint
		- Color of selected joint et al. changes appropriately
		- CTRL + LEFT CLICK on selected join to deselect it
		- Colors revert upon deselection
		- PUSH X while a join is selected to change highlighted axis
			It will cycle through x->y->z (providing joint has dof)
		- If joint is selected RIGHT CLICK + DRAG_UP_DOWN rotates
			joint around selected axis
		- Selected joint appears in bottom left of window including
			it's current rotation details
		- PRESS S to prompt the user (ON THE COMMAND LINE) for a 
			filename. Then save the current skeleton joint info
			in AMC FORMAT
		- CONFIG FILES in ./work/res/assets "walk.cfg" & "all.cfg"
		- FRAME FILES in ./work/res/assets/frames
		- program accepts a 3rd optional argument specifying a
			config file. i.e: ./work/res/assets/all.cfg ..
			it then loads the keyframes and animates the inbetweens'
		- PRESS 'A' to start the animation
		- MIDDLE CLICK to pop up a glut menu for PLAY, PAUSE, STOP,
			REWIND, FAST FORWARD, FAST REWIND
		- I constructed the walk cycle using my interactive controls
			and frame saving abilities
		- I created config files containing sequences of these
				frames
		- The file 'all.cfg' contains the sequence of poses
			including SIT, STAND, WALK0, WALK1, WALK2, CUSTOM
	Stage 3:
		- ROTATION
			- LEFT MOUSE + DRAG to rotate the camera around
		- PAN
			- Unimplemented
		- ZOOM
			- SCROLL UP/DOWN to zoom
			- Zooming manipulates the field of view to get proper
			depth. Not faking it with a camera translation

	Advanced Stage: 100%
		- Combined stage 1 and 2.
		- skeleton will follow the path at the speed defined
			by the splines 
		- Skeleton will look in the direction he is moving
		
2) List any additional modifications that are not in the brief:

	Controls are slightly different to the brief, as the brief
	requested controls that clashed between stages.

	(TIP: click on the window normally to get focus before trying
			any of the keyboard + mouse combination commands)

	Controls: 
		- SHIFT + LEFT CLICK on window to place a spline point.
			- If you click in the top left box, the point will 
				be added to the speed spline
			- If you click elsewhere, the point will be added to
				the trajectory spline.
		- PRESS 'c' to clear both the splines.

		- CTRL + LEFT CLICK to select a joint. You can click on the
			joint ball, or the bone itself.
		- RIGHT MOUSE DOWN + DRAG UP/DOWN to rotate the selected
			 joint
		- PRESS 'x' to cycle the active rotation axis of the 
			currently selected joint (Only cycles to axis that
			the dof of the joint allows)
		- Press 's' to save the current pose (in amc format)
			Prompted for filename on the command line

		- LEFT MOUSE DOWN + DRAG to rotate the camera
		- SCROLL UP/DOWN to zoom.

		- MIDDLE CLICK to bring up the menu. Menu commands are self
			explanitory and act on the speed of the animation.
			NOT the speed of the movement along the splines


	Extras:
		- Skeleton faces the right direction as he travels
			along the spline.
		- Skeleton lerps his position along the spline so we
			get a nice slide back to the start instead of an
			instant teleport.
		- Green diamond that travels along speed spline as a
			function of u f(u). The normalized y value at this
			point corresponds to the normalized arclength used
			to lookup the animated objects position.
		- The balls on both the 2D splines show how the green
			diamonds y maps to the spline through the arclength
			lookup.
		- The second ball on the trajectory spline is a point
			in front of the current point. The skeleton 
			'looks at' this point as he goes along the path
			so that he faces the right direction
		- The brief mentions that the speed curve should be a
			'monotonically increasing function'. But as t is a
			function of arc length, it doesn't matter if a point
			appears on the left or right of a previous point in
			out 2D panel view. As t will increas as a function of
			the length of the spline. So as long as our points are
			always increasing in the y direction on our panel, our
			speed curve will be a 'monotonically increasing
			function'. I allowed a little bit of play in this so
			that we are able to get 'over ease' effects, where you
			go slightly past your final resting point and then
			gently come back. I did lock placement of control
			points so that they must be greater in x and y on
			our 2D panel than any previous point. But I allowed
			the spline itself to bend slightly higher to get the
			over ease effects. I don't think that there actually
			needed to be any constraint on point placement as
			if you make the curve go up then down, it just means
			that as you traveled through time, you went along the
			curve and back again. t is never allowed to go in 
			reverse as it is a function of arclength. I put
			constraints on placing control points because I feel
			that is what the brief wanted even though it doesn't
			actually make any sense.


3) How to run your code, if it differs from the original assignment:

	#first make
	cd build && cmake ../work && make && cd ..
	
	#make
	cd ./build/ && make && cd ..

	#run
	./build/bin/a2 ./work/res/assets/priman.asf

	or

	./build/bin/a2 ./work/res/assets/priman.asf 
								./work/res/assets/all.cfg

								or

								./work/res/assets/walk.cfg