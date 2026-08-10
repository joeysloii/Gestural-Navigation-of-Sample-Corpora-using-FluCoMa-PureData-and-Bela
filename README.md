# Gestural-Navigation-of-Sample-Corpora-using-FluCoMa-PureData-and-Bela
This project uses the Bela Gem Stereo interfaced with a hybrid C++/PureData script. The outcome of this project is a novel instrument which navigates a corpus of drum sounds in real time. FluCoMa is used heavily here for feature extraction, corpus building and regression algorithms.
## Contents 
### C++
- render.cpp - Custom Bela render for hybrid C++ and PureData operation.
- MadgwickAHRS.cpp - Source file containing Madgwick filter. Converts raw IMU data into orientation and acceleration about three axes.
- MadgwickAHRS.h - Header filer containing Madgwick filter.
### PureData (Online)
- _main_direct.pd - Main PureData patch to be ran on Bela. This is 1 of 2 patches that can be used as "_main.pd".
- _main_learned.pd - Main PureData patch to be ran on Bela. This is 2 of 2 patches that can be used as "_main.pd".
- sampler.pd - Abstraction used for cloning arrays to store samples.
### PureData (Offline)
- corpus_builder.pd - Patch used to arrange samples in a 2D space according to spectral features.
- gesture_vector_builder.pd - Patch used to record gestural data from Bela (regressor input).
- ground_truth_builder.pd - Patch used to record corpus transpositional data (regressor output).
- regressor_trainer.pd - Patch used to combine datasets and train a regressor.
### Python
- assign_identifiers.py - Creates .txt file containing file paths to samples alongside a unique identifier (for offline testing).
- assign_identifiers_bela.py - Creates .txt file containing file paths to samples alongside a unique identifier (for use on Bela).
### JSON/Text Peripherals 
- Samples.txt - Output from Python. Contains File paths and unique identifiers for each sample.  
- 2d_corp.json - Output from "corpus_builder.pd". Contains corpus coordinates and unique identifiers for each sample.
- master_input.json - "Output from regressor_trainer.pd". Merged dataset containing all gesture recordings used in "gesture_vector_builder.pd".
- master_output.json - "Output from regressor_trainer.pd". Merged dataset containing all transpositional data used in "ground_truth_builder.pd".
- input_extrema.json - "Output from regressor_trainer.pd". Normalisation extrema required so that input gesture vectors fit the regressor.
- output_extrema.json - "Output from regressor_trainer.pd". Normalisation extrema required so that output transpositional data can be de-normalised.
- learned_mappings.json - "Output from regressor_trainer.pd". Stores and loads the regressor curve.
### Audio Samples
- The example files used in the development of this system were from the "Thomas Penton Essential" drum sample pack.
## Prerequisites
To use the system as it is presented, you must have access to the Bela Gem Stereo with attached MPU6050 IMU (Accelerometer/Gyroscope module), PureData with the FluCoMa library installed, and Python. The FluCoMa library must also be declared on Bela after storing the files in a separate project within the Bela IDE. 
## Instructions for Use
### Building the Corpus
A corpus is an N-dimensional space which has been reduced from, usually, many more dimensions. In this example, 14 dimensions have been reduced to just two, which are used as coordinates in a 2D space. In this way, audio samples that are very similar to each other across all 14 dimensions can be positioned more closely to one another in the 2D space. 
1. First, decide which audio samples will be used. If using the "Thomas Penton Essential" collection, all you need to do is___________
2. Run the Python script "assign_identifiers.py" to choose any number of audio samples from your computer. Save the output .txt as "Samples.txt" on your desktop. Now all file paths have been saved with a unique identifier, so that samples can be traced throughout the following process.
3. Open "corpus_builder.pd" and follow the steps in order. Depending on the types of audio samples you are using, you may wish to modify the types of spectral features you are feeding into the algorithm. For example, the example patch uses spectral shape and chroma features, since this worked well for organising solely drum sounds. Other features such as MFCCs and loudness are also available should you wish to compare how these work. After this step you should have a file named "2d_corp.json", which saves and stores corpus information for later retrieval.
### Direct Mappings 
"Direct Mappings" take IMU data and "directly" map them to some transposition through the corpus of audio samples via some simple mathematical operation. This is the simplest way of navigating the corpus. 
1. Open up "_main_direct.pd" and explore the different sub-patches. You can change the BPM and sequence a different style of beat if you wish.
2. If you have built your own corpus, there are a few values you will need to update. Within "_main_direct.pd", open up the "pd sampler" sub patch and follow the instructions. Then open up "pd kick_voice", "pd snare_voice" and "pd hat_voice", and update the starting coordinates to positions that are better suited to your own corpus. 



