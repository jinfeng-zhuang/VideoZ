set DIR="%cd%"

for /R %DIR% %%f in (*.mp4) do (
    echo %%f
    ffmpeg -i "%%f" 2>> output.txt
)

pause