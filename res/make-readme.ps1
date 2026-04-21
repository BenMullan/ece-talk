# File:     make-readme.ps1 - generates ..\readme.md, from .\slides\*.png
# Exec:     pwsh.exe -executionPolicy bypass -file make-readme.ps1
# Author:   Ben Mullan (c) 2026

@"
# embedded Chromium everywhere!
_a security look at msedgewebview2 + CDP_
<br/><br/>

## useful bits...
- [electron-test.exe](https://github.com/BenMullan/ece-talk/blob/main/src/electron-test/)
- [example-cdp-commands.txt](https://github.com/BenMullan/ece-talk/blob/main/src/example-cdp-commands.txt)
- [start-debuggable-chrome.cmd](https://github.com/BenMullan/ece-talk/blob/main/src/start-debuggable-chrome.cmd)
- [msedgewebview2-win32-cpp-demo/](https://github.com/BenMullan/ece-talk/blob/main/src/msedgewebview2-win32-cpp-demo/)
<br/><br/>

## slides...
$(
    dir -path (join-path $psScriptRoot "slides") -filter "*.png" -file | sort { [int]($_.baseName -replace "\D+", "") } | % {
        "`n`n<br/>`n<img src=""https://github.com/BenMullan/ece-talk/blob/main/res/slides/$($_.name)?raw=true"" width=""100%"" />`n<br/>`n#"
    }
)

"@ | out-file -filePath "$psScriptRoot\..\readme.md"