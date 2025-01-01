const asp = require("./build/Release/haptica-asphodel-js.node")

asp.USBInit()
//asp.USBPollDevices(10)
//console.log("backend version: ", asp.USBGetBackendVersion())
let devices = asp.USBFindDevices();
//devices[0].close();

console.log(devices)

//devices[0].writeNVMRaw(0, new Uint8Array([1,2,3,4,5,6]))

//const res = devices[0].doTranfer(1, new Uint8Array([1,2,3,4,5]), (status, buf)=>{
//    console.log("called from c++")
//    console.log(status, buf)
//})

//console.log(res)

devices[0].setErrorCallback((status)=>{
    console.log("status: ", status)
})


asp.USBDeinit()

