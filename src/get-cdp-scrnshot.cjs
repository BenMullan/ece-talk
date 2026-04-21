// file:    get-cdp-scrnshot.cjs
// usage:   node get-cdp-scrnshot.cjs "ws://localhost:9555/devtools/page/..."
// author:  Ben Mullan + Gemini 2026

const http      = require("node:http");
const fs        = require("node:fs");
const path      = require("node:path");
const crypto    = require("node:crypto");

const wsUrl = process.argv[2];
if (!wsUrl) {
    console.error("Usage: node get-cdp-scrnshot.js <ws_url>");
    process.exit(1);
}

const targetDir = __dirname;
const url = new URL(wsUrl.replace("ws://", "http://")); 

const options = {
    port: url.port || 80,
    host: url.hostname,
    path: url.pathname,
    headers: {
        "Connection": "Upgrade",
        "Upgrade": "websocket",
        "Sec-WebSocket-Key": crypto.randomBytes(16).toString("base64"),
        "Sec-WebSocket-Version": "13"
    }
};

const req = http.request(options);

req.on(
    "error",
    (err) => {
        console.log("HTTP request error:", err.message);
        process.exit(1);
    }
);

req.on(
    "upgrade",
    (res, socket) => {
        console.log("[+] Connected to CDP Target");

        // 1. Construct the CDP Payload
        const payload = JSON.stringify({
            id: 1,
            method: "Page.captureScreenshot",
            params: { format: "png", fromSurface: true }
        });

        // 2. Wrap in a WebSocket Frame (Fin bit + Text opcode + Masking)
        // For a simple demo, we send a basic frame.
        const buf = Buffer.from(payload);
        const frame = Buffer.alloc(buf.length + 6);
        frame[0] = 0x81; // Fin bit set, Opcode 1 (text)
        frame[1] = 0x80 | buf.length; // Mask bit set + payload length
        const mask = crypto.randomBytes(4);
        mask.copy(frame, 2);
        for (let i = 0; i < buf.length; i++) {
            frame[6 + i] = buf[i] ^ mask[i % 4];
        }

        socket.write(frame);

        // Timeout if no response
        setTimeout(() => {
            console.log("Timeout: no response received within 10 seconds");
            process.exit(1);
        }, 10000);

        socket.on("error", (err) => {
            console.log("Socket error:", err.message);
            process.exit(1);
        });

        // 3. Listen for the response
        socket.on("data", (data) => {
            console.log("Received data length:", data.length);
            console.log("Data as hex:", data.toString("hex").substring(0, 100));
            // Convert to string and find the JSON start
            const rawString = data.toString("utf8");
            const start = rawString.indexOf("{");
            if (start === -1) {
                console.log("No JSON start { found in data");
                process.exit(1);
            }
            const jsonStr = rawString.substring(start);
            console.log("JSON string:", jsonStr.substring(0, 200));
            try {
                const json = JSON.parse(jsonStr);
                console.log("Parsed JSON id:", json.id, "has result:", !!json.result);
                if (json.result && json.result.data) {
                    const filename = `screenshot_${Date.now()}.png`;
                    const filePath = path.join(targetDir, filename);
                    
                    fs.writeFileSync(filePath, json.result.data, "base64");
                    console.log(`[!] Screenshot saved to: ${filePath}`);
                    process.exit(0);
                } else {
                    console.log("No result.data in response");
                    process.exit(1);
                }
            } catch (e) {
                console.log("JSON parse error:", e.message);
                process.exit(1);
            }
        });
    }
);

req.end();