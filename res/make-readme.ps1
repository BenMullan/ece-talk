# File:     make-readme.ps1 - generates ..\readme.md, from .\slides\*.png
# Exec:     pwsh.exe -executionPolicy bypass -file make-readme.ps1
# Author:   Ben Mullan (c) 2026

@"
# embedded Chromium everywhere!
a security look at msedgewebview2 + CDP
<br/><br/>

> Watch the [**this talk on YouTube**](https://youtu.be/yLxGzo9OSo4)!
<a href="https://youtu.be/yLxGzo9OSo4" target="_blank">
    <img src="https://github.com/BenMullan/ece-talk/blob/main/res/ece-talk--youtube-thumb.png?raw=true" width="100%" />
</a>
<br/><br/>

hello! Millions of desktop applications (eg: zoom, steam, & vscode) ship a full Chromium browser with a debug-socket backdoor baked in. I examine how CDP — the protocol that powers devtools — creates some _minor_ weaknesses in Electron- and MsEdgeWebView2-based software. Live demo included!
<br/><br/>

## useful bits...
- [electron-test.exe](https://github.com/BenMullan/ece-talk/blob/main/src/electron-test/)
- [example-cdp-commands.txt](https://github.com/BenMullan/ece-talk/blob/main/src/example-cdp-commands.txt)
- [start-debuggable-chrome.cmd](https://github.com/BenMullan/ece-talk/blob/main/src/start-debuggable-chrome.cmd)
- [msedgewebview2-win32-cpp-demo/](https://github.com/BenMullan/ece-talk/blob/main/src/msedgewebview2-win32-cpp-demo/)
<br/><br/>

## proof-of-concept screenshots...
<img src="https://github.com/BenMullan/ece-talk/blob/main/res/demo--electron-code-injection-msgbox.png?raw=true" width="100%" />
<hr/><br/>
<img src="https://github.com/BenMullan/ece-talk/blob/main/res/demo--winword-addin-native-bridge-method-scrape-doc-text.png?raw=true" width="100%" />
<hr/><br/>
<img src="https://github.com/BenMullan/ece-talk/blob/main/res/demo--winword-addin-pane-matrix-animation.png?raw=true" width="100%" />
<br/><br/><br/>

## slides...
$(
    dir -path (join-path $psScriptRoot "slides") -filter "*.png" -file | sort { [int]($_.baseName -replace "\D+", "") } | % {
        "`n`n$("<br/>" * 2)`n<img src=""https://github.com/BenMullan/ece-talk/blob/main/res/slides/$($_.name)?raw=true"" width=""100%"" />`n`n#"
    }
)

"@ | out-file -filePath "$psScriptRoot\..\readme.md"