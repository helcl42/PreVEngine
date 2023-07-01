@echo off
for /d %%d in (*.*) do (
    echo ===========================================
    echo = Compiling shaders in: "%%d"
    echo ===========================================
    cd %%d
    for %%f in (*.bat) do (
        if "%%~xf"==".bat" < NUL call %%f
    )
    cd ..
)