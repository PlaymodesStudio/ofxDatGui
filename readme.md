## ofxDatGui_PM

### This version is a fork focused on changing the way key/mouse interaction is handled, we modify it to use it for our node-based framework [ofxOceanode](https://github.com/PlaymodesStudio/ofxOceanode) (where is a dependency). This are the main changes:
* Interaction based on events (on key press, on key release)
* Added right click event
* Added scrollViews inside a DatGui (not only as a separate component)
* Added hability to have multiple windows, and each window have it's own gui collection, and events listen on that specific window
* Added multislider: this is a kind of specific thing for ofxOceanode. It is a slider for vectors (float or int). The interaction is the same as a slider, the only diference is that it outputs a vector of the type with only one item. The special thing is that when you send to it a real vector it converts to a multivertical slider (without iteraction).
* Minor new methods like getters, updaters, setters.


### If you want to use this fork with your projects you have to change a small amount of code to work.
If you use a component ouside a gui, let's say`auto button = new ofxDatGuiButton("My Button");` and you want it to show up in the screen, before you were doing in update: `button->update();`, and in draw method `button->draw();`. We changed that so you only have to, is setup, do: `button->registerEvents();`.


-----------------------------


**ofxDatGui** is a **simple to use**, fully customizable, high-resolution graphical user interface for [openFrameworks](http://openframeworks.cc/) inspired by the popular JavaScript [datgui](http://workshop.chromeexperiments.com/examples/gui/) interface.  

![ofxDatGui](http://braitsch.github.io/ofxDatGui/img/ofxdatgui_.png "ofxDatGui")

## Features

**ofxDatGui** offers the following features & components:

* Click & Toggle (On/Off) Buttons
* Text Input Fields
* Color Pickers
* Range Sliders
* Dropdown Menus
* Scroll Views
* Button Matrices
* Coordinate Pads
* Value Plotters
* Waveform Monitors
* Framerate Monitor
* Variable Binding
* Completely Customizable
* Folders to group components together
* An optional header & footer that allow you to title the gui, collapse and drag it around

## Documentation

[The latest **ofxDatGui** docs are located here](http://braitsch.github.io/ofxDatGui/)

## ofParameter Support

**ofParameter** support is the single most requested feature and is currently in development. Therefore, **I am not accepting ofParameter related pull requests until the first stable version of ofxDatGui with ofParameter support is released.**

## Questions & Feedback

Questions, feedback, feature requests and ideas for improvement are all very much welcome. Please [open an issue](https://github.com/braitsch/ofxDatGui/issues) or join the conversation on this [openFrameworks thread](https://forum.openframeworks.cc/t/ofxdatgui-a-new-user-interface-for-of/20553) before sending me an email.

If you're enjoying ofxDatGui please star the repository or consider making a small donation to help support its ongoing development. If you've made something with it that you'd like to share [please let me know](https://twitter.com/braitsch) and I'll add it to the [gallery.](http://braitsch.github.io/ofxDatGui/gallery.html)
