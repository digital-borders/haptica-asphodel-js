import { Job } from "bullmq";
import {
  ApdBuilder,
  ASPHODEL_PROTOCOL_TYPE_BOOTLOADER,
  ChannelDecoder,
  ChannelInfo,
  createDeviceDecoder,
  Device,
  DeviceDecoder,
  deviceToString,
  getStreamingCounts,
  StreamAndChannels,
  StreamInfo,
  TCPDeinit,
  TCPFindDevices,
  TCPInit,
  UnitFormatter,
  USBDeInit,
  USBFindDevices,
  USBInit,
} from "../haptica-asphodel-js";
import * as mqtt from "mqtt";

type ChannelClosure = {
  unit_formatter: UnitFormatter;
  counter_time_scale: number;
  sample_time_scale: number;
};

type DeviceInfo = {
  decoder: DeviceDecoder;
  stream_count: number;
  info_array: StreamAndChannels[];
  serial_number: string;
};

function createChannelClosure(
  serial_number: string,
  stream_info: StreamInfo,
  channel_info: ChannelInfo,
  channel_decoder: ChannelDecoder
) {
  let strinfo = stream_info.getInfo();
  let chinfo = channel_info.getInfo();

  let channel_closure: ChannelClosure = {
    unit_formatter: new UnitFormatter(
      chinfo.unit_type,
      chinfo.minimum,
      chinfo.maximum,
      chinfo.resolution,
      true
    ),
    counter_time_scale: 0.0,
    sample_time_scale: 0.0,
  };

  if (strinfo.rate != 0.0 && channel_decoder.getSamples() != 0) {
    channel_closure.counter_time_scale = 1 / strinfo.rate;
    channel_closure.sample_time_scale =
      channel_closure.counter_time_scale / channel_decoder.getSamples();
  }

  return channel_closure;
}

function make2DArray(
  array: Float64Array,
  rows: number,
  columns: number,
  out: Float64Array[]
) {
  if (array.length != rows * columns) throw "length mismatch";
  for (let i = 0; i < columns; i++) {
    let beg = i * rows;
    out.push(array.slice(beg, beg + rows));
  }
  return out;
}

class DeviceData {
  //serial_number: string
  channel: number;
  streams: Float64Array[][];
  unit_formatter: UnitFormatter;
  subchannels: string[];

  constructor(channel: number) {
    this.streams = [];
    this.channel = channel;
    this.subchannels = [];
  }
}

function createDeviceInfo(device: Device, out: DeviceData | null): DeviceInfo {
  let stream_count = device.getStreamCount();
  let stream_infos: StreamAndChannels[] = [];
  for (let i = 0; i < stream_count.count; i++) {
    let stream = device.getStream(i);
    let channels: ChannelInfo[] = [];
    let stream_info = stream.getInfo();

    if (stream_info.channel_count == 0) {
      throw new Error(`Error: stream ${i} has 0 channels`);
    }

    for (let j = 0; j < stream_info.channel_count; j++) {
      let channel_index = stream_info.channel_index_list[j];
      let channel = device.getChannel(channel_index);
      channels.push(channel);
    }
    stream_infos.push(new StreamAndChannels(i, stream, channels));
  }

  let decoder = createDeviceDecoder(
    stream_infos,
    stream_count.filler_bits,
    stream_count.id_bits
  );

  let serial_number = device.getSerialNumber();

  decoder.setUnknownIDCallback((id) => {
    console.log(`Unknown stream id ${id} on ${serial_number}`);
  });

  let decs = decoder.getDecoders();

  decs.forEach((dec, i) => {
    dec.setLostPacketCallback((current, last) => {
      console.log(
        `DECODING:  Lost ${
          current - last - 1
        } packets from ${serial_number} stream ${i}`
      );
    });

    dec.getDecoders().forEach((channel_decoder, j) => {
      if (out == null) return;
      channel_decoder.setDecodeCallback(
        (counter, data, samples, subchannels) => {
          if (j == out.channel) {
            let channel_info = stream_infos[i].getChannelInfos()[j];
            var chinfo = channel_info.getInfo();
            var unit_formatter = new UnitFormatter(
              chinfo.unit_type,
              chinfo.minimum,
              chinfo.maximum,
              chinfo.resolution,
              true
            );
            channel_decoder.setConversionFactor(
              unit_formatter.getConversionScale(),
              unit_formatter.getConversionOffset()
            );
            out.unit_formatter = unit_formatter;
            out.subchannels = channel_decoder.getSubChannelNames();
            let arr: Float64Array[] = [];
            make2DArray(data, samples, subchannels, arr);
            out.streams.push(arr);
          }
        }
      );
    });
  });

  return {
    stream_count: stream_count.count,
    serial_number: device.getSerialNumber(),
    decoder: decoder,
    info_array: stream_infos,
  };
}

