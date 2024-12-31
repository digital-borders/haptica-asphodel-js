Hello Vic, i managed to have a working machine with the sensor attached.
please follow this instructions to log into the machine:
- Install RustDesk client
- Click on the three dots on the right of ID
- Click Network
- Click Server ID/Relay
- ID server: rustdesk.flowcomposer.it
- Relay server: rustdesk.flowcomposer.it
- Key: YamncdF7aWRk68n2oRWgE3DB3UtYGI4LLGDMYDPgfZs=
- Now you can connect to my machine with these info:
- ID: 48449445
- Pwd: "Lenovo Legion Vic2" (withouth quotes)
You will find Visual studio code installed, node installed and the repo already in the Desktop with folder name asphodel



Do these on all OSes for us to be sure whether it will work.
first install all dependencies

on the binding.gyp set the correct path for the library first
```gyp
      'libraries': [
          # switch library to use here
          "<(module_root_dir)/asphodel/builds/linux/libasphodel.so"
      ],
```


```
node-gyp configure build
node-gyp build
npm run test

```

```bash
node:internal/modules/cjs/loader:1452
  return process.dlopen(module, path.toNamespacedPath(filename));
                 ^

Error: libasphodel.so: cannot open shared object file: No such file or directory
    at Module._extensions..node (node:internal/modules/cjs/loader:1452:18)
    at Module.load (node:internal/modules/cjs/loader:1197:32)
    at Module._load (node:internal/modules/cjs/loader:1013:12)
    at Module.require (node:internal/modules/cjs/loader:1225:19)
    at require (node:internal/modules/helpers:177:18)
    at Object.<anonymous> (/home/gg/Desktop/haptica-asphodel-js/binding.js:1:13)
    at Module._compile (node:internal/modules/cjs/loader:1356:14)
    at Module._extensions..js (node:internal/modules/cjs/loader:1414:10)
    at Module.load (node:internal/modules/cjs/loader:1197:32)
    at Module._load (node:internal/modules/cjs/loader:1013:12) {
  code: 'ERR_DLOPEN_FAILED'
}

Node.js v18.19.1
```

When I encounter this error this is how I am fixing it on linux:
```bash
 export LD_LIBRARY_PATH=//home/gg/Desktop/asphodel/build/:$LD_LIBRARY_PATH

```
Do the equivalent for other OSes