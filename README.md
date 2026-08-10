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
