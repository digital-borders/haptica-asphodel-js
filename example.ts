import { USBInit, USBDeInit, USBFindDevices } from "./haptica-asphodel-js";

const main = async () => {
  console.log("Hello, World!");
  //asp.USBInit();
  USBInit();
  console.log("Hello, World!");
  // search for devices
  const devices = USBFindDevices(5);
  console.log("dev", devices);

  for (const device of devices) {
    console.log("Device:", device);
    device.open();
    const num = device.getChannelCount();

    console.log("Channels:", num);

    const boardInfo = device.getBoardInfo();

    console.log("Board Info:", boardInfo);

    const packetCount = 0;
    const transferCount = 0;
    const timeout = 1000;
    const responseTime = 0.1;
    const bufferTime = 0.5;

    // TODO: This give error ASPHODEL_BAD_PARAMETER
    device.startStreamingPackets(
      packetCount,
      transferCount,
      timeout,
      (status, stream_data, packet_size, packet_count) => {
        console.log("Status:", status);
        console.log("Stream Data:", stream_data);
        console.log("Packet Size:", packet_size);
        console.log("Packet Count:", packet_count);
      }
    );

    device.enableStream(0, true);

    //wait 20 seconds
    await new Promise((resolve) => setTimeout(resolve, 20000));

    device.enableStream(0, false);

    device.stopStreamingPackets();

    // for (let i = 0; i < num; i++) {
    //   console.log("Channel:", i);
    //   console.log("Channel Name:", device.getChannelName(i));
    //   console.log("Channel Info:", device.getChannelInfo(i));
    //   console.log("Channel Coefficients:", device.getChannelCoefficients(i, 5));

    // }

    // open channel 1

    device.close();
  }

  USBDeInit();
  //asp.USBDeInit();
};

main();
