import { USBDeInit, USBFindDevices, USBInit } from "../haptica-asphodel-js";

USBInit();




async function main() {

    const devices = USBFindDevices(1);
    console.log(`Found ${devices.length} devices!`)
    devices.forEach((device) => {
    device.open()
    

    if (device.supportsRadioCommands()) {
        console.log("scanning fo remotes devices....")
        
        device.startRadioScan()
        

        setTimeout(()=>{
            
            device.stopRadio()
            let serials = device.getRadioScanResults(255)
            let sorted = serials.sort();
            console.log("Radio Scan results: ", sorted)

            device.close()
        }, 1000)
        
    }
    
})
}


main().then(()=>{
    USBDeInit();
})