function hasRadioScanPower(device: Device) {
  try {
    device.getRadioScanPower(new Uint32Array([0]));
    return true;
  } catch (e) {
    return false;
  }
}

function collectScanResults(device: Device) {
  var results = device.getRadioExtraScanResults(255);
  var scan_powers = new Map();
  if (hasRadioScanPower(device)) {
    var power_max_queries = Math.min(
      Math.floor(device.getMaxOutgoingParamLength() / 4),
      device.getMaxIncomingParamLength()
    );

    for (let i = 0; i < results.length; i++) {
      if (i == power_max_queries) break;
      var result_subset = results.slice(i);
      var serials: number[] = [];
      result_subset.forEach((r) => serials.push(r.serial_number));
      var powers = device.getRadioScanPower(new Uint32Array(serials));

      for (let sn = 0; sn < serials.length; sn++) {
        if (powers[sn] != 0x7f) {
          scan_powers.set(serials[sn], powers[sn]);
        }
      }
    }
  }
  var scans: {
    serial_number: number;
    bootloader: boolean;
    asphodel_type: number;
    device_mode: number;
    scan_strength: number;
  }[] = [];
  results.forEach((r) => {
    var power = scan_powers.get(r.serial_number);
    scans.push({
      serial_number: r.serial_number,
      bootloader: (r.asphodel_type & ASPHODEL_PROTOCOL_TYPE_BOOTLOADER) != 0,
      asphodel_type: r.asphodel_type,
      device_mode: r.device_mode,
      scan_strength: power,
    });
  });

  return scans;
}

function doRadioScan(device: Device) {
  console.log("scanning fo remotes devices");
  device.startRadioScan();
  sleep(1000);
  var scans = collectScanResults(device);
  var sensors: Device[] = [];

  scans.forEach((scan) => {
    var remote = device.getRemoteDevice();
    remote.open();
    try {
      if (!scan.bootloader) {
        device.connectRadio(scan.serial_number);
      } else {
        device.connectRadioBoot(scan.serial_number);
      }

      remote.waitForConnect(1000);

      if (remote.getSerialNumber() == "") {
        throw new Error("No serial number when fetching device info");
      }

      sensors.push(remote);
    } catch (e) {
      remote.close();
      remote.free();
      sensors.forEach((sen) => {
        sen.close();
        sen.free();
      });
      throw e;
    }
  });
  return sensors;
}

async function checkSensorsConnected(device: Device) {
  let remote_devices: Device[] = [];
  if (device.supportsRadioCommands()) {
    while (true) {
      try {
        remote_devices = doRadioScan(device);
        break;
      } catch (e) {}
    }
  }

  return remote_devices;
}

function sleep(millis: number) {
  var bgn = Date.now();
  while (Date.now() - bgn < millis) {}
}

function startStreams(device: Device, active_streams: number[]) {
  var stream_ids = active_streams.sort();
  stream_ids.forEach((id) => {
    device.warmUpStream(id, true);
  });
  var warm_up_time = 0;
  stream_ids.forEach((id) => {
    var stream = device.getStream(id).getInfo();
    if (stream.warm_up_delay > warm_up_time) {
      warm_up_time = stream.warm_up_delay;
    }
  });

  sleep(warm_up_time * 1000);

  stream_ids.forEach((id) => {
    device.enableStream(id, true);
    device.warmUpStream(id, false);
  });
}

