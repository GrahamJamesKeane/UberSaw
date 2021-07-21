# UberSaw Version 1.1
UberSaw is a custom oscillator developed for the [Korg NTS-1 Nu:Tekt digital synthesizer](https://www.korg.com/us/products/dj/nts_1/) with the [logue-sdk](https://github.com/korginc/logue-sdk). This is an emulation of the Roland Super-Saw which featured on the JP-8000 and was developed with reference to the research carried out by Adam Szabo in 2010, and Alex Shore in 2013.

## 1 - Overview
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

## 2 - Updates
The detune values are calculated using a detune curve developed by Adam Szabo. Version 1.1 now introduces a detune value lookup table to reduce processor load. The mix control for the main oscillators allows the user to set the relative volume of the side oscillators to the central oscillator and can be modulated with an LFO.

The 2 secondary oscillators are linked to the pitch of the central oscillator and can now be tuned using the chord selection control to produce octave, fifth, major third, or minor third chords. Both of these secondary oscilators have their own mix controls. This provides a volume range from inaudible to dominant in the output signal. 

The phase drift control allows the user to add a offest phase offset to the side and secondary oscillators.

The ring modulation feature is linked to the secondary oscillators and can be set from inaudible to dominant in the output signal.


**Oscillator Audio Sample (Version 1.0):**
https://soundcloud.com/graham-james-keane/ubersaw-demo-nts-1

**Build Video: (Version 1.0):**
https://youtu.be/s0rdvLjd52w

**Build Article (Version 1.0):**
https://korgnts1beginnersguide.wordpress.com/?p=499

**Project Website:**
https://korgnts1beginnersguide.wordpress.com/

## 3 - Controls (Version 1.1)

![ubersaw controls 2 1](https://user-images.githubusercontent.com/40239414/126568507-7c4d1e99-f671-4021-91cf-c74cdcc5e640.png)


## 4 - NTS-1 Installation Instructions 

### 4.1 - Directly Loading the Unit File
You can download the [ubersaw.ntkdigunit](https://github.com/GrahamJamesKeane/UberSaw/blob/main/ubersaw_v1.1/ubersaw.ntkdigunit) file and load it directly to your NTS-1 using the [librarian](https://korgnts1beginnersguide.wordpress.com/2021/07/06/compiling-and-loading-our-first-custom-project-the-waves-demo/2/) application or [logue-CLI](https://korgnts1beginnersguide.wordpress.com/2021/07/06/compiling-and-loading-our-first-custom-project-the-waves-demo/3/) utility.

### 4.2 - Build the Project Yourself
Alternatively, you can [rebuild](https://korgnts1beginnersguide.wordpress.com/2021/07/06/compiling-and-loading-our-first-custom-project-the-waves-demo/) the project. To do so, clone either [version 1.0](https://github.com/GrahamJamesKeane/UberSaw/tree/main/ubersaw_v1.0) or [version 1.1](https://github.com/GrahamJamesKeane/UberSaw/tree/main/ubersaw_v1.1) and run the Makefile via MSYS (Windows 10). I have provided [tutorials](https://korgnts1beginnersguide.wordpress.com/setting-up-the-development-environment/) on the set-up and use of the various tools you'll need to do this on the project website.

## 5 - Other Platforms
This oscillator was designed specifically for the Nu:Tekt NTS-1. 

Version 1.0 was tested on the Minilogue and while the program functions its behaviour is not as intended. This version is untested on the Prologue.

Version 1.1 is untested on both the Minilogue XD and Prologue.
