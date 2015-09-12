COMP308 Assignment 1

Name :: Hamish Rae-Hodgson
Student ID :: 300169925


1) List what you have completed:
	Core: 100%
		priman.asf renders properly.

	Completion: 100%
		(1) Arbitrary pose. Handstand.
		(2) Walking pose.
		(3) Seated pose. On the ground.
		(4) Can cycle through poses with 'p'.

	Challenge: 100%
		(1) Reads AMC files. Apply motion to priman. He moves.
		(2) Right click menu.
			Play (Plays animation at regular speed)
			Pause (Pauses animation, can be resumed or rewound from this frame)
			Stop (Stops animation playing and reverts to frame 0)
			Rewind (Plays at regular speed in reverse)
			Fast Forward (players forwards at 2x speed)

2) List any additional modifications that are not in the brief:
	
	Added 'Animations' submenu to the right click menu.
		This has many extra animations

	Added 'Poses' submenu to the right click menu.
		This has the 3 poses in it.
		It additionally has a "Captured" option, which will
		display the last "Capture" taken with the option in
		the main right click menu.

	Added 'Toggle Root Motion' to the right click menu.
		This toggles root motion on or off.
		Root motion will transform primans location in space
			during animation. This way, when an animation ends
			it loops from his current position instead of teleporting
			priman back to the starting position.
		Root motion works best on the 'Running' animation.
			As this animation (nearly) loops cleanly.
		You can root motion, in PLAY, REWIND, and FAST FORWARD modes.
		Root motion is automatically disabled when you load a new animation.

	Added 'Capture' button to the right click menu
		This dumps primans current rotation and translation information
			in AMC format to 'capture.amc' as a single frame. It was very 
			handy in creating the poses. It seemed like a waste of time to 
			manually hack at poses for him directly..


3) How to run your code, if it differs from the original assignment:

	#first make
	cd build && cmake ../work && make && cd ..
	
	#make
	cd ./build/ && make && cd ..

	#run
	./build/bin/a2 ./work/res/assets/priman.asf

	# You can then select animations from the right click submenu.
