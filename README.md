# Plans Converter
Plans Converter is an app to convert between major flight plan formats, with the possibility to convert between Garmin FPL (mainly used by SkyVector), FSX/P3D/FS2020 PLN, and X-Plane 11 FMS.

## Installation
Go to releases and download the latest release. The pre-compiled versions are provided for Windows and Linux.

## Build
You need to have Qt 5 and a static version of [pugixml](https://pugixml.org).
Create a folder named 'lib' and put the compiled library file in it. Create a folder named 'include' inside the 'lib' folder and put the include files in it.

### Windows:
Assuming Qt 5 binaries are in PATH, and using mingw32:

```
git clone https://github.com/abdullah-radwan/PlansConverter.git PlansConverter
cd PlansConverter
qmake
mingw32-make
mingw32-make clean
```

### Linux:

```
git clone https://github.com/abdullah-radwan/PlansConverter.git ~/PlansConverter
cd ~/PlansConverter
qmake
make
make clean
chmod u+x PlansConverter
./PlansConverter
```

## Credits
[Kognise](https://github.com/kognise/flight-plan-converter) for PDMS/Decimal coordinates conversion code.

## About
This app is based on Qt 5 and pugixml. It's under GPL v3 license.

Any suggestions and contributes are welcomed.

Copyright © Abdullah Radwan