function stopStreams(device: Device, active_streams: number[]) {
  active_streams.forEach((id) => {
    try {
      device.enableStream(id, false);
    } catch (e) {}
  });
}

async function aquireDataSaving(
  device: Device,
  time: number,
  schedule_id: string,
  job: Job,
  apd_path: string,
  channel: number
) {
  console.log("acquire for: ", time);
  var out = new DeviceData(channel);
  var device_info = createDeviceInfo(device, out);

  var streams_to_activate: number[] = [];
  for (let i = 0; i < device_info.stream_count; i++) {
    streams_to_activate.push(i);
  }

  let response_time = 0.05; // 100 milliseconds
  let buffer_time = 0.5; // 500 milliseconds
  let timeout = 1000; // 1000 milliseconds
  let streaming_counts = getStreamingCounts(
    device_info.info_array,
    response_time,
    buffer_time,
    timeout
  );

  let apd = new ApdBuilder(
    device,
    streams_to_activate,
    streaming_counts,
    schedule_id,
    apd_path
  );

  try {
    device.startStreamingPackets(
      streaming_counts.packet_count,
      streaming_counts.transfer_count,
      streaming_counts.timeout,
      (status, stream_data, packet_size, packet_count) => {
        if (status == 0) {
          apd.update(stream_data);
          for (let packet = 0; packet < packet_count; packet++) {
            device_info.decoder.decode(stream_data.slice(packet * packet_size));
          }
        } else if (status == -7) {
          console.log("STREAMING: Timeout");
        } else {
          console.log(`STREAMING: Bad status ${status}`);
        }
      }
    );

    startStreams(device, streams_to_activate);

    await new Promise((resolve) => {
      let begin = Date.now();
      var progress = 0;
      function loop() {
        if (Date.now() - begin < time) {
          try {
            device.poll(100);
          } catch (e) {
            console.error(e);
          }
          setImmediate(loop);
          return;
        }
        device.stopStreamingPackets();
        device.poll(1000);
        stopStreams(device, streams_to_activate);
        resolve(true);
      }
      loop();
    });
  } catch (e) {
    await apd.final(null);
    throw e;
  }
  return { apd: apd, out: out };
}

function checkAllConnectedReceivers() {
  return USBFindDevices().concat(TCPFindDevices());
}

import * as fs from "fs";
import { AquireConfig, DeviceConfig } from "./cycling";

function init() {
  USBInit();
  TCPInit();
}

function deinit() {
  USBDeInit();
  TCPDeinit();
}

function calculateMean(data: number[]) {
  return data.reduce((prev, curr) => prev + curr) / data.length;
}

function calculateVariance(mean: number, data: number[]) {
  var sqdiff = data.map((value) => Math.pow(value - mean, 2));
  return calculateMean(sqdiff);
}

function calculateStandardDeviation(mean: number, data: number[]) {
  return Math.sqrt(calculateVariance(mean, data));
}

function flattenByCol(data: Float64Array[][], col: number) {
  var out: number[] = [];
  data.forEach((row) => {
    row.forEach((col) => {
      col.forEach((n) => out.push(n));
    });
  });
  return out;
}

function calculateChannelStats(out: DeviceData) {
  var means: {
    subchannel: string;
    mean: {
      value: number;
      bare: string;
      ascii: string;
      html: string;
    };
    stddev: {
      value: number;
      bare: string;
      ascii: string;
      html: string;
    };
  }[] = [];
  out.subchannels.forEach((subchannel, i) => {
    var sub_channel_data = flattenByCol(out.streams, i);
    var mean = calculateMean(sub_channel_data);
    var stddev = calculateStandardDeviation(mean, sub_channel_data);
    means.push({
      subchannel,
      mean: {
        value: mean,
        bare: out.unit_formatter.FormatBare(mean),
        ascii: out.unit_formatter.FormatAscii(mean),
        html: out.unit_formatter.FormatHtml(mean),
      },
      stddev: {
        value: stddev,
        bare: out.unit_formatter.FormatBare(stddev),
        ascii: out.unit_formatter.FormatAscii(stddev),
        html: out.unit_formatter.FormatHtml(stddev),
      },
    });
  });
  return means;
}

