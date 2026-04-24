Here is the unified, machine-readable Product Requirements Document (PRD) for the Nadeem platform. This document is structured hierarchically to provide clear constraints, data models, API contracts, and user flows, making it optimal for an agentic AI to consume for code generation, architecture planning, and test writing.

# **Unified System Requirements Document: Nadeem Platform**

## **1\. System Overview & Architecture**

Nadeem (نديم) is an NFC-enabled smart audio device for children in the Arab region that plays local audio or initiates AI-driven conversations via physical figurines (Semsems). The system comprises four distinct components:

1. **Firmware (ESP32):** The physical device acting as a dumb audio/WebSocket pipe.  
2. **Cloud Backend:** The central API server managing authentication, device bootstrapping, audio files, and AI conversations.

3. **Web Frontend (Parental Dashboard):** A portal for parents to manage devices, view libraries, and audit AI chats.  
4. **Mobile Companion App:** A single-purpose application used strictly for provisioning the hardware to the local Wi-Fi and binding it to a user account.

### **1.1 Technical Stack Constraints**

* **Backend:** Python/Django with PostgreSQL. Must expose REST APIs and WebSocket endpoints.

* **Frontend (Web & Mobile-Web):** Next.js (React) with Tailwind CSS.

* **Infrastructure:** Fully Dockerized (Backend, DB, Redis) with docker-compose scripts for local dev and production. TLS is mandatory in production.

* **Localization (Frontend):** Strictly Arabic (AR). Must enforce Right-to-Left (RTL) layout using dir="rtl".  
* **Typography (Frontend):** 'Baloo Bhaijaan 2' (headings/logos), 'Cairo' or 'Tajawal' (body text).

## ---

**2\. Core Data Models (Target Schema)**

An agentic AI should map these models to database schemas (e.g., Django ORM).

* **User (Parent):** Email, Password Hash, Account Settings.  
* **Device:** device\_id (12 hex chars, STA MAC), device\_token (opaque, revocable), User (Foreign Key), online status.  
* **Semsem (Physical NFC Tag):** uid\_hex (NFC UID string), Title, is\_pro (boolean). Must store and show info for each Nadeem device.

* **Manifest (Semsem Metadata):**  
  * *Regular:* pro=false, list of track objects (name, url).  
  * *Pro:* pro=true, role (string, max 31 bytes, e.g., "doctor").  
* **Usage Stats:** Aggregated metrics per uid\_hex per device\_id. Fields: play\_count, total\_play\_ms, last\_played\_unix, pro\_session\_count, pro\_total\_ms.  
* **Pro Chat Session:** session\_id, device\_id, uid\_hex, Start/End Time, Transcript (Array of \[speaker, text, timestamp\]).

## ---

**3\. System Workflows**

### **3.1 Provisioning & Bootstrapping (Device Binding)**

This is a secure handoff to bind a physical MAC address to a User account.

1. **App \-\> Backend:** User logs in. App requests POST /api/provisioning-tokens. Backend returns a single-use provision\_token (10-min TTL).  
2. **App \-\> Firmware (Local SoftAP 192.168.4.1):** App sends POST /provision containing ssid, password, and provision\_token. Firmware saves Wi-Fi credentials and reboots.  
3. **Firmware \-\> Backend:** Firmware connects to LAN, calls POST /bootstrap with provision\_token.  
4. **Backend Logic:** Validates provision\_token, binds device\_id to User, invalidates provision\_token, and returns a persistent device\_token.

### **3.2 AI Conversation (Semsem Pro)**

1. Firmware reads NFC UID and calls GET /semsem/{uid\_hex}/manifest.  
2. Backend returns {"pro": true, "role": "doctor"}.  
3. Firmware opens WebSocket to wss://\<host\>/chat?role=doctor\&device=\<id\>\&semsem=\<uid\>. (Must pass Authorization: Bearer \<device\_token\> in header or query).  
4. **Audio Stream:** Firmware streams 16kHz mono PCM16 audio (20ms frames / 640 bytes). Backend processes VAD/ASR/LLM/TTS and streams PCM16 audio back.  
5. **Control Frames:** Backend optionally sends JSON text frames {"status": "listening"} or {"status": "speaking"} to drive physical device LEDs.

## ---

**4\. API Contract Specifications**

### **4.1 Firmware ↔ Backend Integration (Device API)**

All endpoints except /bootstrap require Authorization: Bearer \<device\_token\>. All device payloads are application/json.

| Method | Endpoint | Description | Payload / Response |
| :---- | :---- | :---- | :---- |
| **POST** | /bootstrap | Exchanges token. | **Req:** {"provision\_token": "..."} **Res (200):** {"device\_token": "..."} |
| **GET** | /semsem/{uid\_hex}/manifest | Fetches config. | **Res (200):** { "pro": bool, "role"?: string, "tracks"?: \[{"name", "url"}\] } |
| **POST** | /stats | Uploads counters. | **Req:** { "uid", "play\_count", "total\_play\_ms", "last\_played\_unix", "pro\_session\_count", "pro\_total\_ms" } **Res (200)** |
| **WS** | /chat | Live AI Audio Pipe. | **Query:** ?role=\&device=\&semsem= **Transport:** Binary frames (PCM16), Text frames (Control). |

### **4.2 Web Frontend ↔ Backend Integration (App API)**

Used by the Next.js Parental Dashboard. Requires standard User authentication (JWT or Session).

| Method | Endpoint | Purpose | View Mapping |
| :---- | :---- | :---- | :---- |
| **POST** | /api/auth/login | User Auth. | Authentication Screen |
| **POST** | /api/provisioning-tokens | Generates token for App. | Main Dashboard ("Add Device") |
| **GET** | /api/users/me/stats | Aggregated global stats. | Main Dashboard (Stats Cards) |
| **GET** | /api/semsems | Lists all bound UIDs. | Semsem Library (Grid View) |
| **GET** | /api/semsems/\<uid\_hex\> | Manifest data details. | Semsem Details View |
| **GET** | /api/chat-sessions/ | Chronological list of AI chats. | Pro Chat History |
| **GET** | /api/chat-sessions/\<id\> | Full transcript of specific chat. | Pro Chat History Drill-down |
| **GET** | /api/devices/ | List bound devices. | Device Management |
| **DELETE** | /api/devices/\<id\> | Revokes device token. | Device Management (Security) |

## ---

**5\. Web Frontend Implementation Directives**

When generating the Next.js frontend code, adhere to the following mappings:

* **Main Dashboard:** Must aggregate data from /api/users/me/stats and /api/devices/. Display total listening hours, unique Semsems owned, AI conversation minutes, and device online status.  
* **Library Component:** Map data from /api/semsems. Create visual badging logic checking the is\_pro boolean to render either an "عادي" (Regular) badge or a "ذكي" (Pro) badge.  
* **Chat History Auditing:** Map /api/chat-sessions/\<id\>. UI must visually distinguish between the "Child" (user transcribed audio) and the "AI" (Semsem responses) using a chat-bubble layout.  
* **Security Restrictions:** The UI must provide a mechanism to execute DELETE /api/devices/\<id\> to immediately revoke tokens for lost or stolen hardware.