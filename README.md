# CSCI 5229 - FINAL PROJECT

# Igor Overchuk

I have taken the application I have developed up to hw6 and continued expanding it.

This project will build 3 executables: final, mapGenerator, debug
`final` is the main game executable, it will load you into the world where you can run around, shoot enemies and explore. Use the `-f` command line flag to go full screen and the `-d` command line flag to enable debug/dev mode to be able to use additional keys to manipulate the environment. \
`mapGenerator` is the map generator that creates the elevation map and places objects. FYI closing the window with ESC will save any changes you make to the map to the the dem files. You can close the window with your mouse to not save changes. \
`debug`: object viewer to help me create individual objects and make sure their normals are correct.

### My Most Prominent Achievements
**The day night cycle.** I hope to elevate it even further with the additional lighting.\
**The weapon mechanics**, being able to run around and shoot targets.\
**The map generator.** It started as a basic elevation editor and became a full blown map creator where I could make any map I want fairly quickly.\
**The scale of the scene.** I have built a fairly large world and was able to have a lot of objects such as trees around the map thanks to the optimization of display lists.\
**The dynamic flyovers of the helicopters.** Having those flying around adds vibrancy to the scene.\
**Custom Shader Implementation.** Writing my own shaders to perform custom lighting in the form of the flashlight, as well as directional light from the sun.

### Things I failed to Achieve:
I was not able to create the lamps as point lights around the town. My main issue was that there was no easy way to get the vertex's location in world coordinates in the shader, everything was in view points since there is only a gl_ModelView matrix and not a gl_Model matrix. I believe I found some methods to explicitly pass the model matrix as a uniform to the vertex shader but did not have time to implement that solution while also placing additional lights around the map. I believe I still achieved my stretch goal of shaders, but did come up slightly short on my last main goal.

### How to Get Complete Overview of Scene
In the main `final` executable, click `m` to switch to overview mode. Use arrow keys to elevate camera above ground level and `</,` to zoom out. If you zoom far enough you will exit the sky dome.

### Code Re-use
`ex13`: for debug executable bullet object\
`ex18`: for map editor to drag map around and reading/saving from/to dem file
`https://learnopengl.com/Lighting/Light-casters`: for reference on how to write shaders for the various lights I used

### Note
I capture the mouse in first person mode. In order to not have it captured, click `m` to enter overview mode.

### Key bindings (final executable):
`WASD/wasd`: Move around in first person view\
`left mouse click`: Shoot\
`mouse`: move camera around in first person mode (move your mouse over any part of the window to initiate this)\
`SPACE`: jump in first person mode\
`SHIFT`: sprint in first person mode\
`f/F`: toggle flashlight on and off\

In debug mode with `-d` flag:\
`arrows`: Change view angle in perspective overview\
`0`: Reset view angle\
`ESC`: Exit\
`x/X`: toggle axes/view angle text\
`m`: cycle through view modes (perspective overview/perspective first person)\
`p`: pause/resume lighting motion\
`k/K`: slow light cycle / speed up light cycle\
`./>`: zoom in in perspective view\
`,/<`: zoom out in perspective view

### Key bindings (mapGenerator executable):
`m`: cycle modes (map, tree, house, enemy)\
`c`: cycle iterations of "selected" object\
`wasd/WASD`: move brush around (in map), move object around (in tree, house, enemy)\
`click/drag`: move around the map by clicking left mouse and dragging it around\
`arrows`: change view angle\
`ESC`: exit and save (will overwrite current dem files)\
`b/B`: increase/decrease brush size in map editing mode\
`e/E`: increase/decrease elevation/move step in map/objects editing modes\
`,/<`: zoom out\
`./>`: zoom in\
`u`: increase elevation at selected point/s in map edit mode\
`j`: decrease elevation at selected point/s in map edit mode\
`r`: rotate objects\
`Z`: randomize tree heights

### Key bindings (debug executable):
Same key bindings as ex13:\
`l`: Toggles lighting\
`a/A`: Decrease/increase ambient light\
`d/D`: Decrease/increase diffuse light\
`s/S`: Decrease/increase specular light\
`e/E`: Decrease/increase emitted light\
`n/N`: Decrease/increase shininess\
`F1`: Toggle smooth/flat shading\
`F2`: Toggle local viewer mode\
`F3`: Toggle light distance (1/5)\
`F8`: Change ball increment\
`F9`: Invert bottom normal\
`m`: Toggles light movement\
`[]`: Lower/rise light\
`p`: Toggles ortogonal/perspective projection\
`o`: Cycles through objects\
`+/-`: Change field of view of perspective\
`x`:  Toggle axes\
`arrows`: Change view angle\
`PgDn/PgUp`: Zoom in and out\
`0`: Reset view angle\
`ESC`: Exit
