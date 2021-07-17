# UberSaw
UberSaw is a custom oscillator developed for the [Korg NTS-1 Nu:Tekt digital synthesizer](https://www.korg.com/us/products/dj/nts_1/) with the [logue-sdk](https://github.com/korginc/logue-sdk). This is an emulation of the Roland Super-Saw which featured on the JP-8080 and was developed with reference to the research carried out by Adam Szabo in 2010, and Alex Shore in 2013.

Oscillator Audio Sample:
https://soundcloud.com/graham-james-keane/ubersaw-demo-nts-1

Project Website:
https://korgnts1beginnersguide.wordpress.com/

Build Video:
https://youtu.be/s0rdvLjd52w

Build Article:
https://korgnts1beginnersguide.wordpress.com/?p=499

## Controls

![ubersaw_controls](https://user-images.githubusercontent.com/40239414/125534308-43eb3182-7a24-4f6b-8193-b27a17d7c3cb.png)

## NTS-1 Installation Instructions 

### Directly Loading the Unit File
You can download the [ubersaw.ntkdigunit](https://github.com/GrahamJamesKeane/UberSaw/blob/main/ubersaw.ntkdigunit) file and load it directly to your NTS-1 using the [librarian](https://korgnts1beginnersguide.wordpress.com/2021/07/06/compiling-and-loading-our-first-custom-project-the-waves-demo/2/) application or [logue-CLI](https://korgnts1beginnersguide.wordpress.com/2021/07/06/compiling-and-loading-our-first-custom-project-the-waves-demo/3/) utility.

### Build the Project Yourself
Alternatively, you can [rebuild](https://korgnts1beginnersguide.wordpress.com/2021/07/06/compiling-and-loading-our-first-custom-project-the-waves-demo/) the project. To do so, clone the repo and run the Makefile via MSYS (Windows 10). I have provided [tutorials](https://korgnts1beginnersguide.wordpress.com/setting-up-the-development-environment/) on the set-up and use of the various tools you'll need to do this on the project website.

## Other Platforms
This oscillator was designed specifically for the Nu:Tekt NTS-1. The program has been tested on the Minilogue XD, and while the program functions its behaviour is not as intended. Based on this I recommend that this program only be used on the NTS-1 Nu:Tekt.

### Minilogue XD 
~~Note: Some users report that the Minilogue freezes when a note is played.~~

~~Update: Minilogue test version is working, I have updated the main files. I need to confirm it is working as expected. The detune control doens't appear to be having the desired effect.~~

Update: This program does not function correctly on the Minilogue XD,

### Prologue XD 
If you intend to load this program to the Prologue please use the [ubersaw.prlgunit](https://github.com/GrahamJamesKeane/UberSaw/blob/main/ubersaw_PRO/ubersaw.prlgunit) located in the Prologue-specific build [folder](https://github.com/GrahamJamesKeane/UberSaw/tree/main/ubersaw_PRO).

Note: This is currently untested on the Prologue.