function closeSensors(sensors: Device[]) {
  sensors.forEach((sensor) => {
    console.log("closing sensor: ", sensor.getSerialNumber());
    sensor.close();
    sensor.free();
  });
}

async function doWorkOnDevice(job: any) {
  init();
  var config: AquireConfig = job.data;

  var devices = checkAllConnectedReceivers();

  var actual_device = devices.find((dev) => {
    try {
      dev.open();
      return dev.getSerialNumber() == config.device_config.receiver;
    } catch (e) {
      return false;
    } finally {
      dev.close();
    }
  });
  if (!actual_device)
    throw new Error(`device ${config.device_config.receiver} not found.`);
  actual_device.open();
  var sensors: Device[] = [];

  try {
    sensors = await checkSensorsConnected(actual_device as Device);
  } catch (e) {
    actual_device.close();
    throw e;
  }

  sensors.forEach((sensor) => {
    console.log("found sensor: ", sensor.getSerialNumber());
  });
  var actual_sensor = sensors.find(
    (sensor) => sensor.getSerialNumber() == config.device_config.sensor
  );

  if (actual_sensor == undefined) {
    closeSensors(sensors);
    actual_device.close();
    throw new Error(
      `sensor ${config.device_config.sensor} not connected to ${(
        actual_device as Device
      ).getSerialNumber()}.`
    );
  }

  var results: any = null;
  try {
    results = await aquireDataSaving(
      actual_sensor as Device,
      config.device_config.duration,
      "demo",
      job,
      `${config.base_path}/${config.device_config.receiver}-${config.device_config.sensor}.apd`,
      config.device_config.sensorChannel
    );

    var stats = calculateChannelStats(results.out);
    console.log("stats: test", stats);

    for (var op of config.device_config.operations) {
      console.log("operation: ", op.operation, op);
      switch (op.operation.toLowerCase()) {
        case "save":
          const params: {
            filename: string; // The path to save the data
            type: "raw" | "processed"; // The type of the data: raw is the .apd file with the raw data from sensor at full speed
          } = op.params;

          if (params.type == "raw") {
            console.log("saving raw data to: ", params.filename);
            await results.apd.final(`${config.base_path}/${params.filename}-${Date.now()}`);
          } else if (params.type == "processed") {
            fs.writeFileSync(
              `${config.base_path}/${params.filename}.json`,
              JSON.stringify(stats)
            );
          } else throw new Error(`processed output not implemented.`);
          break;
        case "stddev":
        case "mean":
          await new Promise((resolve, reject) => {
            const client = mqtt.connect(
              `${config.mqtt_config.host}:${config.mqtt_config.port}`,
              {
                connectTimeout: 1000,
                username: config.mqtt_config.username,
                password: config.mqtt_config.password,
              }
            );

            client.on("error", (e) => {
              console.log("error: ", e);
              reject(e);
            });

            client.on("connect", () => {
              console.log("Connected to MQTT broker");
              client.publish(
                config.device_config.mqttTopic,
                JSON.stringify(stats),
                (err) => {
                  if (!err) {
                    console.log("message published");
                    client.end((err) => {
                      if (err) {
                        console.log("failed to disconnect");
                        reject(err);
                      } else {
                        console.log("Disconnected successfully");
                        resolve(true);
                      }
                    });
                  } else {
                    console.log("message publish failed");
                    reject(err);
                  }
                }
              );
            });
          });
          break;
        default:
          throw `unhandled operation: ${op.operation}.`;
      }
    }
  } catch (e) {
    throw e;
  } finally {
    console.log("closing sensors");
    closeSensors(sensors);
    console.log("closing device");
    actual_device.close();
    if (results) {
        console.log("finalizing apd", results);
      await results.apd.final(null);
    }
  }
}

export default async (job) => {
  if (job) {
    console.log(`worker working on ${job.name}`);
    try {
      await doWorkOnDevice(job);
    } catch (e) {
      console.error(e);
      throw e;
    }
    console.log(`worker finished working on ${job.name}`);
  }
};
