# Gestural-Navigation-of-Sample-Corpora-using-FluCoMa-PureData-and-Bela
This project uses the Bela Gem Stereo interfaced with a hybrid C++/PureData script. The outcome of this project is a novel instrument which navigates a corpus of drum sounds in real time. FluCoMa is used heavily here for feature extraction, corpus building and regression algorithms.
###
Research by Joseph Mcsloy, MSc Sound and Music Computing at QMUL.
## Contents 
### C++
- render.cpp - Custom Bela render for hybrid C++ and PureData operation.
- MadgwickAHRS.cpp - Source file containing Madgwick filter by Per Tillisch [1]. Converts raw IMU data into orientation and acceleration about three axes.
- MadgwickAHRS.h - Header filer containing Madgwick filter by Per Tillisch [1].
### PureData (Runtime Patches)
- _main_direct.pd - Main PureData patch to be ran on Bela. This is 1 of 2 patches that can be used as "_main.pd".
- _main_learned.pd - Main PureData patch to be ran on Bela. This is 2 of 2 patches that can be used as "_main.pd".
- sampler.pd - Abstraction used for cloning arrays to store samples.
### PureData (Setup Patches)
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
To use the system as it is presented, you must have access to the Bela Gem Stereo with attached MPU6050 IMU (Accelerometer/Gyroscope module), PureData with the FluCoMa library installed, and Python. The FluCoMa library must also be declared on Bela after storing the files in a separate project within the Bela IDE. To do this, please follow the guidance given in this forum post. 
https://forum.bela.io/d/7831-compiling-flucoma-for-pd-on-bela-gem
## Instructions for Use
### Building the Corpus
A corpus is an N-dimensional space which has been reduced from, usually, many more dimensions. In this example, 14 dimensions have been reduced to just two, which are used as coordinates in a 2D space. In this way, audio samples that are very similar to each other across all 14 dimensions can be positioned more closely to one another in the 2D space. 
1. First, decide which audio samples will be used. If using the "Thomas Penton Essential" collection [2], all you need to do is follow the steps in the "Bela Setup" section below. 
2. Run the Python script "assign_identifiers.py" to choose any number of audio samples from your computer. Save the output .txt as "Samples.txt" on your desktop. Now all file paths have been saved with a unique identifier, so that samples can be traced throughout the following process.
3. Open "corpus_builder.pd" and follow the steps in order. Depending on the types of audio samples you are using, you may wish to modify the types of spectral features you are feeding into the algorithm. For example, the example patch uses spectral shape and chroma features, since this worked well for organising solely drum sounds. Other features such as MFCCs and loudness are also available should you wish to compare how these work. After this step you should have a file named "2d_corp.json", which saves and stores corpus information for later retrieval.
### Direct Mappings 
"Direct Mappings" take IMU data and "directly" map them to some transposition through the corpus of audio samples via some simple mathematical operation. This is the simplest way of navigating the corpus. 
1. Open up "_main_direct.pd" and explore the different sub-patches. You can change the BPM and sequence a different style of beat if you wish.
2. If you have built your own corpus, there are a few values you will need to update. Within "_main_direct.pd", open up the "pd sampler" sub patch and follow the instructions. Then open up "pd kick_voice", "pd snare_voice" and "pd hat_voice", and update the starting coordinates to positions that are better suited to your own corpus.
3. You may need to adjust the scaling of IMU data inside "pd Direct_Mappings" if your corpus is large/small. 
### Learned Mappings 
"Learned Mappings" come from a learned relationship between input gestural vectors and output corpus transpositional data. The relationships learned using the "Thomas Penton" samples can, in theory, be used with any set of samples. However, if you need to retrain the learned mappings, follow the steps below. 
1. Open up "gesture_vector_builder.pd" offline and read the instructions carefully. 
2. Load "gesture_vector_builder.pd" onto the Bela IDE and rename it to "_main.pd".
3. Ensure you have followed the relevant stages from the "Bela Setup" section below.
4. Every time you run the code in the IDE, a new dataset will be recorded. These will be merged later.
5. Open up "ground_truth_builder.pd" offline and read the instructions carefully.
6. Every time you run the patch, a new dataset will be recorded. These will be merged later.
7. Open up "regressor_trainer.pd". This is for merging datasets and training the regressor.
8. Read the instructions carefully. After this step, you should have master_input.json, master_output.json, input_extrema.json, output_extrema.json, and learned_mappings.json.
9. Open up "_main_learned.pd" and explore the different sub-patches. You can change the BPM and sequence a different style of beat if you wish.
10. If you have built your own corpus, there are a few values you will need to update. Within "_main_learned.pd", open up the "pd sampler" sub patch and follow the instructions. Then open up "pd kick_voice", "pd snare_voice" and "pd hat_voice", and update the starting coordinates to positions that are better suited to your own corpus.
### Bela Setup 
These are instructions on how to setup the Bela IDE.
1. For FluCoMa objects to work inside Bela, follow the steps here https://forum.bela.io/d/7831-compiling-flucoma-for-pd-on-bela-gem.
2. Drag and drop the following files from your desktop into your IDE project:
- render.cpp
- MadgwickAHRS.cpp/MadgwickAHRS.h
- _main_direct.pd OR _main_leanred.pd
- sampler.pd
- Samples.txt
- 2d_corp.json
- input_extrema.json/output_extrema.json
- learned_mappings.json
2. Create a subfolder in the IDE called "Samples" and put all audio samples in this folder. 
3. Rename "_main_direct.pd" or "_main_learned.pd" to "_main.pd".

# References 
[1] Madgwick filter by Per Tillisch - https://github.com/arduino-libraries/MadgwickAHRS/tree/master 

[2] Samples by Thomas Penton - https://www.loopmasters.com/genres/25-House/products/423-Thomas-Penton-Essential-Series-Vol1?srsltid=AfmBOor347_xAlOUgiidKSEqpVj8U1eVslKIVlIJjHfB9ACv3c0IsHkQ




