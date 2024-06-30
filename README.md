# Basic Interprocess Synchronization in QNX Neutrino RTOS

**by**: Min Khant Soe Oke, Kaung Sithu

## Overview

This project demonstrates various interprocess synchronization methods, specifically focusing on pulses and signals within the QNX Neutrino real-time operating system (RTOS). The project extends a distributed calculator system to include continuous function calculations (e.g., sine function) and explores non-blocking communication methods essential in real-time applications.

## Dependencies

**To run this project, you need to have the following dependencies installed:**

* QNX Neutrino RTOS
* Basic knowledge of C programming
* Familiarity with Unix-like systems, including processes, threads, and IPC

## Features

**Interprocess Communication (IPC)**
- Allows processes to communicate and synchronize their actions.
- Utilizes messages, pulses, and signals for IPC.

**Pulses**
- Lightweight messages that do not block the sender.
- Carry minimal data and allow for non-blocking notifications.

**Signals**
- Notify processes of events asynchronously.
- Can interrupt the target process to handle the event immediately.

**Non-Blocking Communication**
- Ensures processes do not get delayed waiting for a response.
- Crucial for maintaining system responsiveness and performance.

**Distributed Systems**
- Consist of multiple interacting processes, often spread across different nodes.
- Synchronization and communication between these processes are critical for system functionality and reliability.

## How to Use

1. **Start the Server**:
   - Open a terminal and run `./server`.

2. **Start the Sine Function Client**:
   - Open another terminal and run `./sine_with_pulse`.

3. **Start the Basic Operations Client**:
   - Open another terminal and run `./client`.

4. **Client-Server Interaction**:
   - The client will send requests for arithmetic operations or sine calculations to the server.
   - The server will delegate the tasks to the appropriate clients based on the operation.
   - The basic operations client will display the results received from the server.
   - The sine function client will send continuous sine calculations to the server as pulses.

## Authors

- Min Khant Soe Oke
- Kaung Sithu
