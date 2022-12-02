# Piggy2
`Piggy2` is a simple application which makes loading a FRIDA script into a target process simple. Whilst this replicates the functionality of the FRIDA cli tools, they have a dependency on python. Since this isn't always available or practical for embedded targets, `Piggy2` instead is implemented as a native C program using the `frida-core devkit`.

# Building
```bash
$ make
...
```

# Testing
This repository includes the following sample script in `piggy.js`.
```js
console.log(`piggy.js: PID ${Process.id}`);
```

The tool can be tested as follows. The output from the script can be seen inline as part of the output:
```bash
$ make run
build/piggy2 /bin/ls piggy.js
[*] Found device: "Local System"
[*] spawning: /bin/ls
[*] target_pid: 757449
[*] Attached device
[*] Attached
[*] Created options (external)
[*] Script path: /home/jon.w/git/piggy2/piggy.js
[*] Read script
[*] Created script
[*] Connected message callback
[*] piggy.js: PID 757449
[*] Loaded script
[*] Script loaded
```

# Usage
```bash
 ./build/piggy2 --help
Usage: ./build/piggy2 (<pid> | <name>) <script>
```
