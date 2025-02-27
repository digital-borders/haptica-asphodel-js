import { Job } from "bullmq";

const sendMqttProcessor = async (job: Job) => {
  const childrenValues = await job.getChildrenValues();

  console.log("sendMqttWorker: childrenValues", childrenValues);
  return {
    mqtt: "mqtt",
  };
};

export default sendMqttProcessor;
