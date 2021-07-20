# UberSaw Version 1.1
UberSaw is a custom oscillator developed for the [Korg NTS-1 Nu:Tekt digital synthesizer](https://www.korg.com/us/products/dj/nts_1/) with the [logue-sdk](https://github.com/korginc/logue-sdk). This is an emulation of the Roland Super-Saw which featured on the JP-8080 and was developed with reference to the research carried out by Adam Szabo in 2010, and Alex Shore in 2013.

The oscillator features:
- Super-Saw oscillator (1 central + 6 side oscillators).
- Mix control for Super-Saw (LFO modulated)
- Super-Saw detune control
- 2 tunable secondary oscillators
- Individual mix controls for secondary oscillators
- Chord selection for secondary oscillators
- Ring modulation control
- Phase drift control
- LFO frequency and shape control

The detune values are calculated using a detuine curve developed by Adam Szabo. Version 1.1 now introduces a detune value lookup table to reduce processor load. tHe mix control for the main oscillators allows the user to set the relative volume of the side oscillators to the central and can be modulated with an LFO.

The 2 secondary oscillators are linked to the pitch of the central oscillator and can now be tuned using the chord selection control to produce octave, fifth, major third, or minor third chords. Both of these secondary oscilators have their own mix controls. This provides a volume range from inaudible to dominant in the output signal. 

The phase drift control allows the user to add a offest phase offset to the side and secondary oscillators.

The ring modulation feature utilises the secondary oscillators to create the effect and can be set from inaudible to dominant in the output signal.


Oscillator Audio Sample (Version 1.0):
https://soundcloud.com/graham-james-keane/ubersaw-demo-nts-1

Build Video: (Version 1.0)
https://youtu.be/s0rdvLjd52w

Build Article (Version 1.0):
https://korgnts1beginnersguide.wordpress.com/?p=499

Project Website:
https://korgnts1beginnersguide.wordpress.com/

## Controls (Version 1.1)

![ubersaw controls](https://user-images.githubusercontent.com/40239414/126343237-40fc2843-9fc3-4291-b6ff-8fc86e0a1dd1.png)


## NTS-1 Installation Instructions 

### Directly Loading the Unit File
You can download the [ubersaw.ntkdigunit](https://github.com/GrahamJamesKeane/UberSaw/blob/main/ubersaw_v1.1/ubersaw.ntkdigunit) file and load it directly to your NTS-1 using the [librarian](https://korgnts1beginnersguide.wordpress.com/2021/07/06/compiling-and-loading-our-first-custom-project-the-waves-demo/2/) application or [logue-CLI](https://korgnts1beginnersguide.wordpress.com/2021/07/06/compiling-and-loading-our-first-custom-project-the-waves-demo/3/) utility.

### Build the Project Yourself
Alternatively, you can [rebuild](https://korgnts1beginnersguide.wordpress.com/2021/07/06/compiling-and-loading-our-first-custom-project-the-waves-demo/) the project. To do so, clone either [version 1.0](https://github.com/GrahamJamesKeane/UberSaw/tree/main/ubersaw_v1.0) or [version 1.1](https://github.com/GrahamJamesKeane/UberSaw/tree/main/ubersaw_v1.1) and run the Makefile via MSYS (Windows 10). I have provided [tutorials](https://korgnts1beginnersguide.wordpress.com/setting-up-the-development-environment/) on the set-up and use of the various tools you'll need to do this on the project website.

## Other Platforms
This oscillator was designed specifically for the Nu:Tekt NTS-1. The program has been tested on the Minilogue XD, and while the program functions its behaviour is not as intended. Based on this I currently recommend that this program only be used on the NTS-1 Nu:Tekt.

Note: This is currently untested on the Prologue.
