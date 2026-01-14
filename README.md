This repository demonstrates an Early-Bird Asynchronous Procedure Call (APC) injection technique on Windows, focusing on process creation timing and APC delivery behavior.

The implementation explores how queuing an APC during the early stages of process initialization, using the DEBUG_PROCESS creation flag, allows execution to occur before the target process reaches its real entry point. This approach avoids reliance on a thread later entering an alertable state and highlights why execution timing is critical to APC-based techniques.

The project is intended for educational and defensive security research purposes, with emphasis on:

Windows process and thread initialization

APC scheduling mechanics

Memory allocation and protection transitions

Understanding why certain injection techniques succeed or fail in practice

This code is provided to support learning, analysis, and detection engineering—not for misuse.
