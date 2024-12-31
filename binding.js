const asp = require("./build/Release/haptica-asphodel-js.node")

asp.USBInit()
//asp.USBPollDevices(10)
//console.log("backend version: ", asp.USBGetBackendVersion())
//let devices = asp.USBFindDevices();
//devices[0].close();

console.log(asp.USBDevicesSupported())
asp.USBDeinit()

