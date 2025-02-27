import { Job } from "bullmq";

const acquireDataProcessor = async (job: Job) => {
  console.log("acquireDataWorker");
  return {
    rawValueFromSensor: 5000,
  };
};

export default acquireDataProcessor;
