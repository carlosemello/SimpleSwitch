# SimpleSwitch 
This class manages all house keeping (tracking state, debouncing, etc.) for
a switch to be used in an Arduino project. It was originaly meant for use with
Footswitches in my own projects, but can be applied to any simple electro-mechanical 
momentary switch. The class implements software-side toggle action, so we can use 
simple momentary switches as on/off or even multi state "circular" switches.

Each instance of the SimpleSwitch class controls a single physical switch,
which should be connected to one of the digital i/o pins.The switch can 
be of 3 types: Momentary, Toggle or Circular as explained in the header file:
