# Raytracer-v2.0
Raytracer

To compile first install cmake (sudo apt-get install cmake)

Open terminal and type:

```
cd folder/with/CMakelists.txt
cmake .
make
```

To run (also in terminal):

```
./rt2 -o "path/to/objects.txt" -l "path/to/lights.txt" -s "path/to/settings.txt" -f "path/to/file.avs" -t 4
```

Arguments:

*  -o: File with objects description (required)
*  -l: File with lights description (required)
*  -s: File with settings description (required)
*  -f: Output file avs (required)
*  -t: number of threads to use (defaults to 1)

To display the image with imagemagick (sudo apt-get install imagemagick):

```
convert /path/to/file.avs path/to/image.png
display path/to/image.png
```
