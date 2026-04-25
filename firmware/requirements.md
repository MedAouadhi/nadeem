I need to get the requirements document for the firmware of an electronic product I am developing based on 
https://www.waveshare.com/wiki/ESP32-S3-AUDIO-Board devkit, with the nfc module v3 added to it via wires https://www.elechouse.com/elechouse/images/product/PN532_module_V3/PN532_%20Manual_V3.pdf.

The product is destined for the arab region and is called "Nadeem, نديم" and it is a audio box for kids that plays music or audiobooks depending on the rfid object that is placed on top of it, similar to Toniebox.

The rfid toys are called "سماسم" samasem, single is سمسم  (semsem).

each semsem corresponds to a folder (a list of audio files to be played)

There are special semsem that once placed will activate a chatbot mode in which the kid will have a conversation with the semsem which will be a (doctor, engineer ..) so that the kid benefits from it.

The firmware high level architecture should have these modules/components: 

- An onboarding module, that serves a webserver so that the user can connect to it using a dedicated app and configure the wifi, as well as receiving bootstrapping information to allow the nadeem to communicate with the nadeem server to receive the audio/ other information.

- Audio module, plays the given audio files based on the received events (play , stop, next, previous)

- Semsem pro: the module that takes care of having a ai conversation and having the audio stream with the backend (input from the microphone )

-  Semsem : a module that detects the placement/removal of a semsem , and gets the semsem id and queries the sdcard if the corresponding audio file(s) exist locally, otherwise requests the audio from the backend, also stores the data (the progress of each audio track within each semsem list, for it to be resumed later)

-  a hmi module: that takes care of getting the inputs from the nfc module and buttons to generate events to send to other modules (semsem present, semsem removed, play pressed, next pressed, ....)

-  led strip module: gets the events from the other modules to show the current mode (6 rgb leds on a circular strip)
---

The modules should use the smf state machine components to have the modules use the state machine pattern for ease of readability and maintenance.

use the best practices of esp-idf and freertos and the code should be optimized for human understanding and readability.

The components should keep statistics about the usage of the semsems so that they can be used for parental control and to enhance the product.

The app should have a console  for the enhancement of the developer experience and commands for most of actions of the system so that the developer can test /debug the features.
