# Early-Bird APC Injection

This repository demonstrates **Early-Bird Asynchronous Procedure Call (APC) injection** on Windows, with a focus on **process creation timing** and **APC delivery behavior** during early process initialization.

The project explores how queuing an APC during the initial stages of a process—using the `DEBUG_PROCESS` creation flag—allows execution to occur **before the target process reaches its actual entry point**. This removes the dependency on a thread later entering an alertable state and highlights why timing is critical for APC-based techniques.

---

## Overview

Early-Bird APC injection leverages the Windows APC mechanism but differs from traditional APC injection by targeting execution **before normal user-mode code begins**. By intervening early in the process lifecycle, queued APCs can be delivered as execution starts, making this technique more reliable than standard APC injection.

This repository is intended to support:
- Understanding Windows process and thread initialization
- Studying APC scheduling and delivery mechanics
- Security research and detection engineering
- Analysis of why certain injection techniques succeed or fail

---

## High-Level Flow

1. A target process is created using debugging-related creation flags  
2. Memory is allocated within the target process  
3. A payload is written and prepared for execution  
4. An APC is queued to the primary thread early in execution  
5. When execution resumes, Windows delivers the APC before the real entry point  

---


## Screenshot / Diagram
<img width="1624" height="968" alt="Early Bird 1" src="https://github.com/user-attachments/assets/17516ef8-34a2-498b-b900-6b92a8a60da1" />



## Disclaimer

This project is provided **strictly for educational and defensive security research purposes**.  
It is intended to improve understanding of Windows internals and detection logic.  
Do not use this code in environments or contexts where you do not have explicit authorization.

---

## Feedback

This is an ongoing learning effort.  
Feedback, corrections, and discussion are welcome.
