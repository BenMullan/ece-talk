// file:    electron-test/main.js - minimal electron demo
// exec:    npm i && npx electron-builder --win && .\dist\win-unpacked\electron-test.exe
// author:  Ben Mullan 2026

/*
    npx electron .
    npx electron-builder --win
    .\dist\win-unpacked\electron-test.exe --remote-debugging-port=9333
    npm i wscat -g
    npx wscat --connect "ws://127.0.0.1:9333/devtools/browser/<<<browser-guid>>>"
    npx wscat --connect "ws://127.0.0.1:8900/devtools/page/<<<target-hash>>>"
    curl localhost:9555/json | jq -r ".[0].webSocketDebuggerUrl" | clip
    npx @electron/fuses read --app .\electron-test.exe
*/

const electron = require("electron");

const createWindow = () => {

    const _browserWindow = new electron.BrowserWindow(
        {
            width: 800, height: 600,
            webPreferences: {
                nodeIntegration: true,        // whoops
                contextIsolation: false,      // risky
                enableRemoteModule: false
            }
        }
    );

    _browserWindow.loadFile("index.xhtml");

};

electron.app.whenReady().then(createWindow);