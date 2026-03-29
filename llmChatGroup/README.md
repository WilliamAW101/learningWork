# MIRA Chat Group

A multi-client AI-powered chat server written in C++20. Clients can chat with each other in real time, and query **MIRA** — a locally running LLM powered by [llama.cpp](https://github.com/ggerganov/llama.cpp) with CUDA GPU acceleration.

---

## Features

- Multi-client TCP chat server with concurrent connection handling
- Real time message broadcasting between all connected clients
- On-demand AI responses via MIRA with `Hey MIRA: <your message>`
- Local LLM inference using llama.cpp. No API key, no internet required
- CUDA GPU acceleration and tested on NVIDIA GTX 1650
- Non-blocking AI inference. LLM runs on a separate thread, chat continues uninterrupted

---

## Demo

```
Client 1:  Hey everyone
Client 2:  From Client 2 : Hey!
Client 1:  Hey MIRA: What is the capital of France?
MIRA:      The capital of France is Paris.
Client 2:  From Client 1 : Hey MIRA: What is the capital of France?
```

---

## Architecture

```
server binary
├── accepts incoming TCP connections
├── spawns a thread per client
├── broadcasts messages to all connected clients
└── routes "Hey MIRA:" messages to LLM on a separate thread

client binary
├── connects to server
├── listener thread — receives and prints incoming messages
└── main thread — reads user input and sends to server

LlamaModel
├── loads GGUF model into GPU VRAM via llama.cpp
├── formats prompt using Gemma chat template
└── generates response token by token
```

---

## Tech Stack

| Technology | Purpose |
|---|---|
| C++20 | Core language |
| POSIX Sockets | TCP networking |
| std::thread / std::mutex | Concurrency |
| std::atomic | Thread safe flags and counters |
| llama.cpp | Local LLM inference |
| CUDA | GPU acceleration |
| CMake | Build system |

---

## Requirements

- Linux (Ubuntu 24.04 recommended)
- NVIDIA GPU with CUDA support
- CUDA Toolkit 12.x+
- NVIDIA Driver 595+
- CMake 3.28+
- GCC 13+
- A GGUF model file (see below)

---

## Setup

### 1. Clone the repo

```bash
git clone --recurse-submodules https://github.com/WilliamAW101/learningWork.git
cd learningWork/llmChatGroup
```

### 2. Build llama.cpp with CUDA

```bash
cd ../llama.cpp
mkdir build && cd build
cmake .. -DGGML_CUDA=ON
make -j4
```

### 3. Download a model
```
There are multiple ways to download, I just use wgets.
```
### NOTE:
```
I am using my laptop for development, I plan on trying better models on my PC.
```
### 4. Update model path

In `src/chatServer/server.cpp`, update the model path:

```cpp
model = new LlamaModel("/path/to/your/models/gemma.gguf");
```

### 5. Build the project

```bash
mkdir build && cd build
cmake ..
make
```

### 6. Set library path

```bash
export LD_LIBRARY_PATH=/path/to/llama.cpp/build/bin:$LD_LIBRARY_PATH
```

Add to `~/.bashrc` to make it permanent.

---

## Running

### Start the server

```bash
./server
```

### Connect clients (open a new terminal for each)

```bash
./client
```

### Chat

```
Please enter a message (type exit to shutdown):
Hello everyone!

Please enter a message (type exit to shutdown):
Hey MIRA: Explain recursion in one sentence
```

MIRA will respond to all connected clients.

---

## Usage

| Input | Behavior |
|---|---|
| Any message | Broadcast to all other connected clients |
| `Hey MIRA: <message>` | Sends message to LLM, broadcasts response to all clients |
| `exit` | Disconnects the client |

---

### Further plans

```
I plan on adding more features like username, maybe hosting on server? Not sure, I am just playing around with LLMs, C++, and many other cool features.
```
