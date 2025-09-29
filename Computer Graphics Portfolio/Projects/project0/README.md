# Project 0: Hello Graphics (20 points)

Project 0 is a simple project intended to make sure you have the Git repository and openFrameworks set up correctly on your machine, and that you're making progress through the tutorials in the textbook.

To receive credit for "Project 0", show the instructor that you have progressed through the Halladay text to at least the point of rendering a texture on screen (the equivalent of Figure 3-6 on page 48).  If you have time, it is recommended that you continue progressing through Halladay up to Listing 5-8 / Figure 5-3 on page 96.  You can get checked off for this project during class time or the instructor's office hours.  **You should also add me (@tetzlaff) as a member with "Reporter"-level (read-only) access to the repository and submit the URL to Canvas for the "Project 0" assignment.**

## Corrections and comments on Halladay chapters 1-4:

### Chapter 1:
* Page 8: Equation under second paragraph should read: (0, 3) * 5 = (0 * 5, 3 * 5) = (0, 15); incorrectly reads (1, 3) * ...
* Page 9: Equation near the bottom of the page should read: ... = (1/4, 2/5, 3/6) = (0.25, 0.4, 0.5); incorrectly reads (1/5, ...
* Page 10: Equations near the top should read: 
    * (1, 2, 3) * (4, 5, 6) = (1 * 4, 2 * 5, 3 * 6) = (4, 10, 18)
    * (1, 2, 3) / (4, 5, 6) = (1 / 4, 2 / 5, 3 / 6) = (0.25, 0.4, 0.5)

### Chapter 2:
* Page 22: pressing a key moves the bottom right vertex downward, not upward.
* Listing 2-11 and 2-13 have the signs flipped on the y-coordinates; listing 2-7 (and the GitHub repo) has it correct.

### Chapter 3:
* Figure 3-3, page 40: indices should be numbered 0, 1, 2, 3; not 1, 2, 3, 4, and should be numbered in counter-clockwise order starting from top-right.
* Page 44: Don't forget to modify the call to ```shader.load()``` in ```ofApp::setup``` to use the new shaders you wrote.
* Figure 3-10, page 56: Blue minus Green (bottom center in the table) should be (0.0, 0.0, 0.8), not (0.5, 1.0, 1.0).  The actual color shown appears to be correct (deep blue), just the numbers are wrong.

### Chapter 4:
This chapter assumes that you basically are starting from the project from chapters 1-3, but with some of the variable names changed.  Throughout the chapter, variable names seem to drift:
* ```greenMan``` sampler variable from listing 4-5 needs to get changed to ```tex``` for listing 4-7 to work.
* ```charShader``` C++ variable from listing 4-1 changes to ```shader``` in listing 4-7.
* ```charMesh``` from listing 4-1 changes to ```quad``` in listing 4-7.
* ```alienImg``` from listing 4-1 changes to ```img``` in listing 4-7.
* ```shader``` from listing 4-7 changes to ```alphaTestShader``` in listing 4-13
* Final draw() in listing 4-13 should be ```sunMesh.draw()``` instead of ```cloudMesh.draw()```. 

A few other things about chapter 4:

* I found that the best setup for ```charMesh``` to get it look good is width=0.1, height=0.2, offset=(0,-0.24,0).
* To get it to look exactly like Figure 4-4, you need to change alphaTest.frag to use something like ```outCol.a < 0.5``` instead of ```outCol.a < 1.0```  If you leave the threshold at 1.0, you may not even see the cloud at all.  Alternatively, you could use premultiplied alpha as I discuss in the final video for next week.
* Listing 4-16: Nothing technically wrong, but takes the prize for most examples of bad code style in a single code listing: use of a static local variable (it's better to just add another member variable to ```ofApp```), a conditional operator combined with += with side-effect (confusing), using C-style cast instead of ```static_cast```.
* If you get to the animated spritesheet, I found that it works better if you use (0.281, 0.1895) for the spriteSize, instead of (0.28, 0.19).
